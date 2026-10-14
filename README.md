# rednf —— DNF 1031（清风）服务端源码重建

rednf 是 DNF（地下城与勇士）台服 1031 服务端的**源码重建**工程：14 个 `df_*_r` 服务全部由
C++ 源码经 CMake 构建，可以直接替换原版 ELF 运行；并提供从零开始的 Docker 构建与启动方案。

> 原版运行时（配置模板、secsvr、TSS SDK、PVF、初始 SQL 等）来自子模块
> [`llnut/dnf`](https://github.com/llnut/dnf)（清风 1031 版），挂在 `.origin_1031/`。
> 本仓库不包含任何第三方二进制；工具链由脚本从 CentOS Vault 下载。

## 目录

```
.origin_1031/      子模块 llnut/dnf：原版运行时与初始数据库（只读引用）
remake/
  source/          14 个服务的重建源码 + CMake（唯一入口 remake/source/CMakeLists.txt）
  secsvr/          secsvr 三件套（gunnersvr/zergsvr/secagent）重建源码（暂无一键构建，运行时用原版）
  runtime/         运行期辅助库源码（libtss_shmfix）
toolchains/        bootstrap.sh：下载/解包 GCC 4.x 工具链与 32 位库（产物不入库）
docker/            Dockerfile、compose.yaml、MySQL 初始化、启停脚本
auction_bot/       可选组件：拍卖行机器人（Python）
docs/              文档
```

## 快速开始（Docker）

```bash
git clone --recursive https://github.com/LoyieKing/rednf.git && cd rednf
cp docker/.env.example docker/.env          # 至少改 PUBLIC_IP 与两个密码
docker compose -f docker/compose.yaml --env-file docker/.env up -d --build
docker compose -f docker/compose.yaml logs -f dnf
```

详细步骤、端口、账号/GM、客户端连接见 **[docs/从零构建与启动.md](docs/从零构建与启动.md)**；
TSS、字符集、重启存档、排障见 **[docs/运维与排障.md](docs/运维与排障.md)**。

## 只构建二进制（不用 Docker）

需要 x86_64 Linux（已在 Debian 13 上验证），`cmake g++-multilib rpm2cpio cpio curl`，
以及 i386 的 `libssl-dev`（`dpkg --add-architecture i386 && apt install libssl-dev:i386`）。

```bash
git submodule update --init .origin_1031
bash toolchains/bootstrap.sh
cmake -S remake/source -B remake/build
cmake --build remake/build -j$(nproc)
# 产物：remake/build/<服务>/df_<服务>_r 与 remake/build/geoip/libGeoIP.so.1
```

两条构建路线：默认现代路线（`remake_verify=OFF`，宿主 GCC `-m32`，语义等价、可直接运行）；
验证路线（`-Dremake_verify=ON`，GCC 4.x，目标与原版逐字节一致）。见
[docs/df_modern_route.md](docs/df_modern_route.md)、[docs/df_binaries_identical.md](docs/df_binaries_identical.md)。

## 修改约定

- 默认行为必须与原版（ORIG）一致；有意的改动放在 `#if !remake_verify` 内并注明 MOD。
- 目前的 MOD：拍卖行机器人账号 `AUCTION_BOT_OWNER_ID (-114514)`，见 [docs/拍卖行.md](docs/拍卖行.md)。

## 许可证

尚未选定（见仓库说明）。子模块 `llnut/dnf` 为 MIT。
