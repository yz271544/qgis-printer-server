VERSION=$(shell git describe --tags)
PARALLEL_LEVEL := 8

.PHONY: version
version:
	@echo ${VERSION}

REPO ?= registry.cn-beijing.aliyuncs.com/dc_huzy
BASE_TAG_VERSION ?= 3.40.9

.PHONY: baseimage
baseimage:
	docker build -t ${REPO}/jingweiprinter-base:3.40.9-noble --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base.dockerfile .

.PHONY: baseimageproxy
baseimageproxy:
	docker build -t ${REPO}/jingweiprinter-base:3.40.5-noble --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base-proxy.dockerfile .

.PHONY: basegithub
basegithub:
	docker build -t ${REPO}/jingweiprinter-base:3.40.9-noble --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base-github.dockerfile .

# nnv
.PHONY: printerimagennv
printerimagennv:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-nnv --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv.dockerfile .

.PHONY: printerimagesproxy
printerimagesproxy:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-nnv --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv-proxy.dockerfile .

# nnv-c86
.PHONY: printerimagennvc86
printerimagennvc86:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-nnv-c86 --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv.dockerfile .

.PHONE: printerimagennvc86proxy
printerimagennvc86proxy:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-nnv-c86 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv-proxy.dockerfile .

.PHONY: printergithubnnv
printergithubnnv:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-nnv --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv-github.dockerfile .

# nnv aarch64
.PHONY: printergithubnnvarm64
printergithubnnvarm64:
	docker buildx build --platform linux/arm64 -t ${REPO}/jingweiprinter:${VERSION}-nnv-arm64 --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv-github-aarch64.dockerfile --push .

# nnv aarch64 runtime local
.PHONY: printerlocalruntimennvarm64
printerlocalruntimennvarm64:
	docker buildx build --platform linux/arm64 -t ${REPO}/qgis:${VERSION}-nnv-arm64 --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg REPO=${REPO} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv-local-runtime-aarch64.dockerfile --push .

.PHONY: printerlocalnnvarm64
printerlocalnnvarm64:
	docker buildx build --platform linux/arm64 -t ${REPO}/jingweiprinter:${VERSION}-nnv-arm64 --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg REPO=${REPO} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv-local-aarch64.dockerfile --push .

.PHONY: printerimage470
printerimage470:
	$(eval NVIDIA_MAIN_VERSION := 470)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.9-noble bash -c "wget -q https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && dpkg -i cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && apt-get update >/dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep -oP 'Version: \\K.*' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
            --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.dockerfile .

.PHONY: printerimage510
printerimage510:
	$(eval NVIDIA_MAIN_VERSION := 510)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.9-noble bash -c "wget -q https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && dpkg -i cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && apt-get update >/dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep -oP 'Version: \\K.*' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
            --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.dockerfile .

.PHONY: printerimage515
printerimage515:
	$(eval NVIDIA_MAIN_VERSION := 515)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.9-noble bash -c "wget -q https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && dpkg -i cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && apt-get update >/dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep -oP 'Version: \\K.*' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
            --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.dockerfile .

.PHONY: printerimage520
printerimage520:
	$(eval NVIDIA_MAIN_VERSION := 520)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.9-noble bash -c "wget -q https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && dpkg -i cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && apt-get update >/dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep -oP 'Version: \\K.*' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
            --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.dockerfile .

.PHONY: printerimage535
printerimage535:
	$(eval NVIDIA_MAIN_VERSION := 535)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.9-noble bash -c "wget -q https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && dpkg -i cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && apt-get update >/dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep -oP 'Version: \\K.*' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
            --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.dockerfile .

.PHONY: printerimage550
printerimage550:
	$(eval NVIDIA_MAIN_VERSION := 550)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.9-noble bash -c "wget -q https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && dpkg -i cuda-keyring_1.1-1_all.deb >/dev/null 2>&1 && apt-get update >/dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep -oP 'Version: \\K.*' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
            --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} \
    		--build-arg CODE_VERSION=${VERSION} \
    		--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
    		--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
    		--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
    		-f building/noble-${NVIDIA_MAIN_VERSION}.dockerfile .

.PHONY: printerimage550120
printerimage550120:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-550-120 --build-arg BASE_TAG_VERSION=${BASE_TAG_VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-550-120.dockerfile .

.PHONY: printerimages
printerimages: printerimagennv

.PHONY: printergithub
printergithub: printergithubnnv

.PHONY: images
images: baseimage printerimages

.PHONY: imagesc86
imagesc86: baseimage printerimagennvc86

.PHONY: imagesproxy
imagesproxy: baseimageproxy printerimagesproxy

.PHONY: imagesc86proxy
imagesc86proxy: baseimageproxy printerimagennvc86proxy

.PHONY: github
github: basegithub printergithub

.PHONY: aarch64local
aarch64local: printerlocalruntimennvarm64 printerlocalnnvarm64

.PHONY: push
push:
	for tag in $$(docker images --format "{{.Repository}}:{{.Tag}}" ${REPO}/jingweiprinter | grep "${VERSION}-"); do \
		docker push $$tag; \
	done
