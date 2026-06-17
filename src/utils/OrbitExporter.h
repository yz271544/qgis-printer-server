//
// Created by Claude for jingweiprinter
//

#ifndef JINGWEIPRINTER_ORBITEXPORTER_H
#define JINGWEIPRINTER_ORBITEXPORTER_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QVector>
#include <QVariantMap>
#include <QImage>
#include <memory>

#include <qgsvector3d.h>
#include <qgsfeedback.h>
#include <qgs3danimationsettings.h>
#include <qgsoffscreen3dengine.h>
#include <qgs3dmapscene.h>

class QImage;
class QgsVector3D;
class Qgs3DMapSettings;
class QgsOffscreen3DEngine;
class Qgs3DMapScene;
class Qgs3DAnimationSettings;
class QgsFeedback;
class OrbitVideoEncoder;

/**
 * \brief Final render-time config passed to OrbitExporter::exportVideo().
 *
 * Produced by buildOrbitExporterConfig() from OrbitConfig + OrbitCamera.
 *
 * Example usage:
 * \code
 *   OrbitExporter exporter;
 *   QString error;
 *   bool ok = exporter.exportVideo( cfg, *mapSettings, nullptr, error );
 * \endcode
 */
struct OrbitExporterConfig
{
    /**
     * Center of the orbit in map/world coordinates (x, y, z).
     * This is the lookAtPoint — camera always faces it.
     * Computed from camera.centerLongitude/Latitude/Height.
     */
    QgsVector3D centerPoint;

    /**
     * Horizontal orbit radius in map units.
     * Computed from camera position and orbit_radius_multiplier.
     * Default: 1000.0
     */
    double orbitRadius = 1000.0;

    /**
     * Number of frames to render. 360 frames = 1 degree per step.
     * Recommended: 120-360 for smooth orbit.
     * Default: 120
     */
    int frameCount = 120;

    /**
     * Camera pitch angle in degrees.
     *  0 = top-down, 45 = classic oblique, 90 = horizontal.
     * Computed from camera height and orbit radius, or read from config.
     * Default: 45.0
     */
    float pitch = 45.0f;

    /**
     * Frame rate for the output video. Only used when outputPath
     * ends with .mp4 or .mkv.
     * Default: 30
     */
    int framesPerSecond = 30;

    /**
     * Output file path.
     * - .mp4 / .mkv: invokes FFmpeg to encode video.
     * - .png / .jpg / directory path: saves image sequence.
     * - empty: saves to a temp directory.
     */
    QString outputPath;

    /**
     * Output resolution of each frame.
     * Default: 1920 x 1080
     */
    QSize outputSize = QSize( 1920, 1080 );

    /**
     * JPEG quality for intermediate frames (FFmpeg mode).
     * Range: 1-100. Default: 90.
     */
    int jpegQuality = 90;

    /**
     * If true, temp directory with JPEG frames is deleted after FFmpeg finishes.
     * Default: true
     */
    bool deleteTempFrames = true;

    /**
     * Start angle offset in degrees (adds yaw offset to first frame).
     * Default: 0.0
     */
    float startAngle = 0.0f;
};


/**
 * \brief Camera parameters from the /api/qgz request, used for orbit computation.
 *
 * These are read from Camera3dPosition DTO and used to compute orbitRadius and pitch.
 */
struct OrbitCamera
{
    double cameraLongitude = 0.0;   //!< Camera X position (map units)
    double cameraLatitude  = 0.0;   //!< Camera Y position (map units)
    double cameraHeight    = 0.0;   //!< Camera Z / height above ground
    double cameraDirX      = 0.0;   //!< Camera direction vector X
    double cameraDirY      = 0.0;   //!< Camera direction vector Y
    double cameraDirZ      = 0.0;   //!< Camera direction vector Z
    double centerLongitude = 0.0;   //!< Look-at point X
    double centerLatitude  = 0.0;   //!< Look-at point Y
    double centerHeight    = 0.0;   //!< Look-at point Z
    double headingDeg      = 0.0;   //!< Camera heading / yaw in degrees
    double pitchDeg        = 0.0;   //!< Camera pitch in degrees
};


/**
 * \brief Runtime video export configuration loaded from config.yaml.
 *
 * These are the defaults, optionally overridden by the API request.
 */
struct OrbitConfig
{
    bool    enable              = false;
    int     outputWidth         = 1920;
    int     outputHeight        = 1080;
    int     frameCount          = 120;
    int     fps                 = 30;
    int     jpegQuality         = 90;
    double  orbitRadiusMultiplier = 1.0;
    float   startAngle          = 0.0f;
    bool    deleteTempFrames    = true;
    bool    autoComputePitch    = true;
    float   fallbackPitch       = 45.0f;
    QString outputFormat        = QStringLiteral( "mp4" );
    bool    processEvents       = true;

    /**
     * Loads from a QVariantMap (loaded from config.yaml via YAML).
     * Keys: enable, output_width, output_height, frame_count, fps,
     *       jpeg_quality, orbit_radius_multiplier, start_angle,
     *       delete_temp_frames, auto_compute_pitch, fallback_pitch,
     *       output_format, process_events
     */
    void loadFromVariantMap( const QVariantMap &map );

    /**
     * Returns the effective output size as QSize.
     */
    QSize outputSize() const { return QSize( outputWidth, outputHeight ); }
};


/**
 * \brief Builds OrbitExporterConfig from OrbitConfig + OrbitCamera.
 *
 * Computes:
 *   centerPoint  = (centerLongitude, centerLatitude, centerHeight)
 *   orbitRadius  = distance_2d(camera, center) * orbitRadiusMultiplier
 *   pitch        = auto_compute_pitch ? atan2(cameraHeight, radius_2d) : fallbackPitch
 *   yaw_i        = headingDeg + i * (360 / frameCount) for each frame
 *
 * Call buildExporterConfig() then pass the result to OrbitExporter::exportVideo().
 */
class OrbitParams
{
  public:

    OrbitParams( const OrbitConfig &config, const OrbitCamera &camera )
      : mConfig( config )
      , mCamera( camera )
    {}

    /**
     * Computes the final OrbitExporterConfig by combining config defaults
     * with camera-derived orbit geometry.
     *
     * \param outputDir  Directory to write output (image sequence or temp MP4 dir).
     *                   Can be empty (uses system temp dir).
     */
    OrbitExporterConfig buildExporterConfig( const QString &outputDir = QString() ) const;

    /**
     * Returns the computed horizontal distance from camera to look-at point.
     * Used for diagnostics.
     */
    double cameraToLookAtDistance2D() const;

    /**
     * Returns the computed orbit radius (horizontal, 2D).
     */
    double computedOrbitRadius() const;

    /**
     * Returns the computed pitch angle in degrees.
     */
    float computedPitch() const;

    const OrbitConfig &config() const { return mConfig; }
    const OrbitCamera &camera() const { return mCamera; }

  private:

    const OrbitConfig &mConfig;
    const OrbitCamera &mCamera;
};


/**
 * \brief Exports a circular-orbit 3D map animation as an image sequence or MP4 video.
 *
 * Computes camera keyframes evenly distributed on a circular orbit around a given
 * center point, renders each frame using QGIS's offscreen 3D engine, and optionally
 * encodes them into an MP4 using FFmpeg.
 */
class OrbitExporter : public QObject
{
    Q_OBJECT

  public:

    explicit OrbitExporter( QObject *parent = nullptr );
    ~OrbitExporter() override;

    /**
     * Runs the full export pipeline.
     *
     * \param config      Final render-time config (built by buildOrbitExporterConfig()).
     * \param mapSettings The 3D map settings (must outlive the call).
     * \param feedback    Optional cancellation object.
     * \param error       On failure, populated with a human-readable error message.
     *
     * \return true on success, false on failure or cancellation.
     * Must be called from the Qt GUI/main thread.
     */
    bool exportVideo( const OrbitExporterConfig &config,
                 Qgs3DMapSettings &mapSettings,
                 QgsFeedback *feedback,
                 QString &error );

    /**
     * Requests cancellation of the running export.
     */
    void cancel() { if ( mFeedback ) mFeedback->cancel(); }

    /**
     * Returns progress in [0.0, 100.0].
     */
    double progress() const { return mProgress; }

    /**
     * Returns the last error string.
     */
    QString errorString() const { return mError; }

  signals:
    /**
     * Emitted periodically with progress in [0, 100].
     */
    void progressChanged( double progress );

    /**
     * Emitted after each frame is rendered (0-based index).
     */
    void frameRendered( int frameIndex );

  private:

    QVector<Qgs3DAnimationSettings::Keyframe> computeKeyframes(
        const OrbitExporterConfig &config ) const;
    bool setupEngine( const QSize &outputSize );
    QImage renderFrame();
    bool saveFrame( int frameIndex, const QImage &image );

    // -- state --

    OrbitExporterConfig mConfig;
    Qgs3DMapSettings *mMapSettings = nullptr;
    QgsFeedback *mFeedback = nullptr;

    std::unique_ptr<QgsOffscreen3DEngine> mEngine;
    std::unique_ptr<Qgs3DMapScene> mScene;

    std::unique_ptr<OrbitVideoEncoder> mVideoEncoder;

    QString mFramesOutputDir;
    int mCurrentFrame = 0;
    double mProgress = 0.0;
    QString mError;
};

#endif // JINGWEIPRINTER_ORBITEXPORTER_H