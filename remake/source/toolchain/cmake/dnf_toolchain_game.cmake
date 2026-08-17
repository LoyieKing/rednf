# game 专用工具链（2026-08-16 起）：df_game_r 逐 TU 编译器分发。
# 默认 c6（c6root 4.4.7 驱动）；后续按 df_game_r ORIG 的 TU 变体逐步调优
# （参考 guild：c6446r / c6444r 混合），见 dnf_game_cxx.sh 注释。
include(/home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake/dnf_toolchain_44.cmake)
set(CMAKE_CXX_COMPILER /home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake/dnf_game_cxx.sh)
