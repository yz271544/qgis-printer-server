VERSION=$(shell git describe --tags)
PARALLEL_LEVEL := 8

.PHONY: version
version:
	@echo ${VERSION}

REPO ?= registry.cn-beijing.aliyuncs.com/dc_huzy

.PHONY: baseimage
baseimage:
	docker build -t ${REPO}/jingweiprinter-base:3.40.4-noble --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/base.docker .

# 470.256
.PHONY: printerimage460
printerimage460:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-460 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-460.docker .
# 470.256
.PHONY: printerimage470
printerimage470:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-470 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-470.docker .
# 525.147
.PHONY: printerimage510
printerimage510:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-510 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-510.docker .
# 525.147
.PHONY: printerimage515
printerimage515:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-515 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-515.docker .
# 525.147
.PHONY: printerimage520
printerimage520:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-520 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-520.docker .
# 535.183
.PHONY: printerimage535
printerimage535:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-535 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-535.docker .
# 550.120
.PHONY: printerimage550
printerimage550:
	docker build -t ${REPO}/jingweiprinter:${VERSION}-550 --build-arg CODE_VERSION=${VERSION} --build-arg PARALLEL_LEVEL=${PARALLEL_LEVEL} -f building/noble-550.docker .

.PHONY: printerimages
printerimages: printerimage460 printerimage470 printerimage510 printerimage515 printerimage520 printerimage535 printerimage550

.PHONY: images
images: baseimage printerimages

.PHONY: push
push:
	docker push ${REPO}/jingweiprinter:${VERSION}-460
	docker push ${REPO}/jingweiprinter:${VERSION}-470
	docker push ${REPO}/jingweiprinter:${VERSION}-510
	docker push ${REPO}/jingweiprinter:${VERSION}-515
	docker push ${REPO}/jingweiprinter:${VERSION}-520
	docker push ${REPO}/jingweiprinter:${VERSION}-535
	docker push ${REPO}/jingweiprinter:${VERSION}-550
