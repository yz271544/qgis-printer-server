VERSION=$(git describe --tags)

.PHONY: version
version:
	@echo ${VERSION}

REPO ?= registry.cn-beijing.aliyuncs.com/dc_huzy

.PHONY: images
images:
	docker build -t ${REPO}/jingweiprinter:${VERSION} --build-arg CODE_VERSION=${VERSION} -f building/noble.docker .

.PHONY: push
push:
	docker push ${REPO}/jingweiprinter:${VERSION}