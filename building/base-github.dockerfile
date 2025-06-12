FROM qgis/qgis:3.40.5-noble
LABEL authors="Lyndon"

ARG CODE_VERSION
RUN echo "Building with code version: $CODE_VERSION"

ENV MASTER_RELEASE=true

ARG PARALLEL_LEVEL
RUN echo "Building with parallel level: $PARALLEL_LEVEL"
ENV CMAKE_BUILD_PARALLEL_LEVEL=$PARALLEL_LEVEL

#RUN cp /etc/apt/sources.list.d/ubuntu.sources /etc/apt/sources.list.d/ubuntu.sources.bak
#RUN sed -i "s/archive.ubuntu.com/mirrors.aliyun.com/g" /etc/apt/sources.list.d/ubuntu.sources
#RUN sed -i "s/security.ubuntu.com/mirrors.aliyun.com/g" /etc/apt/sources.list.d/ubuntu.sources

RUN apt-get update
RUN apt-get install -y bison build-essential ca-certificates ccache cmake cmake-curses-gui dh-python \
    doxygen expect flex flip gdal-bin git graphviz grass-dev libdraco-dev libexiv2-dev libexpat1-dev \
    libfcgi-dev libgdal-dev libgeos-dev libgsl-dev libpq-dev libproj-dev libprotobuf-dev libqca-qt5-2-dev \
    libqca-qt5-2-plugins libqscintilla2-qt5-dev libqt5opengl5-dev libqt5serialport5-dev libqt5sql5-sqlite \
    libqt5svg5-dev libqt5webkit5-dev libqt5xmlpatterns5-dev libqwt-qt5-dev libspatialindex-dev libspatialite-dev \
    libsqlite3-dev libsqlite3-mod-spatialite libyaml-tiny-perl libzip-dev libzstd-dev lighttpd locales ninja-build \
    ocl-icd-opencl-dev opencl-headers pandoc pkg-config poppler-utils protobuf-compiler pyqt5-dev pyqt5-dev-tools \
    pyqt5.qsci-dev python3-all-dev python3-autopep8 python3-dev python3-gdal python3-jinja2 python3-lxml python3-mock \
    python3-nose2 python3-owslib python3-plotly python3-psycopg2 python3-pygments python3-pyproj python3-pyqt5 \
    python3-pyqt5.qsci python3-pyqt5.qtmultimedia python3-pyqt5.qtpositioning python3-pyqt5.qtserialport \
    python3-pyqt5.qtsql python3-pyqt5.qtsvg python3-pyqt5.qtwebkit python3-pyqtbuild python3-sip python3-termcolor \
    python3-yaml qt3d-assimpsceneimport-plugin qt3d-defaultgeometryloader-plugin qt3d-gltfsceneio-plugin \
    qt3d-scene2d-plugin qt3d5-dev qtbase5-dev qtbase5-private-dev qtkeychain-qt5-dev qtmultimedia5-dev \
    qtpositioning5-dev qttools5-dev qttools5-dev-tools sip-tools spawn-fcgi xauth xfonts-100dpi xfonts-75dpi \
    xfonts-base xfonts-scalable xvfb libcurl4-openssl-dev libarchive13t64 libarchive-dev

RUN apt-get install -y libqgis-dev=1:3.42.3+40noble

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

CMD ["bash"]
