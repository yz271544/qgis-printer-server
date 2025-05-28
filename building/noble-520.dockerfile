FROM registry.cn-beijing.aliyuncs.com/dc_huzy/jingweiprinter-base:3.40.5-noble AS builder
LABEL authors="Lyndon"

ARG CODE_VERSION
RUN echo "Building with code version: $CODE_VERSION"

ENV MASTER_RELEASE=true

ARG PARALLEL_LEVEL
RUN echo "Building with parallel level: $PARALLEL_LEVEL"
ENV CMAKE_BUILD_PARALLEL_LEVEL=$PARALLEL_LEVEL

WORKDIR /lyndon/iProject/cpath
# build and install jingweiprinter
RUN git clone http://172.31.100.21/zhengyang.hu/jingweiprinter.git
WORKDIR /lyndon/iProject/cpath/jingweiprinter
RUN git checkout -b $CODE_VERSION $CODE_VERSION
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
RUN cmake --build build
WORKDIR /lyndon/iProject/cpath/jingweiprinter/build
RUN make install
WORKDIR /lyndon/iProject/cpath

FROM qgis/qgis:3.40.5-noble AS runner
ARG NVIDIA_MAIN_VERSION
ARG DRIVER_FULL_VERSION
LABEL authors="Lyndon"
LABEL driver.version="${DRIVER_FULL_VERSION}"

COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /usr/local/bin /usr/local/bin
COPY --from=builder /usr/local/share /usr/local/share

RUN mkdir /lyndon/iProject/cpath/jingweiprinter/common/input/ -p
COPY common/input/topicMap.json /lyndon/iProject/cpath/jingweiprinter/common/input/topicMap.json

RUN mkdir /usr/local/bin/conf
COPY conf/config-prod.yaml /usr/local/bin/conf/config.yaml

#RUN sed -i "s/mirrors.aliyun.com/mirrors.ustc.edu.cn/g" /etc/apt/sources.list.d/ubuntu.sources
RUN wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb
RUN dpkg -i cuda-keyring_1.1-1_all.deb
RUN apt update
RUN apt install -y libarchive13t64
RUN apt install -y net-tools telnet dnsutils
RUN apt install -y libqapt3 libqapt3-runtime
RUN apt install -y mesa-utils
RUN apt install -y nvidia-utils-520 libnvidia-gl-520
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
