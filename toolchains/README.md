# toolchains

本目录只提交 `bootstrap.sh`、本说明和 `src/*.c`，其余均由脚本生成（见 `.gitignore`）。

```bash
bash toolchains/bootstrap.sh          # 需要 rpm2cpio、cpio、curl、gcc-multilib
DNF_MIRROR=https://... bash toolchains/bootstrap.sh   # 可替换 vault.centos.org 镜像
```

生成内容：CentOS 6/5 的 i386 sysroot（c6root、c5root）、GCC 4.4.x cc1plus、
32 位 zlib / libcrypto（取自 Debian `libssl-dev:i386`，可用 `LIBCRYPTO32_A` 指定）、
OpenSSL3→C6 兼容 shim、以及 `toolchain.env` 与编译包装脚本。
nxencryption 从 `.origin_1031` 子模块复制，需先 `git submodule update --init`。
