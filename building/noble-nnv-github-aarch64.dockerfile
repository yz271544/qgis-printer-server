FROM ubuntu:noble AS builder
LABEL authors="Lyndon"

ARG CODE_VERSION
RUN echo "Building with code version: $CODE_VERSION"

ENV MASTER_RELEASE=true

ARG PARALLEL_LEVEL
RUN echo "Building with parallel level: $PARALLEL_LEVEL"
ENV CMAKE_BUILD_PARALLEL_LEVEL=$PARALLEL_LEVEL

RUN sed -i "s|ports.ubuntu.com|mirrors.tuna.tsinghua.edu.cn|g" /etc/apt/sources.list.d/ubuntu.sources
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update
RUN apt-get install -y bison build-essential ca-certificates ccache cmake cmake-curses-gui dh-python doxygen expect \
    flex flip gdal-bin git graphviz grass-dev libdraco-dev libexiv2-dev libexpat1-dev libfcgi-dev libgdal-dev \
    libgeos-dev libgsl-dev libpq-dev libproj-dev libprotobuf-dev libqca-qt5-2-dev libqca-qt5-2-plugins \
    libqscintilla2-qt5-dev libqt5opengl5-dev libqt5serialport5-dev libqt5sql5-sqlite libqt5svg5-dev libqt5webkit5-dev \
    libqt5xmlpatterns5-dev libqwt-qt5-dev libspatialindex-dev libspatialite-dev libsqlite3-dev \
    libsqlite3-mod-spatialite libyaml-tiny-perl libzip-dev libzstd-dev lighttpd locales ninja-build ocl-icd-opencl-dev \
    opencl-headers pandoc pkg-config poppler-utils protobuf-compiler pyqt5-dev pyqt5-dev-tools pyqt5.qsci-dev \
    python3-all-dev python3-autopep8 python3-dev python3-gdal python3-jinja2 python3-lxml python3-mock python3-nose2 \
    python3-owslib python3-plotly python3-psycopg2 python3-pygments python3-pyproj python3-pyqt5 python3-pyqt5.qsci \
    python3-pyqt5.qtmultimedia python3-pyqt5.qtpositioning python3-pyqt5.qtserialport python3-pyqt5.qtsql \
    python3-pyqt5.qtsvg python3-pyqt5.qtwebkit python3-pyqtbuild python3-sip python3-termcolor python3-yaml \
    qt3d-assimpsceneimport-plugin qt3d-defaultgeometryloader-plugin qt3d-gltfsceneio-plugin qt3d-scene2d-plugin \
    qt3d5-dev qtbase5-dev qtbase5-private-dev qtkeychain-qt5-dev qtmultimedia5-dev qtpositioning5-dev qttools5-dev \
    qttools5-dev-tools sip-tools spawn-fcgi xauth xfonts-100dpi xfonts-75dpi xfonts-base xfonts-scalable xvfb

RUN ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

COPY building/other-party/PDAL-2.6.3-src.tar.bz2 /usr/local/src/
COPY building/other-party/QGIS-final-3_40_6.tar.gz /usr/local/src/

RUN bzip2 -d /usr/local/src/PDAL-2.6.3-src.tar.bz2
RUN tar xvf /usr/local/src/PDAL-2.6.3-src.tar -C /usr/local/src/
RUN tar -zxvf /usr/local/src/QGIS-final-3_40_6.tar.gz -C /usr/local/src/

WORKDIR /usr/local/src/PDAL-2.6.3-src
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
RUN cmake --install build

RUN ln -s /usr/bin/ccache /usr/local/bin/gcc
RUN ln -s /usr/bin/ccache /usr/local/bin/g++

WORKDIR /usr/local/src/QGIS-final-3_40_6
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DWITH_3D=ON -DWITH_ANALYSIS=ON -DWITH_APIDOC=OFF -DWITH_ASTYLE=OFF \
    -DWITH_AUTH=ON -DWITH_BINDINGS=ON -DWITH_COPC=ON -DWITH_CRASH_HANDLER=ON -DWITH_CUSTOM_WIDGETS=OFF \
    -DWITH_DESKTOP=ON -DWITH_DRACO=ON -DWITH_EPT=ON -DWITH_GRASS7=ON -DWITH_GRASS8=ON -DWITH_GRASS_PLUGIN=ON \
    -DWITH_GSL=ON -DWITH_GUI=ON -DWITH_HANA=OFF -DWITH_INTERNAL_LAZPERF=ON -DWITH_INTERNAL_MDAL=ON \
    -DWITH_INTERNAL_MESHOPTIMIZER=ON -DWITH_INTERNAL_NLOHMANN_JSON=ON -DWITH_INTERNAL_O2=ON -DWITH_INTERNAL_POLY2TRI=ON \
    -DWITH_OAUTH2_PLUGIN=ON -DWITH_ORACLE=OFF -DWITH_PDAL=ON -DWITH_PDF4QT=OFF -DWITH_POSTGRESQL=ON \
    -DWITH_PY_COMPILE=OFF -DWITH_QGIS_PROCESS=ON -DWITH_QSPATIALITE=OFF -DWITH_QTGAMEPAD=OFF -DWITH_QTPRINTER=ON \
    -DWITH_QTSERIALPORT=ON -DWITH_QTWEBENGINE=OFF -DWITH_QTWEBKIT=ON -DWITH_QUICK=OFF -DWITH_QWTPOLAR=OFF \
    -DWITH_SERVER=OFF -DWITH_SPATIALITE=ON -DWITH_STAGED_PLUGINS=ON -DWITH_VCPKG=OFF \

WORKDIR /usr/local/src/QGIS-final-3_40_6/build
RUN make -j$(nproc)
RUN make install
RUN ldconfig

# set git proxy
RUN mkdir -p /root/.ssh
RUN chmod 700 /root/.ssh
COPY building/.ssh/ /root/.ssh
RUN git config --global user.name "Lyndon"
RUN git config --global user.email "Lyndon.Hu9@gmail.com"
RUN git config --global http.proxy http://172.17.0.1:10808/
RUN git config --global http.sslverify false
RUN git config --global http.postbuffer 2097152000
RUN git config --global https.proxy http://172.17.0.1:10808/
RUN git config --global https.postbuffer 2097152000
RUN git config --global core.autocrlf input

RUN mkdir /lyndon/iProject/cpath -p
WORKDIR /lyndon/iProject/cpath
# build and install yaml-cpp
RUN git clone git@github.com:yz271544/yaml-cpp.git
WORKDIR /lyndon/iProject/cpath/yaml-cpp
RUN git checkout -b 0.8.0 0.8.0
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
WORKDIR /lyndon/iProject/cpath/yaml-cpp/build
RUN make install
WORKDIR /lyndon/iProject/cpath
# build and install oatpp
RUN git clone git@github.com:yz271544/oatpp.git
WORKDIR /lyndon/iProject/cpath/oatpp
RUN git checkout -b 1.4.0 1.4.0
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
WORKDIR /lyndon/iProject/cpath/oatpp/build
RUN make install
WORKDIR /lyndon/iProject/cpath
# build and install oatpp-curl
RUN git clone git@github.com:yz271544/oatpp-curl.git
WORKDIR /lyndon/iProject/cpath/oatpp-curl
RUN git checkout -b 1.4.0 1.4.0
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
WORKDIR /lyndon/iProject/cpath/oatpp-curl/build
RUN make install
WORKDIR /lyndon/iProject/cpath
# build and install spdlog
RUN git clone git@github.com:yz271544/spdlog.git
WORKDIR /lyndon/iProject/cpath/spdlog
RUN git checkout -b v1.9.2 v1.9.2
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
WORKDIR /lyndon/iProject/cpath/spdlog/build
RUN make install
WORKDIR /lyndon/iProject/cpath
# build and install jingweiprinter
RUN git clone git@github.com:yz271544/jingweiprinter.git
WORKDIR /lyndon/iProject/cpath/jingweiprinter
RUN git checkout -b $CODE_VERSION $CODE_VERSION
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
WORKDIR /lyndon/iProject/cpath/jingweiprinter/build
RUN make install
WORKDIR /lyndon/iProject/cpath

FROM ubuntu:noble AS runner
LABEL authors="Lyndon"

RUN sed -i "s|ports.ubuntu.com|mirrors.tuna.tsinghua.edu.cn|g" /etc/apt/sources.list.d/ubuntu.sources
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update
RUN apt-get install -y bison build-essential ca-certificates ccache cmake cmake-curses-gui dh-python doxygen expect \
    flex flip gdal-bin git graphviz grass-dev libdraco-dev libexiv2-dev libexpat1-dev libfcgi-dev libgdal-dev \
    libgeos-dev libgsl-dev libpq-dev libproj-dev libprotobuf-dev libqca-qt5-2-dev libqca-qt5-2-plugins \
    libqscintilla2-qt5-dev libqt5opengl5-dev libqt5serialport5-dev libqt5sql5-sqlite libqt5svg5-dev libqt5webkit5-dev \
    libqt5xmlpatterns5-dev libqwt-qt5-dev libspatialindex-dev libspatialite-dev libsqlite3-dev \
    libsqlite3-mod-spatialite libyaml-tiny-perl libzip-dev libzstd-dev lighttpd locales ninja-build ocl-icd-opencl-dev \
    opencl-headers pandoc pkg-config poppler-utils protobuf-compiler pyqt5-dev pyqt5-dev-tools pyqt5.qsci-dev \
    python3-all-dev python3-autopep8 python3-dev python3-gdal python3-jinja2 python3-lxml python3-mock python3-nose2 \
    python3-owslib python3-plotly python3-psycopg2 python3-pygments python3-pyproj python3-pyqt5 python3-pyqt5.qsci \
    python3-pyqt5.qtmultimedia python3-pyqt5.qtpositioning python3-pyqt5.qtserialport python3-pyqt5.qtsql \
    python3-pyqt5.qtsvg python3-pyqt5.qtwebkit python3-pyqtbuild python3-sip python3-termcolor python3-yaml \
    qt3d-assimpsceneimport-plugin qt3d-defaultgeometryloader-plugin qt3d-gltfsceneio-plugin qt3d-scene2d-plugin \
    qt3d5-dev qtbase5-dev qtbase5-private-dev qtkeychain-qt5-dev qtmultimedia5-dev qtpositioning5-dev qttools5-dev \
    qttools5-dev-tools sip-tools spawn-fcgi xauth xfonts-100dpi xfonts-75dpi xfonts-base xfonts-scalable xvfb

RUN ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
RUN echo "Asia/Shanghai" > /etc/timezone
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /usr/local/bin /usr/local/bin
COPY --from=builder /usr/local/share /usr/local/share

RUN mkdir /lyndon/iProject/cpath/jingweiprinter/common/input/ -p
COPY common/input/topicMap.json /lyndon/iProject/cpath/jingweiprinter/common/input/topicMap.json

RUN mkdir /usr/local/bin/conf
COPY conf/config-prod.yaml /usr/local/bin/conf/config.yaml

RUN apt update
RUN apt install -y libarchive13t64
RUN apt install -y net-tools telnet dnsutils
RUN apt install -y libqapt3 libqapt3-runtime
RUN apt install -y mesa-utils
RUN apt install -y libxcb-*
ENV DISPLAY=:0
ENV QT_QPA_PLATFORM="xcb"

RUN mkdir /root/.fonts
COPY building/simhei.ttf /root/.fonts/simhei.ttf
COPY building/simsunb.ttf /root/.fonts/simsunb.ttf
RUN fc-cache -fv

WORKDIR /usr/local/bin

ENTRYPOINT ["jingweiprinter"]
