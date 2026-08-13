#!/bin/sh
# monitor 逐 TU 编译器分发（2026-08-13 第 44 轮集成）：
# - 默认 c6444r（4.4.7 驱动 + 4.4.4-13 cc1plus），与 ORIG 4.4.4-13 TU 对齐
# - DNFProhibitUserC5 / MemoryCashManagerC5：用 c5r52（4.1.2-52）。
#   ORIG .comment 中 4.1.2 x2；A/B 实测 4.1.2 的 register bool 比较产生
#   ORIG 的 setle %al + test %al,%al 形态，4.4.x 为 setle %bl 或 jg。
#   IsTimeOutConnectable / ProcessCashDataPrint 已拆到 C5 副本编译，
#   原 TU 其余函数保持 c6444r 不回退。
ROOT=/home/loyieking/dnf_workspace/dnf_decompile/source/toolchain/cmake
C6444R="$ROOT/dnf_c6444r_gxx.sh"
C6446R="$ROOT/dnf_c6446r_gxx.sh"
C5R52="$ROOT/dnf_c5r52_gxx.sh"
src=""
prev=""
for a in "$@"; do
    if [ "$prev" = "-c" ]; then
        src="$a"
        break
    fi
    prev="$a"
done
case "$(basename "$src" .cpp)" in
    EventActionManager)
        exec "$C6446R" -fno-exceptions "$@" ;;
    DNFApplication|DNFMonitorServerMain|DNFServerHandler|DNFSignalTranslator|DNFTcpHandler|DNFTableBase|DNFThreadInterface|TaskScheduler)
        exec "$C6446R" "$@" ;;
    DNFProhibitUserC5|MemoryCashManagerC5)
        filtered=""
        for a in "$@"; do
            if [ "$a" = "-std=gnu++0x" ]; then
                continue
            fi
            case "$a" in
                -isystem*|-I*)
                    continue ;;
            esac
            filtered="$filtered $a"
        done
        C5INC="-isystem /tmp/c5r52tool/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
-isystem /tmp/c5r52tool/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
-isystem /tmp/c5r52tool/usr/include/c++/4.1.2 \
-isystem /tmp/c5r52tool/usr/include/c++/4.1.2/x86_64-redhat-linux \
-isystem /tmp/c5r52tool/usr/include/c++/4.1.2/backward \
-isystem /tmp/c5r52tool/usr/include \
-I/home/loyieking/dnf_workspace/dnf_decompile/source/cmake/monitor \
-I/home/loyieking/dnf_workspace/dnf_decompile/source/DNFServer/ServerCommon \
-I/home/loyieking/dnf_workspace/dnf_decompile/source/DNFServer/GameServer/Monitor \
-I/home/loyieking/dnf_workspace/dnf_decompile/source/shared/packet/include \
-I/home/loyieking/dnf_workspace/dnf_decompile/source/shared \
-I/home/loyieking/dnf_workspace/dnf_decompile/source/shared/common/include"
        exec "$C5R52" $filtered $C5INC ;;
    *)
        exec "$C6444R" "$@" ;;
esac
