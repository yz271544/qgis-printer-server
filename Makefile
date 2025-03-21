VERSION=$(shell git describe --tags)
PARALLEL_LEVEL := 8

.PHONY: version
version:
	@echo ${VERSION}

REPO ?= registry.cn-beijing.aliyuncs.com/dc_huzy

.PHONY: baseimage
baseimage:
	docker build -t ${REPO}/jingweiprinter-base:3.40.4-noble --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base.docker .

# nnv
.PHONY: printerimagennv
printerimagennv:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-nnv --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-nnv.docker .

.PHONY: printerimage470
printerimage470:
	$(eval NVIDIA_MAIN_VERSION := 470)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.4-noble bash -c "wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && dpkg -i cuda-keyring_1.1-1_all.deb && apt update > /dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep Version | awk '{print \$$2}' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.docker .

.PHONY: printerimage510
printerimage510:
	$(eval NVIDIA_MAIN_VERSION := 510)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.4-noble bash -c "wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && dpkg -i cuda-keyring_1.1-1_all.deb && apt update > /dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep Version | awk '{print \$$2}' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.docker .

.PHONY: printerimage515
printerimage515:
	$(eval NVIDIA_MAIN_VERSION := 515)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.4-noble bash -c "wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && dpkg -i cuda-keyring_1.1-1_all.deb && apt update > /dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep Version | awk '{print \$$2}' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.docker .

.PHONY: printerimage520
printerimage520:
	$(eval NVIDIA_MAIN_VERSION := 520)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.4-noble bash -c "wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && dpkg -i cuda-keyring_1.1-1_all.deb && apt update > /dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep Version | awk '{print \$$2}' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.docker .

.PHONY: printerimage535
printerimage535:
	$(eval NVIDIA_MAIN_VERSION := 535)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.4-noble bash -c "wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && dpkg -i cuda-keyring_1.1-1_all.deb && apt update > /dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep Version | awk '{print \$$2}' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
			--build-arg CODE_VERSION=${VERSION} \
			--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
			--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
			--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
			-f building/noble-${NVIDIA_MAIN_VERSION}.docker .

.PHONY: printerimage550
printerimage550:
	$(eval NVIDIA_MAIN_VERSION := 550)
	$(eval NVIDIA_PACKAGE := nvidia-utils-${NVIDIA_MAIN_VERSION})
	$(eval DRIVER_FULL_VERSION := $(shell docker run --rm qgis/qgis:3.40.4-noble bash -c "wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb && dpkg -i cuda-keyring_1.1-1_all.deb && apt update > /dev/null 2>&1 && apt-cache show ${NVIDIA_PACKAGE} 2>/dev/null | grep Version | awk '{print \$$2}' | head -1"))
	@if [ -z "${DRIVER_FULL_VERSION}" ]; then echo "Error: Failed to get driver version for ${NVIDIA_PACKAGE}"; exit 1; fi
	docker build -t ${REPO}/jingweiprinter:${VERSION}-${DRIVER_FULL_VERSION} \
    		--build-arg CODE_VERSION=${VERSION} \
    		--build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} \
    		--build-arg NVIDIA_MAIN_VERSION=${NVIDIA_MAIN_VERSION} \
    		--build-arg DRIVER_FULL_VERSION=${DRIVER_FULL_VERSION} \
    		-f building/noble-${NVIDIA_MAIN_VERSION}.docker .

.PHONY: printerimages
printerimages: printerimagennv printerimage470 printerimage510 printerimage515 printerimage520 printerimage535 printerimage550

.PHONY: images
images: baseimage printerimages

.PHONY: push
push:
	for tag in $$(docker images --format "{{.Repository}}:{{.Tag}}" ${REPO}/jingweiprinter | grep "${VERSION}-"); do \
		docker push $$tag; \
	done