//
// Created by Lyndon on 2025/1/26.
//

#include "qgsgltf3dutils.h"

#include "qgsgltfutils.h"
#include "qgsblockingnetworkrequest.h"
#include "qgscoordinatetransform.h"
#include "qgslogger.h"
#include "qgsmetalroughmaterial.h"
#include "qgstexturematerial.h"

#include <Qt3DCore/QEntity>

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
typedef Qt3DRender::QAttribute Qt3DQAttribute;
typedef Qt3DRender::QBuffer Qt3DQBuffer;
typedef Qt3DRender::QGeometry Qt3DQGeometry;
#else
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QGeometry>
typedef Qt3DCore::QAttribute Qt3DQAttribute;
typedef Qt3DCore::QBuffer Qt3DQBuffer;
typedef Qt3DCore::QGeometry Qt3DQGeometry;
#endif

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QTexture>

#include <QFile>
#include <QFileInfo>
#include <QMatrix4x4>
#include "qgsmatrix4x4.h"


#include "gtest/gtest.h"
#include "utils/Formula.h"
#include "qgis_3d.h"
#include <QImage>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#define TINYGLTF_NO_STB_IMAGE       // we use QImage-based reading of images
#define TINYGLTF_NO_STB_IMAGE_WRITE // we don't need writing of images
#include "tiny_gltf.h"

class QImageLoadTest : public ::testing::Test {

public:
    // TODO: move elsewhere
    static QByteArray fetchUri( const QUrl &url, QStringList *errors )
    {
        if ( url.scheme().startsWith( "http" ) )
        {
            QNetworkRequest request = QNetworkRequest( url );
            request.setAttribute( QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );
            request.setAttribute( QNetworkRequest::CacheSaveControlAttribute, true );
            QgsBlockingNetworkRequest networkRequest;
            // TODO: setup auth, setup headers
            if ( networkRequest.get( request ) != QgsBlockingNetworkRequest::NoError )
            {
                if ( errors )
                    *errors << QStringLiteral( "Failed to download image: %1" ).arg( url.toString() );
            }
            else
            {
                const QgsNetworkReplyContent content = networkRequest.reply();
                return content.content();
            }
        }
        else if ( url.isLocalFile() && QFile::exists( url.toLocalFile() ) )
        {
            QFile f( url.toLocalFile() );
            if ( f.open( QIODevice::ReadOnly ) )
            {
                return f.readAll();
            }
            else
            {
                if ( errors )
                    *errors << QStringLiteral( "Unable to open image: %1" ).arg( url.toString() );
            }
        }
        return QByteArray();
    }
};


TEST_F(QImageLoadTest, test_image_load) {
    QUrl url = QUrl("http://47.94.145.6:80/map/realistic3d/1847168269595754497-jkg/BlockYXYB/BlockYXYB_L16_4.b3dm");
    QStringList errors;
    QByteArray ba = QImageLoadTest::fetchUri(url, &errors);

    QImage img;
    auto isLoaded = img.loadFromData(( const unsigned char * ) ba.constData(), ba.size());
    ASSERT_TRUE(isLoaded);
}