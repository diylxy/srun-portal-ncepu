# 华北电力大学srun portal 校园网认证程序-C++语言实现
一种基于C++的华北电力大学校园网自动认证服务程序。支持自动检测故障并尝试重连。
##
## 免责声明
**校园网认证系统可能会更新，本程序也可能因此失效，因此造成的损失由使用者承担。本人保证程序中未刻意加入恶意代码，但因为程序需获取用户账号密码，并且访问互联网，请自行审核源代码并编译此项目**  
## 参数
```bash
Portal 用户名 密码 [网卡IP地址 | 网卡接口名，默认eth0]
```
## 克隆到本地
```bash
git clone --recursive
```

## 直接编译
本项目使用CMake构建系统  
```bash
mkdir build
cd build
cmake ..
make
```

## 交叉编译
此处以OpenWRT（X-WRT）为例。请先编译路由器固件，并使用得到的编译器和staging_dir编译此程序
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
export STAGING_DIR=/home/lxy/x-wrt/staging_dir/                 # 替换为你的openwrt staging_dir
make
```
