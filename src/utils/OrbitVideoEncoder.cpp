//
// Created by Claude for jingweiprinter
//

#include "utils/OrbitVideoEncoder.h"

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
  mTempDir = QDir::temp().filePath( u"jingwei_orbit_"_s + QUuid::createUuid().toString() );
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
  const QString framePattern = QDir( mTempDir ).filePath( u"frame_%04d.jpg"_s );

  QStringList args;
  args.reserve( 20 );

  args << u"-y"_s;                                   // overwrite output
  args << u"-framerate"_s << QString::number( mFps );
  args << u"-i"_s << framePattern;                   // input pattern
  args << u"-c:v"_s << u"libx264"_s;                 // H.264 codec
  args << u"-preset"_s << u"medium"_s;               // speed vs. compression
  args << u"-crf"_s << u"23"_s;                      // quality (0=lossless, 23=default)
  args << u"-pix_fmt"_s << u"yuv420p"_s;             // compatibility
  args << u"-movflags"_s << u"+faststart"_s;         // moov atom at start
  args << u"-s"_s << QStringLiteral( "%1x%2" ).arg( mFrameSize.width() ).arg( mFrameSize.height() );
  args << mOutputPath;

  // Run FFmpeg synchronously
  QProcess p;
  p.setProcessChannelMode( QProcess::SeparateChannels );
  p.start( u"ffmpeg"_s, args );

  const bool finished = p.waitForFinished( 600000 );  // 10-minute timeout
  if ( !finished )
  {
    p.kill();
    mError = QStringLiteral( "FFmpeg timed out after 10 minutes." );
    return false;
  }

  if ( p.exitCode() != 0 )
  {
    mError = u"FFmpeg error (exit code %1): "_s.arg( p.exitCode() )
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
  p.start( u"ffmpeg"_s, { u"-version"_s } );
  const bool ok = p.waitForFinished( 5000 );
  return ok && p.exitCode() == 0;
}