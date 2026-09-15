#!/bin/sh
# c6444r 变体：c6root 4.4.7 驱动 + Red Hat 4.4.4-13 cc1plus（monitor ORIG .comment 主体编译器）。
# 等价旧 /tmp/c6-g++-444r；头文件用 /tmp/lsd44/v4.4.4（dnf_helpers 按变体分发）。
ROOT="${DNF_TC_ROOT:-$(cd "$(dirname "$0")/../../.." && pwd)/toolchains}"
export LD_LIBRARY_PATH="${ROOT}/c6root/usr/lib64:${ROOT}/c6root/usr/lib"
# 2026-09-15 dbmw 泳道路由修订（A/B 实证，唯一变量=后端/头集；TU 四数 + nm 符号集见回报）：
#   DNFMySql.cpp       4.1.2(C5) tot=5  near=3  diff=35  -> 4.4.4 tot=39 near=0 diff=5  （+33 件）
#   DNFPacketTracer.cpp 4.1.2(C5) tot=20 near=11 diff=24 -> 4.4.4 tot=62 near=0 diff=5  （+31 件）
#   两 TU 的 ORIG 代码形态均为 4.4 系（4.1.2 口径的 near/diff 显著更差），故不再转入
#   dnf_dbmw_c5_gxx.sh；其余批次的 4.1.2 路由能力保留（去掉下面两行 case 项即恢复）。
SKIP_C5=0
PKT446=0
for arg in "$@"; do
    case "$arg" in
        */DBMW/DNFMySql.cpp|*/DBMW/DNFPacketTracer.cpp) SKIP_C5=1 ;;
    esac
done
# 2026-09-15 dbmw 泳道 A/B（唯一变量=后端；含 nm --defined-only 符号集逐名零差）：
#   DNFPacketTracer.cpp  4.4.4 tot=62 (near=1 diff=4)  ->  4.4.6(cc1plus446bin) tot=63 (ae+1、near 0)
#   （4.4.7/无 -B：tot=62 无收益；4.4.4 为基线）=> 本 TU 固定 4.4.6。
for arg in "$@"; do
    case "$arg" in
        */DBMW/DNFPacketTracer.cpp) PKT446=1 ;;
    esac
done
if [ "$SKIP_C5" = 0 ]; then
    for arg in "$@"; do
        if [ "$arg" = "-D_DNF_DBMW_C5_TU" ]; then
            exec "$(dirname "$0")/dnf_dbmw_c5_gxx.sh" "$@"
        fi
    done
fi
if [ "$PKT446" = 1 ] && [ -x "${ROOT}/cc1plus446bin/cc1plus" ]; then
    exec "${ROOT}/c6root/usr/bin/g++" -B "${ROOT}/cc1plus446bin/" "$@"
fi
if [ -x "${ROOT}/cc1plus444bin/cc1plus" ]; then
    exec "${ROOT}/c6root/usr/bin/g++" -B "${ROOT}/cc1plus444bin/" "$@"
fi
exec "${ROOT}/c6root/usr/bin/g++" "$@"
