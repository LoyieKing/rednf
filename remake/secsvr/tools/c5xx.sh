#!/bin/sh
# GCC 4.1.2（CentOS 5 root，与 secsvr 原始 SUSE GCC 4.1.0/4.1.2 同代）编译包装。
# 用法：c5xx.sh [g++ 参数...]（调用方自行加 -I<源码根>，如 -I source/gunnersvr）
# 注意：-nostdinc 需配合下方 isystem 路径；只用于 -c 编译，链接需另行处理。
C5=/tmp/c5root
# 原 secsvr 二进制为 -fPIC 编译（代码内大量 __i686.get_pc_thunk + GOT 寻址），
# 加 -fPIC 对齐代码生成，便于函数级助记符对照。
# -fno-implement-inlines：原二进制只发出少量 out-of-line inline 弱符号
# （TdrWriteBuf::textize、rapidxml parse_element/parse_node 等），与 GCC 4.1.0 默认一致。
exec $C5/usr/bin/g++ -m32 -O2 -DNDEBUG -pthread -fPIC -fno-implement-inlines -std=gnu++98 -nostdinc \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include \
  -isystem $C5/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed \
  -isystem $C5/usr/include/c++/4.1.2 \
  -isystem $C5/usr/include/c++/4.1.2/x86_64-redhat-linux \
  -isystem $C5/usr/include/c++/4.1.2/backward \
  -isystem $C5/usr/include \
  "$@"
