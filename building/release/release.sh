#!/usr/bin/env bash

current_tag=$(git describe --tags)


curl --request POST --header "PRIVATE-TOKEN: mhVqLTzGk_V2yMBeiwwf" \
--data name="release-v1.0.17" \
--data tag_name="v1.0.17" \
--data url="http://172.31.100.21/zhengyang.hu/jingweiprinter" \
--data ref="v1.0.17" \
--data description="接口异步执行，主线程渲染后回调响应" \
http://172.31.100.21/api/v4/projects/64/releases


curl --header "PRIVATE-TOKEN: mhVqLTzGk_V2yMBeiwwf" http://172.31.100.21/api/v4/projects/64/releases

