#!/usr/bin/bash

if [ -z "$1" ]; then
    echo "第一个参数不能为空，请输入release发布描述。"
    exit 1
else
    echo "发布描述：$1"
fi

current_tag=$(git describe --tags)

if [[ $current_tag =~ -g ]]; then
    echo "tag有偏差"
    exit 1
fi

if [ -z "$current_tag" ]; then
  echo "No tag found"
  exit 1
fi

if [ $(git rev-parse --abbrev-ref HEAD) != "master" ]; then
  echo "Not on master branch"
  exit 1
fi

if [ -n "$(git status --porcelain)" ]; then
  echo "Working directory not clean"
  exit 1
fi

if [ -n "$(git log origin/master..HEAD)" ]; then
  echo "Not all changes pushed"
  exit 1
fi

if [ -n "$(git tag --points-at HEAD)" ]; then
  echo "Tag already exists"
  exit 1
fi

if

curl --request POST --header "PRIVATE-TOKEN: mhVqLTzGk_V2yMBeiwwf" \
--data name="release-${current_tag}" \
--data tag_name="${current_tag}" \
--data url="http://172.31.100.21/zhengyang.hu/jingweiprinter" \
--data ref="${current_tag}" \
--data description="$1" \
http://172.31.100.21/api/v4/projects/64/releases


curl --header "PRIVATE-TOKEN: mhVqLTzGk_V2yMBeiwwf" http://172.31.100.21/api/v4/projects/64/releases

