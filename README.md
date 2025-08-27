# qgis print server

with qgis application for printer

## Motivation

this service can running on headless standalone mode with qgis server in docker container.

## Development Environment

- oatpp
- oatpp-curl
- yaml-cpp
- spdlog

### Install dependencies

```bash
mkdir ${your project dir}/cpath -p
cd ${your project dir}/cpath
git clone git@ssh.github.com:oatpp/oatpp.git
git clone git@ssh.github.com:oatpp/oatpp-curl.git

cd oatpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target install

cd ../oatpp-curl
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target install

```

## Release Environment

```bash
mkdir ${your project dir}/cpath -p
cd ${your project dir}/cpath
git clone git@ssh.github.com:oatpp/oatpp.git
git clone git@ssh.github.com:oatpp/oatpp-curl.git

cd oatpp
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install

cd ../oatpp-curl
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target install

```

## windows

### with vcpkg

clone vcpkg to your computer

```bash
git clone https://github.com/microsoft/vcpkg.git

# bootstrap vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat

# put the vcpkg to the path you like

PATH=%PATH%;D:\iProject\cpath\vcpkg

```

cmake options

```
-DCMAKE_TOOLCHAIN_FILE=D:\iProject\cpath\vcpkg\scripts\buildsystems\vcpkg.cmake

```

## test use nvidia prime Environment

```shell
cd jingweiprinter/cmake-build-debug/src
QT_QPA_PLATFORM=xcb __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia __VK_LAYER_NV_optimus=NVIDIA_only ./jingweiprinter
```

```
```

## build arm64


## some examples

<table>
    <tr>
        <td><img src="doc/images/d3_scene.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/打印测试-现场位置图-a3-polygon.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/27-现场位置图-a3-3D.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/27区-现场位置图-a3.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/打印测试-现场位置图-a3.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/打印测试-现场位置图-a3-3D.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/晋祠-现场位置图-a3.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/晋祠-现场位置图-a3-3D.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/京广线1-现场位置图-a3-3D.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/瑞光电厂-现场位置图-a3-3D.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/应县木塔-现场位置图-a3.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/应县木塔-现场位置图-a3-3D.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/g_0814-现场位置图-a3.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/g_0814-现场位置图-a3-3D.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
    <tr>
        <td><img src="doc/images/qgz/local/zn-其他功能测试2-现场位置图-a3-3D.png" alt="27" style="float: left; width: 48%; margin-right: 2%;"></td>
        <td><img src="doc/images/qgz/local/zn-表格导入导出测试-现场位置图-a3-3D.png" alt="27-3d" style="float: left; width: 48%;"></td>
    </tr>
</table>
