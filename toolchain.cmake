set(CMAKE_SYSTEM_NAME Linux)
set(TOOLCHAIN_PREFIX /home/lxy/x-wrt/staging_dir/toolchain-aarch64_cortex-a53_gcc-13.3.0_musl/bin/)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}aarch64-openwrt-linux-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}aarch64-openwrt-linux-g++)
set(CMAKE_FIND_ROOT_PATH /home/lxy/x-wrt/staging_dir/target-aarch64_cortex-a53_musl/)