ARG BASE_TAG_VERSION

FROM registry.cn-beijing.aliyuncs.com/dc_huzy/jingweiprinter-base:${BASE_TAG_VERSION}-noble AS builder
LABEL authors="Lyndon"

RUN echo "Building with tag version: $BASE_TAG_VERSION"
ARG CODE_VERSION
RUN echo "Building with code version: $CODE_VERSION"

ENV MASTER_RELEASE=true

ARG PARALLEL_LEVEL
RUN echo "Building with parallel level: $PARALLEL_LEVEL"
ENV CMAKE_BUILD_PARALLEL_LEVEL=$PARALLEL_LEVEL

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

FROM qgis/qgis:${BASE_TAG_VERSION}-noble AS runner
LABEL authors="Lyndon"

COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /usr/local/bin /usr/local/bin
COPY --from=builder /usr/local/share /usr/local/share

RUN mkdir /lyndon/iProject/cpath/jingweiprinter/common/input/ -p
COPY common/input/topicMap.json /lyndon/iProject/cpath/jingweiprinter/common/input/topicMap.json

RUN mkdir /usr/local/bin/conf
COPY conf/config-prod.yaml /usr/local/bin/conf/config.yaml

#RUN sed -i "s/mirrors.aliyun.com/mirrors.ustc.edu.cn/g" /etc/apt/sources.list.d/ubuntu.sources
RUN apt update
RUN apt install -y libarchive13t64
RUN apt install -y net-tools telnet dnsutils
RUN apt install -y libqapt3 libqapt3-runtime
RUN apt install -y mesa-utils
RUN apt install -y libxcb-*
ENV DISPLAY=:0
ENV QT_QPA_PLATFORM="xcb"
RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
RUN echo "Asia/Shanghai" > /etc/timezone

RUN mkdir /root/.fonts
COPY building/simhei.ttf /root/.fonts/simhei.ttf
COPY building/simsunb.ttf /root/.fonts/simsunb.ttf
RUN fc-cache -fv

WORKDIR /usr/local/bin

ENTRYPOINT ["jingweiprinter"]
