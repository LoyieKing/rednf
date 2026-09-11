// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NclKeyTools/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "NclKeyTools/ncl_char_plus.inc"


#include "NclKeyTools/ncl_check_digit_str.inc"


#include "NclKeyTools/ncl_make_mac.inc"
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::memcpy(ifr.ifr_name, "eth0", 5);
    ioctl(sock, SIOCGIFHWADDR, &ifr);
    close(sock);
    int off = 0;
    // [b28d-D43] 循环变量复用 sock 槽（ORIG 0x0857247c/0x08572483 同一 -0x10
    // 槽：4.1 对已死 sock 做 lifetime 复用；4.4 不复用，独立 i 会多占一槽并使
    // ifr 整体浅移 4 ⇒ 源码改用同一变量，槽序即与 ORIG 全等）。
    for (sock = 0; sock <= 5; ++sock)
    {
        std::sprintf(out + off, "%03d",
                     static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[sock]));
        off += 3;
        out[off] = '\0';
    }
}

#include "NclKeyTools/ncl_make_date.inc"
{
    // ORIG：time(&t)（传指针，P43）；tm 字段直读结构槽（非经 p 指针）。
    time_t t;
    std::time(&t);
    struct tm tmBuf;
    struct tm* p = localtime_r(&t, &tmBuf);
    // ORIG 0x85724f9：`je <nop 岛>` + sprintf + 尾 `jmp` + nop（P223 空块占位岛）。
    if (p == 0)
        goto L_done;
    std::sprintf(out, "%04d%02d%02d",
                 tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday);
L_done:;
}

#include "NclKeyTools/ncl_encrypt_key.inc"


#include "NclKeyTools/ncl_decrypt_key.inc"


#include "NclKeyTools/ncl_check_key.inc"


#include "NclKeyTools/ncl_check_date.inc"

