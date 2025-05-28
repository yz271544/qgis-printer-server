//
// Created by Lyndon on 2025/1/26.
//

#include "gtest/gtest.h"
#include "utils/Formula.h"
#include <QString>
#include <QList>
#include <QFile>
#include "config.h"
#include "utils/ImageUtil.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#include <utils/ShowDataUtil.h>

TEST(Base64Test, writePng) {
    auto northArrowPath = QString("/lyndon/iProject/cpath/jingweiprinter/common/output/north_arrow.png");
    auto northBase64 = QString("data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACYAAABECAMAAADa37fNAAAA81BMVEUAAAAsLCwFAQEmGhkEAQEFAQEFAQEjGhYkGxsEAQEEAQEkGhYXEhIFAgIIBAQSDw4jIBsFAQEjGRYFAQEGAwMIBAQPDAwJCQkGAQEGAQERDwwGAQEFAgIFAgIGAgIGAgIMCAcFAQEFAQEGAgIHAgIHAwMMCgocFhMFAQEFAQEFAQEFAQEGAQEFAQEGAgIGAgIHAgIHAgIGAgIVFRUFAQEFAQEGAQEGAgIFAwMFAwMGAwMGAwMHBAQjGRYjGRcFAgIkGRcHAgImGhcFAQEjGRYkGhckGhckGRckGRckGRcjGRcjGhcjGhgkGhoGBgYFAQEjGRZXOu2gAAAAT3RSTlMABfs76vbw2Rzt6H8RlToWCdzuy1FDJBrnuB/lpaGcfjXIxIVyTC0o4dXTz66piYFua2cNwbyxeWFeWVVH/J6NhXVY161sZNXJv7WRdEYpLZbk2gAAAtZJREFUSMfNlGl3ojAUhi8ppEIBRREV932tVavVVrsvs0/6/3/NEEIIOLbDOTNnTp8PPSU+JDfvTYA9Uq9HAMcn+8N/raWkJNqP7M0byk3Wg2nS3fXX9Bva0cPDw7OvPX3KviSo7fqbDgm0u38cyPsc3Qjt+/lhxzk5vv4Sahn63wEy2eydKxZ1XvtwkJ4U1Pb5yU2lTrLSH7bw/Ep5bx+ZF9ojvd/v6/ABIdUkVho3k2g2lpNsskqwnUAbkATF9TaYEDXfe0eRdkZxaCFClNywbOzeaOnlYHnvwIoQgtbQv18OLg9I+WHLpD3ViIcfidka5fetTrnLbJlqMruj3XInbhlKAXymiGp4y54K49imK80MMEbEp8LPVW4irHWOWwWZaWWJe+V12B4r7M4VZprW4CO6ZQdxWXmxOmGguojAYjPPSiLgMgnIQUhpRv+eyw5wGhrX5IK4vjK911sxGdwjruGryHRbWs5CDLRIyKkYXVQAJMwSZ+sLrShFzrMEvTGEdFWhaaYYH/dgt4q0jAjQWaS4HWwiXbNIhMgq1Q3UxVsOj2M/krM6zA3gbJSohi9FMXOwL4BzQWJMxA82NMK2SEWhxCPJNUDH/MnU4pps8vfVNMCyC4w6imuozeNc0gNS41Pzn61gKyNg1GZ+DBJrCe/UoNBCbFWXrak5vswmt9WwJJcdO4VF0mbLFVTXt4O4OqK5flauGuR86x+aAZuBpWX45fnlnE75hlcGwE8Wx1APLh6i6+e9OVZhfDpZw0LxI+Wfoj7NGk2hQ29d6MntErVUcbSvaHmPpyQd++C2FFqYAYKqQr9gOsTIe4OomQHB+dAbWUCcNqJ32BUD7qzovVna08ZeYZ2aNmrPbdO052cjrWYuVKIF84tPDKbH2Kwbk2ZzYtRNoJEpNJIIW4xKEuyjP6Lb2ECFWGn4nYZqxZ6LagMO0VGc6Ft4DodpTUEwrcD/5BfGeJjAKlqg2AAAAABJRU5ErkJggg==");
    std::pair<QString, QByteArray> base64_image = ImageUtil::parse_base64_image(northBase64);
    QFile file(northArrowPath);
    if (file.open(QIODevice::WriteOnly)) {
        spdlog::info("northArrowPath: {}", northArrowPath.toStdString());
        file.write(base64_image.second);
        file.close();
    }
}

