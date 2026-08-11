# guild 专用工具链（2026-08-11 第 11 轮集成用）：
# 默认 c6（c6root 4.4.7 驱动）；DNFTcpHandler/DNFSignalTranslator/
# DNFApplication/DNFGuildServerMain 4 个 TU 保留 c6444r（4.4.7 驱动 +
# 4.4.4-13 cc1plus）。A/B 实测：c6 全量 549 md，c6444r 全量 536 md，
# 本混合（默认 c6 + 4 TU c6444r）同样 536 md，且保住第 9 轮已 identical
# 的 13 个函数不回归。
include(/home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake/dnf_toolchain_44.cmake)
set(CMAKE_CXX_COMPILER /home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake/dnf_guild_cxx.sh)
