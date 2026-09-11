#!/bin/sh
# 生成 df_game_r 的动态导出符号映射（version script）—— 与 ORIG 二进制逐符号对齐。
#   用途：链接时 `-Wl,--version-script=game_export.map`，
#         使 EXE 的 .dynsym 导出面 = ORIG（消除 onlyNEW 的 libstdc++ 库实例，
#         并保证 --whole-archive 引入的 MySQL/yaSSL 符号按 ORIG 导出）。
#   用法：bash source/toolchain/gen_export_map.sh [ORIG路径] [输出]
ORIG="${1:-../dnf_installer/build/dnf_data/home/template/init/df_game_r}"
OUT="${2:-$(dirname "$0")/game_export.map}"
{
  echo "{"
  echo "  global:"
  nm -D --defined-only "$ORIG" 2>/dev/null | awk '{print $3}' | sort -u | while read -r s; do
    [ -n "$s" ] && printf '    "%s";\n' "$s"
  done
  echo "  local:"
  echo "    *;"
  echo "};"
} > "$OUT"
echo "wrote $OUT: $(grep -c '";' "$OUT") symbols"
