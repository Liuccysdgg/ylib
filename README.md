# YLIB跨平台C++开发库

自2019年的雏形至今通过不停的迭代、优化更新现在已经完全可以胜任几乎所有C++项目的开发，可以做为基础库用来配合项目基础构建、也可以使用其中网络、数据库和通用工具封装实现高级应用。

## 构建部署


### 下载项目

```bash
git clone https://github.com/Liuccysdgg/ylib.git
```

### Windows

# 创建文件夹

```bash
mkdir ylib_build_vs
```

# 进入目录


```bash
cd ylib_build_vs
```

# CMake构建生成VS项目


```bash
cmake ../ylib
```

### Linux(推荐Ubuntu)

# 进入目录


```basic
cd ylib
```

# 执行构建脚本


```bash
chmod 777 build.sh
./build.sh
```

## 构建应用


Linux自动安装至系统环境，Windows请按照以下要求进行链接和引用

### 引用头文件


```bash
ylib\include
ylib\3rdparty
```

### 链接静态库


```bash
# 库目录
ylib_vs_build\lib\Debug
# 必须包含(基础组件)
ybase.lib 
# 网络库
ynet.lib
# 工具库(必须包含)
yutil.lib
# QtGUI库(请另外单独链接Qt)
yqt.lib
# MySqlConnectorCpp
ymysql.lib
# Lua支持
ylua.lib
```

## 🙇致谢


如果没有社区已有的优秀软件帮助，ylib就不可能构建出来

* [https://github.com/ldcsaa/HP-Socket](https://github.com/ldcsaa/HP-Socket)  HPSocket高性能网络库
* [https://github.com/GreycLab/CImg.git](https://github.com/GreycLab/CImg.git) CImg库是一个用于图像处理的小型开源C++ 库
* [https://github.com/FFmpeg/FFmpeg](https://github.com/FFmpeg/FFmpeg) FFmpeg 是用于处理多媒体内容（例如音频、视频、字幕和相关元数据）的库和工具的集合
* [https://github.com/satoren/kaguya](https://github.com/satoren/kaguya) C++ 与 Lua 的绑定
* [https://github.com/lua/lua](https://github.com/lua/lua)
* [https://github.com/brofield/simpleini](https://github.com/brofield/simpleini) 一个跨平台库，提供简单的 API 来读取和写入 INI 样式的配置文件
* [https://www.sqlite.org](https://www.sqlite.org/) SQLite是一个C语言库，它实现了一个 小型、 快速、 独立、 高可靠性、 功能齐全的SQL数据库引擎
* [https://github.com/madler/zlib](https://github.com/madler/zlib)   通用数据压缩库
