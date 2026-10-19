# rednf —— DNF 台服 1031 服务端源码重建

## ⚠️ 许可与免责声明

**本仓库仅供学习与技术交流使用，严禁任何形式的商业用途。**

1. **许可协议**：除另有说明外，本仓库中由作者原创的内容依据 [知识共享 署名-非商业性使用-相同方式共享 4.0 国际许可协议（CC BY-NC-SA 4.0）](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode.zh-hans) 授权。协议全文见 `LICENSE` 文件。
2. **禁止商用**：不得将本仓库的全部或部分内容用于营利，包括但不限于运营收费服务器、出售或出租程序与服务、在其中进行付费或广告变现。
3. **第三方权利**：本项目所涉游戏名称、商标、客户端及游戏资源的著作权与其他权利归各自权利人所有。本仓库不提供、也不授权使用任何受版权保护的游戏客户端或游戏数据。作者无权、亦未曾将任何第三方权利授予使用者。上述许可仅适用于作者有权授权的部分，引用的第三方组件仍以其各自的许可条款为准。
4. **使用者责任**：使用者应自行确保其使用行为符合所在地法律法规及相关权利人的条款，并自行承担由此产生的全部风险与法律责任。
5. **免责**：本仓库按“现状”提供，不附带任何明示或默示的担保。在适用法律允许的最大范围内，作者不对因使用本仓库而产生的任何直接或间接损失承担责任。
6. **权利人联系**：如权利人认为本仓库的任何内容侵犯了其合法权益，或对本项目的版权存在任何疑问，请通过 724180662@163.com 联系作者并附上权属证明。作者核实后将及时处理，包括删除相关内容。

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

本仓库原创内容采用 CC BY-NC-SA 4.0（全文见 `LICENSE`），详见文首“许可与免责声明”。子模块 `llnut/dnf` 为 MIT。
