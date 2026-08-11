#!/bin/sh
# c5r52 变体：c5r52tool gcc-4.1.2-52（ORIG coserver .comment 精确版本）驱动。
# 用于按 TU 指定 4.1.2-52 编译（DNFTickHandler / DNFUserInOutCounter 等）。
ROOT="${DNF_TC_ROOT:-/tmp}"
export LD_LIBRARY_PATH="${ROOT}/c5r52tool/usr/lib64:${ROOT}/c5r52tool/usr/lib:${ROOT}/c5root/usr/lib64:${ROOT}/c5root/usr/lib"
exec "${ROOT}/c5r52tool/usr/bin/g++" "$@"
