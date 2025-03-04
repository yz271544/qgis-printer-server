VERSION=$(shell git describe --tags)
PARALLEL_LEVEL := 8

.PHONY: version
version:
	@echo ${VERSION}

REPO ?= registry.cn-beijing.aliyuncs.com/dc_huzy

.PHONY: baseimage
baseimage:
	docker build -t ${REPO}/jingweiprinter-base:3.40.4-noble --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base.docker .

.PHONY: printerimage470
printerimage470:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-470 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-470.docker .

.PHONY: printerimage550
printerimage550:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-550 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-550.docker .

.PHONY: printerimages
printerimages: printerimage470 printerimage550

.PHONY: images
images: baseimage printerimages

.PHONY: push
push:
	docker push ${REPO}/jingweiprinter:${VERSION}-470
	docker push ${REPO}/jingweiprinter:${VERSION}-550