// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFFunctionLib/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "DNFFunctionLib.h"


#include <iostream>

#include <dirent.h>
#include <execinfo.h>
#include <iconv.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "DNFFileLog.h"






#include "DNFFunctionLib/Char2Hex.inc"


// ---- 字符集全局（原始为 file-scope static std::string，_ZL 本地符号） ----
#ifndef DF_NO_CODEPAGE
static std::string gClientEncoding;
static std::string gScriptEncoding;
static std::string gDatabaseEncoding;
#endif

// ---- 自由函数 ----

// 原始为 TLS 缓冲区 gNumberToStringBuffer（符号 0x200 字节，GS-0x208 + index*0x40，8 槽）
// DWARF 类型为 char[8][0x40]（两个 subrange：7/63），必须按二维数组形态书写，
// 才能让 &gNumberToStringBuffer[index] 的 PLUS 以「基址寄存器为目的地」展开（lea），
// 与 ORIG 一致；写成 char[0x200] + index*0x40 会产生 add 形态（§39 伪影）。
static __thread char gNumberToStringBuffer[8][0x40];

// 注（2026-08-11 修正）：不要在此处“还原”匿名命名空间的 get_global()。
// ORIG 二进制 TLS 块中的 _ZZN12_GLOBAL__N_110get_globalEvE6global（.tbss
// 偏移 0x200，位于 gNumberToStringBuffer 之后，使缓冲区地址为 GS-0x208）
// 来自静态链接的 libstdc++ eh_globals.o（标准库内部符号），并非项目函数。
// 曾按同名同形伪造一份，导致本项目对象与 libstdc++ 各持一个同名 LOCAL TLS
// 符号、链接不合并，.tbss 变 0x210、缓冲区偏移错误地变成 GS-0x210。
// 删除伪造定义后，仅保留 libstdc++ 提供的唯一副本，TLS 布局恢复 GS-0x208。

// [b28d-solo-DNFFunctionLib] AE（ORIG 0x0810904b，22 insn）——本泳道 fresh 复验：
// /tmp/tu_DNFFunctionLib.o `_Z14NumberToStringji` 22/22 逐条同序，唯 3 处重定位差：
// ①TLS LE `lea 0x0(%eax),%eax`+R_386_TLS_LE ↔ ORIG `lea -0x3c550(%eax),%eax`；
// ②`movl $0x0,0x4(%esp)`+R_386_32 .rodata ↔ ORIG `movl $0x8b446ea`（"%u"）；
// ③`call sprintf`+R_386_PC32 ↔ ORIG `call sprintf@plt`。函数体形 =
// `sprintf(gNumberToStringBuffer[index], "%u", value); return
// gNumberToStringBuffer[index];`（P0：无具名局部，实参/返回值右值直传；二维 TLS
// 数组使 &buf[index] 折 lea 而非 add）。残差归因：TLS LE 偏移属跨 TU/链接期 TLS 组
// 布局（P280 族）；.o 重定位标签差属 P264 显示口径 —— game_func_report
// --no-compile 因 md 撞名（NumberToString.md 为 ji 基线）假报 DIFF，TU 口径 ae。
// 无源码改动；无 register asm 绑定（P0 合规）。
char* NumberToString(unsigned int value, int index) {
    sprintf(gNumberToStringBuffer[index], "%u", value);
    return gNumberToStringBuffer[index];
}

// [b28d-solo-DNFFunctionLib] AE（ORIG 0x08109098，29 insn）——本泳道 fresh 复验：
// /tmp/tu_DNFFunctionLib.o `_Z14NumberToStringyi` 29/29 逐条同序，唯 3 处重定位差：
// ①TLS LE `lea 0x0(%eax),%eax`+R_386_TLS_LE ↔ ORIG `lea -0x3c550(%eax),%eax`（两处）；
// ②`movl $0x3,0x4(%esp)`+R_386_32 .rodata ↔ ORIG `movl $0x8b446ed`（"%qu"）；
// ③`call sprintf`+R_386_PC32 ↔ ORIG `call sprintf@plt`。函数体形同 ji 版；ulonglong
// 形参的 -0x10/-0xc 双槽 + %ecx 缓存 buffer 地址为 i386 8 字节实参 ABI 拆分形
// （ORIG 同形，非具名局部）。残差归因同上（P280 TLS 组布局 / P264 重定位显示口径）。
// 无源码改动；无 register asm 绑定（P0 合规）。
char* NumberToString(unsigned long long value, int index) {
    sprintf(gNumberToStringBuffer[index], "%qu", value);
    return gNumberToStringBuffer[index];
}

#include "DNFFunctionLib/PrintBackTrace.inc"

#include "DNFFunctionLib/StringToNumber.inc"

#include "DNFFunctionLib/checkTimeToday.inc"

// ORIG T 0x0810912a _Z23GetCurrentResetBaseTimeli（返回 reset 时间戳 long）。
#include "DNFFunctionLib/GetCurrentResetBaseTime.inc"

// 反编译：saucHex = "0123456789abcdef"

#include "DNFFunctionLib/Hex2Char.inc"

#ifndef DF_NO_CODEPAGE
unsigned int SDC_Rand(unsigned int* seed) {
    if (seed == NULL) {
        return 0;
    }
    // 原始：单累加器逐步 LCG（imul → 存栈 → add [ebp] 内存 RMW；result 逐步 shl/xor）
    unsigned int a = *seed;
    a *= 0x41c64e6d;
    a += 0x3039;
    unsigned int result = (a >> 0x10) & 0x7ff;
    a *= 0x41c64e6d;
    a += 0x3039;
    result <<= 10;
    // ORIG xor 为寄存器形态（edx 装载掩码、eax 装载 result、xor、存回）；
    // 直接 result ^ mask 在 4.4.6-3 -O0 下折叠为 xor %eax,mem。(int) 转换包裹复现 ORIG。
    result = (unsigned int)((int)result ^ (int)((a >> 0x10) & 0x3ff));
    a *= 0x41c64e6d;
    a += 0x3039;
    result <<= 10;
    result = (unsigned int)((int)result ^ (int)((a >> 0x10) & 0x3ff));
    *seed = a;
    return result;
}
#endif

// ---- DNFFLib ----

#include "DNFFunctionLib/Binary2Hex.inc"

#include "DNFFunctionLib/Hex2Binary.inc"

// ORIG 证据（全部 9 个服务）：r / 65536 用非 cmov 的
// mov edx,eax; sar 31,edx; shr 16,edx; lea; sar 16 形态（i586 除法定型），
// 与 -march=i586 产物逐指令一致；§89 允许函数级 target 覆盖。
// divide==0 分支为 switch 形态（mov divide,%eax; test; jne），不能写成 if。
__attribute__((target("arch=i586")))
#include "DNFFunctionLib/get_rand_int.inc"

// ORIG 中自由函数 get_rand_int 与 DNFFLib::get_rand_int 为同字节的独立复制体
// （73 条指令逐条一致），并非包装调用；此处保持与成员版本相同的完整实现。
// [DNF_SVC_STATICS] df_statics_r 例外：ORIG 该二进制的自由函数 get_rand_int（0x8062b28）
//   属于一个小 TU —— 其静态初始化函数 _GLOBAL__I__Z12get_rand_inti 只初始化一个
//   ios_base::Init，正是 Statics/TaskImpl.cpp 那个 TU（该文件已提供定义）；
//   而 DNFFunctionLib.o 那个 TU（_GLOBAL__I__Z8Char2HexhPc）只初始化
//   ios Init + 3 个 std::string，与 ORIG 同址 TU 逐条对应，其中不含自由函数
//   get_rand_int。故 statics 构建下屏蔽此副本，避免同一符号两处强定义（链接冲突）
//   并保持与 ORIG 相同的 TU 归属。其它服务不受影响（本宏仅 statics 目标定义）。
// [DNF_SVC_MONITOR] df_monitor_r 同款例外：ORIG 该二进制的自由函数 get_rand_int
//   （0x0809bf18）位于 TaskImpl.o —— .text 中紧邻 comp_by_time(0x0809bf04)、
//   后接 CTask_ChristmasEvent 一族（0x0809c00e 起），该 .o 的 GI 键为
//   _GLOBAL__I__Z12get_rand_inti(0x0809c4a3)；DNFApplication.o 区（GI 键
//   CApplicationInstance 0x08066025）与 DNFFunctionLib.o 区（_GLOBAL__I__Z8Char2HexhPc）
//   均相隔 ~0x3a000 字节且不含该符号。旁证：df_auction/community/coserver/guild/
//   manager/point/bridge 这 7 个 ORIG 都编入了 DNFFunctionLib.o（ExplodeString 在）
//   却没有自由 get_rand_int ⇒ ORIG 的 DNFFunctionLib.cpp 本来就不定义它。
//   Monitor/TaskImpl.cpp 已按 ORIG 提供定义（该 TU 首强实体，GI 键一致），故此处
//   屏蔽此副本，避免 _Z12get_rand_inti 两处强定义（链接冲突）并保持 ORIG 的 TU 归属。
//   其它服务不受影响（DNF_SVC_MONITOR 仅 monitor 目标定义）。
#if !defined(DNF_SVC_STATICS) && !defined(DNF_SVC_MONITOR)
__attribute__((target("arch=i586")))
#include "DNFFunctionLib/get_rand_int_1.inc"
#endif

#include "DNFFunctionLib/ExplodeString.inc"

#include "DNFFunctionLib/PrintTextFile.inc"

#include "DNFFunctionLib/fPrintTextFile.inc"

#ifndef DF_NO_CODEPAGE
#include "DNFFunctionLib/CharacSetSwitch.inc"

#include "DNFFunctionLib/ConvertGBKtoUTF8.inc"

#include "DNFFunctionLib/ConvertUTF8toGBK.inc"
#endif

#include "DNFFunctionLib/Make_Dir.inc"

#include "DNFFunctionLib/Sleep_Ext.inc"

#ifndef DF_NO_CODEPAGE
// ---- CodePage ----

#include "DNFFunctionLib/initCodePage.inc"

#include "DNFFunctionLib/client.inc"

#include "DNFFunctionLib/script.inc"

#include "DNFFunctionLib/database.inc"

#include "DNFFunctionLib/client2Database.inc"

#include "DNFFunctionLib/database2Client.inc"

#include "DNFFunctionLib/database2Script.inc"

#include "DNFFunctionLib/script2Database.inc"
#endif
