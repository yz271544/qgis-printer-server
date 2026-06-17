//
// Created by Claude for jingweiprinter
//

#include "utils/OrbitVideoEncoder.h"

#include <QImage>
#include <QProcess>
#include <QDir>
#include <QUuid>

// -------------------------------------------------------------------------

bool OrbitVideoEncoder::start( const QString &outputPath,
                               int fps,
                               const QSize &frameSize,
                               int quality )
{
  mOutputPath = outputPath;
  mFps        = fps;
  mFrameSize  = frameSize;
  mQuality    = quality;
  mFrameCount = 0;
  mError.clear();

  // Create a unique temp directory for JPEG frames
  mTempDir = QDir::temp().filePath( QStringLiteral("jingwei_orbit_") + QUuid::createUuid().toString() );
  if ( !QDir().mkpath( mTempDir ) )
  {
    mError = QStringLiteral( "Could not create temporary directory: %1" ).arg( mTempDir );
    return false;
  }

  return true;
}

// -------------------------------------------------------------------------

bool OrbitVideoEncoder::encodeFrame( int frameIndex, const QImage &image )
{
  const QString name = QStringLiteral( "frame_%1.jpg" )
      .arg( frameIndex + 1, 4, 10, QLatin1Char( '0' ) );
  const QString path = QDir( mTempDir ).filePath( name );

  if ( !image.save( path, "JPEG", mQuality ) )
  {
    mError = QStringLiteral( "Failed to save JPEG frame: %1" ).arg( path );
    return false;
  }

  mFrameCount++;
  return true;
}

// -------------------------------------------------------------------------

bool OrbitVideoEncoder::finish( bool deleteTemp )
{
  if ( mFrameCount == 0 )
  {
    mError = QStringLiteral( "No frames were captured." );
    return false;
  }

  if ( !isFfmpegAvailable() )
  {
    mError = QStringLiteral(
      "FFmpeg is not installed or not in PATH. "
      "Please install FFmpeg (e.g. apt install ffmpeg / brew install ffmpeg) "
      "and try again. Alternatively, export as an image sequence instead." );
    if ( deleteTemp )
      QDir( mTempDir ).removeRecursively();
    return false;
  }

  // Frame pattern: frame_0001.jpg, frame_0002.jpg, ...
  const QString framePattern = QDir( mTempDir ).filePath( QStringLiteral("frame_%04d.jpg") );

  QStringList args;
  args.reserve( 20 );

  args << QStringLiteral("-y");                                   // overwrite output
  args << QStringLiteral("-framerate") << QString::number( mFps );
  args << QStringLiteral("-i") << framePattern;                   // input pattern
  args << QStringLiteral("-c:v") << QStringLiteral("libx264");                 // H.264 codec
  args << QStringLiteral("-preset") << QStringLiteral("medium");               // speed vs. compression
  args << QStringLiteral("-crf") << QStringLiteral("23");                      // quality (0=lossless, 23=default)
  args << QStringLiteral("-pix_fmt") << QStringLiteral("yuv420p");             // compatibility
  args << QStringLiteral("-movflags") << QStringLiteral("+faststart");         // moov atom at start
  args << QStringLiteral("-s") << QStringLiteral( "%1x%2" ).arg( mFrameSize.width() ).arg( mFrameSize.height() );
  args << mOutputPath;

  // Run FFmpeg synchronously
  QProcess p;
  p.setProcessChannelMode( QProcess::SeparateChannels );
  p.start( QStringLiteral("ffmpeg"), args );

  const bool finished = p.waitForFinished( 600000 );  // 10-minute timeout
  if ( !finished )
  {
    p.kill();
    mError = QStringLiteral( "FFmpeg timed out after 10 minutes." );
    return false;
  }

  if ( p.exitCode() != 0 )
  {
    mError = QStringLiteral("FFmpeg error (exit code %1): ").arg( p.exitCode() )
           + QString::fromUtf8( p.readAllStandardError() );
    return false;
  }

  if ( deleteTemp )
    QDir( mTempDir ).removeRecursively();

  return true;
}

// -------------------------------------------------------------------------

bool OrbitVideoEncoder::isFfmpegAvailable()
{
  QProcess p;
  p.start( QStringLiteral("ffmpeg"), { QStringLiteral("-version") } );
  const bool ok = p.waitForFinished( 5000 );
  return ok && p.exitCode() == 0;
}