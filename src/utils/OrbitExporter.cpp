//
// Created by Claude for jingweiprinter
//

#include "utils/OrbitExporter.h"
#include "utils/OrbitVideoEncoder.h"

#include <qgs3dutils.h>
#include <qgsoffscreen3dengine.h>
#include <qgs3dmapscene.h>
#include <qgs3danimationsettings.h>
#include <qgscameracontroller.h>
#include <qgsfeedback.h>

#include <QtMath>
#include <QDir>
#include <QUuid>

// -------------------------------------------------------------------------

OrbitExporter::OrbitExporter( QObject *parent )
  : QObject( parent )
{
}

OrbitExporter::~OrbitExporter() = default;

// -------------------------------------------------------------------------

QVector<Qgs3DAnimationSettings::Keyframe>
OrbitExporter::computeKeyframes( const OrbitExporterConfig &config ) const
{
  const double R = config.orbitRadius;
  const double H = config.centerPoint.z();   // camera height above orbit plane

  // Camera distance from the orbit center (straight-line).
  // Camera sits at (cx + R*cos(yaw), cy + R*sin(yaw), cz + H).
  const double dist = std::sqrt( R * R + H * H );

  // pitch is fixed across all frames (user-supplied value)
  const float pitch = config.pitch;

  // Optional start angle offset
  const float startAngle = config.startAngle;

  QVector<Qgs3DAnimationSettings::Keyframe> keyframes;
  keyframes.reserve( config.frameCount );

  for ( int i = 0; i < config.frameCount; ++i )
  {
    const double yawDeg = startAngle + i * ( 360.0 / config.frameCount );

    // The lookAtPoint (center) is constant — camera always faces it.
    keyframes.append( {
      /* .time  */ 0.0f,
      /* .point */ QgsVector3D(
          static_cast<float>( config.centerPoint.x() ),
          static_cast<float>( config.centerPoint.y() ),
          static_cast<float>( config.centerPoint.z() ) ),
      /* .dist  */ static_cast<float>( dist ),
      /* .pitch */ pitch,
      /* .yaw   */ static_cast<float>( yawDeg )
    } );
  }

  return keyframes;
}

// -------------------------------------------------------------------------

bool OrbitExporter::setupEngine( const QSize &outputSize )
{
  mEngine = std::make_unique<QgsOffscreen3DEngine>();
  mEngine->setSize( outputSize );

  // Scene takes ownership of the camera controller
  mScene = std::make_unique<Qgs3DMapScene>( *mMapSettings, mEngine.get() );
  mEngine->setRootEntity( mScene.get() );

  // Required for render-capture node to produce frames (same as exportAnimation)
  mEngine->renderSettings()->setRenderPolicy(
      Qt3DRender::QRenderSettings::RenderPolicy::Always );

  return true;
}

// -------------------------------------------------------------------------

QImage OrbitExporter::renderFrame()
{
  return Qgs3DUtils::captureSceneImage( *mEngine, mScene.get() );
}

// -------------------------------------------------------------------------

bool OrbitExporter::saveFrame( int frameIndex, const QImage &image )
{
  const QString name = QStringLiteral( "frame_%1.png" )
      .arg( frameIndex + 1, 4, 10, QLatin1Char( '0' ) );
  const QString path = QDir( mFramesOutputDir ).filePath( name );

  if ( !image.save( path ) )
  {
    mError = QStringLiteral( "Failed to save frame: %1" ).arg( path );
    return false;
  }
  return true;
}

// -------------------------------------------------------------------------

bool OrbitExporter::export( const OrbitExporterConfig &config,
                            Qgs3DMapSettings &mapSettings,
                            QgsFeedback *feedback,
                            QString &error )
{
  mConfig      = config;
  mMapSettings = &mapSettings;
  mFeedback    = feedback;
  mProgress    = 0.0;
  mError.clear();

  // -- 1. Compute orbit keyframes --
  const auto keyframes = computeKeyframes( config );
  if ( keyframes.size() < 2 )
  {
    error = QStringLiteral( "Need at least 2 frames." );
    return false;
  }

  // -- 2. Determine output mode --
  const bool isVideoOutput = config.outputPath.toLower().endsWith( u".mp4" )
                           || config.outputPath.toLower().endsWith( u".mkv" );

  if ( isVideoOutput )
  {
    // Create a temp directory for JPEG frames
    mFramesOutputDir = QDir::temp().filePath( u"jingwei_orbit_"_s + QUuid::createUuid().toString() );
    if ( !QDir().mkpath( mFramesOutputDir ) )
    {
      error = QStringLiteral( "Could not create temporary directory for frames." );
      return false;
    }

    mVideoEncoder = std::make_unique<OrbitVideoEncoder>();
    if ( !mVideoEncoder->start( config.outputPath,
                                config.framesPerSecond,
                                config.outputSize,
                                config.jpegQuality ) )
    {
      error = mVideoEncoder->errorString();
      mVideoEncoder.reset();
      return false;
    }
  }
  else
  {
    // Image sequence mode
    if ( config.outputPath.isEmpty() )
      mFramesOutputDir = QDir::temp().filePath( u"jingwei_orbit_"_s + QUuid::createUuid().toString() );
    else
      mFramesOutputDir = config.outputPath;

    if ( !QDir().mkpath( mFramesOutputDir ) )
    {
      error = QStringLiteral( "Could not create output directory: %1" ).arg( mFramesOutputDir );
      return false;
    }
  }

  // -- 3. Setup offscreen engine --
  if ( !setupEngine( config.outputSize ) )
  {
    error = QStringLiteral( "Failed to initialize 3D engine." );
    return false;
  }

  // -- 4. Main render loop --
  const int total = keyframes.size();
  for ( int i = 0; i < total; ++i )
  {
    if ( feedback && feedback->isCanceled() )
    {
      error = QStringLiteral( "Export canceled by user." );
      return false;
    }

    mProgress = ( i * 100.0 ) / total;
    emit progressChanged( mProgress );

    // Set camera for this frame
    const auto &kf = keyframes[i];
    mScene->cameraController()->setLookingAtMapPoint( kf.point, kf.dist, kf.pitch, kf.yaw );

    // Capture frame
    const QImage img = renderFrame();
    if ( img.isNull() )
    {
      error = QStringLiteral( "Frame capture returned null image at frame %1." ).arg( i );
      return false;
    }

    // Save frame
    if ( !saveFrame( i, img ) )
    {
      error = mError;
      return false;
    }

    emit frameRendered( i );
  }

  // -- 5. Finalize video encoding --
  if ( mVideoEncoder )
  {
    if ( !mVideoEncoder->finish( config.deleteTempFrames ) )
    {
      error = mVideoEncoder->errorString();
      return false;
    }
  }

  mProgress = 100.0;
  emit progressChanged( mProgress );
  return true;
}

// -------------------------------------------------------------------------

void OrbitConfig::loadFromVariantMap( const QVariantMap &map )
{
    auto get = [&]( const QString &key, auto &value ) {
        if ( map.contains( key ) )
            value = map[key].value<decltype( value )>();
    };
    auto getStr = [&]( const QString &key, QString &value ) {
        if ( map.contains( key ) )
            value = map[key].toString();
    };
    auto getBool = [&]( const QString &key, bool &value ) {
        if ( map.contains( key ) )
            value = map[key].toBool();
    };
    auto getFloat = [&]( const QString &key, float &value ) {
        if ( map.contains( key ) )
            value = map[key].toFloat();
    };

    getBool( u"enable"_s,               enable );
    get(    u"output_width"_s,           outputWidth );
    get(    u"output_height"_s,          outputHeight );
    get(    u"frame_count"_s,            frameCount );
    get(    u"fps"_s,                    fps );
    get(    u"jpeg_quality"_s,           jpegQuality );
    get(    u"orbit_radius_multiplier"_s, orbitRadiusMultiplier );
    getFloat( u"start_angle"_s,          startAngle );
    getBool( u"delete_temp_frames"_s,    deleteTempFrames );
    getBool( u"auto_compute_pitch"_s,    autoComputePitch );
    getFloat( u"fallback_pitch"_s,       fallbackPitch );
    getStr(  u"output_format"_s,         outputFormat );
    getBool( u"process_events"_s,        processEvents );
}

// -------------------------------------------------------------------------

double OrbitParams::cameraToLookAtDistance2D() const
{
    const double dx = mCamera.cameraLongitude - mCamera.centerLongitude;
    const double dy = mCamera.cameraLatitude  - mCamera.centerLatitude;
    return std::sqrt( dx * dx + dy * dy );
}

double OrbitParams::computedOrbitRadius() const
{
    return cameraToLookAtDistance2D() * mConfig.orbitRadiusMultiplier;
}

float OrbitParams::computedPitch() const
{
    if ( !mConfig.autoComputePitch )
        return mConfig.fallbackPitch;

    const double R2d = computedOrbitRadius();
    const double H   = mCamera.cameraHeight - mCamera.centerHeight;

    if ( R2d < 0.001 )
        return mConfig.fallbackPitch;

    return static_cast<float>( std::atan2( std::abs( H ), R2d ) * 180.0 / M_PI );
}

OrbitExporterConfig OrbitParams::buildExporterConfig( const QString &outputDir ) const
{
    OrbitExporterConfig cfg;

    // centerPoint: look-at point (the orbit center)
    cfg.centerPoint = QgsVector3D(
        mCamera.centerLongitude,
        mCamera.centerLatitude,
        mCamera.centerHeight );

    // orbitRadius: camera-to-lookAt 2D distance * multiplier
    cfg.orbitRadius = computedOrbitRadius();
    if ( cfg.orbitRadius < 1.0 )
        cfg.orbitRadius = 1000.0;  // fallback minimum

    // pitch: auto-computed or config fallback
    cfg.pitch = computedPitch();

    // frameCount, fps, jpegQuality, startAngle from config
    cfg.frameCount       = mConfig.frameCount;
    cfg.framesPerSecond  = mConfig.fps;
    cfg.jpegQuality      = mConfig.jpegQuality;
    cfg.startAngle       = mConfig.startAngle;
    cfg.deleteTempFrames = mConfig.deleteTempFrames;
    cfg.outputSize       = mConfig.outputSize();

    // Build outputPath: if outputDir ends with .mp4/.mkv/.png, use directly;
    // otherwise treat as directory and append orbit.<format>
    QString outPath = outputDir;
    if ( outPath.isEmpty() )
        outPath = QDir::temp().filePath( u"jingwei_orbit_"_s + QUuid::createUuid().toString() );

    const QString lower = outPath.toLower();
    if ( lower.endsWith( u".mp4" ) || lower.endsWith( u".mkv" )
        || lower.endsWith( u".png" ) || lower.endsWith( u".jpg" ) )
    {
        cfg.outputPath = outPath;
    }
    else
    {
        if ( !outPath.endsWith( u"/"_s ) && !outPath.endsWith( u"\\"_s ) )
            outPath += u"/"_s;
        cfg.outputPath = outPath + u"orbit."_s + mConfig.outputFormat;
    }

    return cfg;
}