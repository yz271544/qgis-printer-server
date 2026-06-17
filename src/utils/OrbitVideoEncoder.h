//
// Created by Claude for jingweiprinter
//

#ifndef JINGWEIPRINTER_ORBITVIDEOENCODER_H
#define JINGWEIPRINTER_ORBITVIDEOENCODER_H

#include <QImage>
#include <QString>
#include <QSize>

/**
 * \brief Wraps FFmpeg for video encoding of orbit animation frames.
 *
 * FFmpeg is invoked as a subprocess once at the end of the export,
 * reading pre-rendered JPEG frames from a temporary directory.
 *
 * FFmpeg must be available in PATH at runtime. If not found, the
 * encoder returns a clear error message and the caller falls back
 * to image-sequence export.
 *
 * FFmpeg command used:
 *   ffmpeg -y -framerate {fps} -i {tempDir}/frame_%04d.jpg \\
 *     -c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p \\
 *     -movflags +faststart -s {width}x{height} {outputPath}
 */
class OrbitVideoEncoder
{
  public:

    /**
     * Opens a temporary directory and prepares for frame encoding.
     *
     * \param outputPath  Path for the final MP4 file.
     * \param fps         Frame rate.
     * \param frameSize   Resolution for FFmpeg -s flag.
     * \param quality     JPEG quality for intermediate frames (1-100).
     *
     * \return true if the temp directory was created successfully.
     */
    bool start( const QString &outputPath, int fps, const QSize &frameSize, int quality );

    /**
     * Writes a JPEG frame to the temporary directory.
     *
     * \param frameIndex  Zero-based frame index (used in filename).
     * \param image       The rendered QImage.
     *
     * \return true on success.
     */
    bool encodeFrame( int frameIndex, const QImage &image );

    /**
     * Runs FFmpeg to produce the final video from the JPEG frames,
     * then cleans up the temp directory (if deleteTemp is true).
     *
     * \param deleteTemp  If true, the temp directory is removed after success.
     *
     * \return true if FFmpeg exited with code 0.
     */
    bool finish( bool deleteTemp = true );

    /**
     * Returns the last error message.
     */
    QString errorString() const { return mError; }

    /**
     * Checks whether FFmpeg is available on this system at runtime.
     * Tries running "ffmpeg -version" from PATH.
     */
    static bool isFfmpegAvailable();

  private:

    QString mTempDir;
    QString mOutputPath;
    int mFps = 30;
    QSize mFrameSize;
    int mQuality = 90;
    int mFrameCount = 0;
    QString mError;
};

#endif // JINGWEIPRINTER_ORBITVIDEOENCODER_H