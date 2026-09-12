// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch12_security_server/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "CEnvironment.h"        // CEnvironment / less_TCHAR / ENUM_SERVER_GROUP
#include "DNFLexWrapper.h"       // DNFLexWrapper::setStringTableFromBuffer
#include "LogManager.h"          // LogManager::logFormat
#include "PackSet.h"             // PackSet
#include "NpcScript.h"           // STScriptFileList / loadRDARScriptListFile
#include "CreatureScript.h"      // g_moduleNameTag
#include "GreyImage.h"           // GreyImage
#include "ServicePolicy.h"       // ServicePolicy
#include "ServiceRestrictManager.h"  // RestrictCategory::Enum
#include "CDataManager.h"        // rewardSubKey / stLimitItemUsageInfo
#include "CItemList.h"           // stCeraShopBonusInfo_t
#include "CGuildServerProxy.h"   // stGMInfo_t
#include "Quest.h"               // stQuestTargetCharacter_t
#include "TagRecommendNewbieOption.h"  // tagRecommendNewbieOptionTable
#include "Interval.h"                // IntervalSet

#include "OnlyOrigBatch12_security_server/pi_2nd_print_bin.inc"


static unsigned int ROL(unsigned int v, int bits)
{
    // ORIG _ZL3ROLji @0x0809c418（range 0x0809c418..0x0809c42c，10 条）：
    // `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx;
    // mov %edx,%ecx; rol %cl,%eax; mov %eax,0x8(%ebp); mov 0x8(%ebp),%eax`
    // = 结果写回形参槽再读回 ⇒ 原码 `v = ...; return v;`（P0 槽往返 ⇒ 有具名
    // 左值回写）；直接 `return (v<<bits)|(v>>(32-bits))` 使 4.4 用 %ebx 缓存
    // 中间值（push ebx + 12 条）。
    // [b28d-solo-OnlyOrigBatch12] N7/N18a 封顶（原判词复证 + 4.4.6 复测）：
    // ORIG 10 条 = `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,%ecx;
    // rol %cl,%eax; mov %eax,0x8(%ebp); mov 0x8(%ebp),%eax`（4.1 不折内存 RMW）。
    // 4.4 纯表达式形恒折 RMW `roll %cl,0x8(%ebp)`（8 条）；具名 t 槽 12 条、
    // 裸 register %ebx 12 条、`*&v`/引用/指针别名/交换加法序等探针
    // /tmp/b28d12/probe_rol{,2}.cpp 二十形全败；c6446r(4.4.6) 复测同 8 条形
    // （probe_rol_446.o）⇒ 非后端可翻。P248 三条件不成立（无跨 call 保活、
    // ORIG 目标为 caller-saved %eax）⇒ 保留最简纯表达式形，残差封 N7+N18a。
    v = (v << bits) | (v >> (32 - bits));
    return v;
}

static void pi1(unsigned long* a)
{
    a[1] ^= a[0];
}

static void pi2(unsigned long* a, unsigned long* b)
{
    unsigned long t = a[1] + b[0];
    t = ROL(t, 1) + t - 1;
    t = ROL(t, 4) ^ t;
    a[0] ^= t;
}

static void pi3(unsigned long* a, unsigned long* b)
{
    unsigned long t = a[0] + b[1];
    t = ROL(t, 2) + t + 1;
    t = ROL(t, 8) ^ t;
    t += b[2];
    t = ROL(t, 1) - t;
    t = ROL(t, 16) ^ (a[0] | t);
    a[1] ^= t;
}

static void pi4(unsigned long* a, unsigned long* b)
{
    unsigned long t = a[1] + b[3];
    t = ROL(t, 2) + t + 1;
    a[0] ^= t;
}

// setup  ORIG t 0x080b482e：8 轮扩展（pi1/pi2/pi3/pi4 交替，后 4 轮用 b+4）。
static void setup(unsigned long* a, unsigned long* b, unsigned long* c)
{
    unsigned long d[2];
    d[0] = a[0];
    d[1] = a[1];
    int i;       // 槽分配按声明序（i@-0x8、n@-0x4），store 序按初始化/赋值
    int n = 4;   // 语句序：ORIG 先 n=4 后 i=0 → 声明与赋值分离复现。
    i = 0;

    // ORIG 交错：pi1,pi2→c=d0, pi3→c=d1, pi4→c=d0, pi1→c=d1,
    //           pi2(b+n)→c=d0, pi3→c=d1, pi4→c=d0, pi1→c=d1。共 8 写。
    pi1(d);
    pi2(d, b);
    c[i++] = d[0];
    pi3(d, b);
    c[i++] = d[1];
    pi4(d, b);
    c[i++] = d[0];
    pi1(d);
    c[i++] = d[1];
    pi2(d, b + n);
    c[i++] = d[0];
    pi3(d, b + n);
    c[i++] = d[1];
    pi4(d, b + n);
    c[i++] = d[0];
    pi1(d);
    c[i++] = d[1];
}

// ===========================================================================
// ss_sprintf / ss_swprintf / ss_strcpy / ss_wcscpy / ss_wcscat —— ORIG
extern int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList);
extern int ss_vswprintf(wchar_t* buffer, size_t count, const wchar_t* format, char* argList);
extern int ss_strcpy(char* dest, size_t count, const char* src);
#include "OnlyOrigBatch12_security_server/ss_sprintf.inc"


#include "OnlyOrigBatch12_security_server/ss_swprintf.inc"


#include "OnlyOrigBatch12_security_server/ss_wcscpy.inc"


#include "OnlyOrigBatch12_security_server/ss_wcscat.inc"
{
    // ORIG 0x08ad473e：count 槽有符号 jg ⇒ (int)count <= cur+len 为守卫，
    // return 1 fall-through、memcpy 块 jg 前跳；memcpy 首参按字节
    // (char*)dest+cur*4（shl $2+add），size (len+1)<<2（add+shl+mov edx）。
    int cur = wcslen(dest);
    int len = wcslen(src);
    if ((int)count <= cur + len)
    {
        return 1;
    }
    memcpy(&dest[cur], src, (len + 1) << 2);
    return 0;
}

// ---- 模板版（ORIG W：ss_sprintf<1024/256/32/512>、ss_strcpy<30>）----
extern int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList);
extern int ss_strcpy(char* dest, size_t count, const char* src);
template <size_t N>
#include "OnlyOrigBatch12_security_server/ss_sprintf_1.inc"
{
    // [b28d-solo-OnlyOrigBatch12] 主模板体 = ORIG 256/32/512 三实例形
    // （0x08aa69b4/0x08ac36de/…：sub $0x28 + va_list 槽 -0x10 + 返回槽 -0xc
    // 中转），三件与本源形逐条全等 AE。1024 实例 @0x08acf0e0 为 sub $0x18
    // 无槽直传形（同模板自相矛盾 = N105）⇒ 由下方 ss_sprintf<1024> 显式特化
    // 单独复现（判词见该处）。反向删 va_list/返回槽会把 256/32/512 三件打成
    // DIFF（P211 陷阱法联判已否）。
    va_list args;
    va_start(args, format);
    int r = ss_vsprintf(buffer, N, format, (char*)args);
    va_end(args);
    return r;
}

template <size_t N>
#include "OnlyOrigBatch12_security_server/ss_strcpy.inc"
{
    return ss_strcpy(dest, N, src);
}

// [b28d-solo-OnlyOrigBatch12] ss_sprintf<1024> 显式特化（weak，与 ORIG W 绑定一致）：
// ORIG 0x08acf0e0 为 13 条无槽直传形（`sub $0x18; lea 0x10(%ebp),%eax;
// mov %eax,0xc(%esp); … call; leave; ret`），与同模板 256/32/512 的 va_list 槽形
// （sub $0x28 + -0x10/-0xc 槽）自相矛盾 ⇒ 1024 实例为独立源形。特化体用
// __builtin_next_arg(format) 直接取首个变参地址（i386 SysV 即 &format+4，语义等价
// va_start 初始化），复现无 va_list 槽直传；4.4 仅余一条 eax→edx 实参暂存
// （N18a/P75 装载序，13→14 条，探针 /tmp/b28d12/probe_ss{,2,3,4,5}.cpp 十余形全败）。
// 主模板体保持不变（256/32/512 三实例 AE 零回归）。
// [SemB05 2026-09-12] ss_sprintf<1024> 显式特化**删除**：本 TU（-O0）三种写法实测
// 只能得 14 条（__builtin_next_arg + cast）或 17 条（普通 va_list），ORIG 的 13 条
// 无槽直传形不可达。该 .o 在链接行（objects1.rsp）中先于 DNFLexWrapperStream.cpp.o
// 出现 ⇒ 原特化会覆盖后者的**逐条全等 13 条**实例（-O2 下两种 cast 写法均产 13 条）。
// 删除特化 + 入口改用非模板重载后，本 TU 不再定义该符号，链接取 DNFLexWrapperStream
// 版本 ⇒ 与 ORIG 13/13 全等。语义不变（同一 va_start/va_end 契约）。

// ===========================================================================
// swap_value<IntervalSet*>  ORIG W 0x0857688f（模板；WindowsPackFiles.cpp
// 注释原样，本 TU force 发射）。
// ===========================================================================
template <class T>
#include "OnlyOrigBatch12_security_server/swap_value.inc"
{
    T tmp = a;
    a = b;
    b = tmp;
}

// ===========================================================================
// Force 发射（P147：头内联 W / 模板实例化缺失时显式使用一次）
// ===========================================================================
namespace
{

#include "OnlyOrigBatch12_security_server/forceEmitBatch12.inc"


}  // namespace
