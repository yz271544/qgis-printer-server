VERSION=$(shell git describe --tags)
PARALLEL_LEVEL := 8

.PHONY: version
version:
	@echo ${VERSION}

REPO ?= registry.cn-beijing.aliyuncs.com/dc_huzy

.PHONY: baseimage
baseimage:
	docker build -t ${REPO}/jingweiprinter-base:3.40.4-noble --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base.docker .

.PHONY: printerimage
printerimage:
	docker build -t ${REPO}/jingweiprinter:${VERSION} --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble.docker .

.PHONY: images
images: baseimage printerimage

.PHONY: push
push:
	docker push ${REPO}/jingweiprinter:${VERSION}