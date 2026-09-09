// ============================================================================
// OnlyOrigBatch12_security_server.cpp —— 由 OnlyOrigBatch12.cpp 按 ORIG TU
// 边界拆出。目标 ORIG TU：SecurityServer.cpp
// （ORIG GI 键 = _Z16pi_2nd_print_binPcs @0x0827563e）。
// 本文件首个 file-scope 强符号·外部链接定义 = pi_2nd_print_bin ⇒
//   _GLOBAL__I__Z16pi_2nd_print_binPcs（P327）。
// 其余并入本文件的定义均排在 pi_2nd_print_bin 之后，不影响 GI 选名。
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

// ---------------------------------------------------------------------------
// PI-2nd 加密轮函数域（ORIG 0x080b46xx/0x082744a4）—— ORIG TU = SecurityServer.cpp。
// ===========================================================================
// pi_2nd_print_bin  ORIG T 0x082744a4（本文件首强实体 ⇒ GI 键名）
// ===========================================================================
void pi_2nd_print_bin(char* data, short count)
{
    // b28c-l23 根因翻案（旧"双槽不可翻"判词作废）：ORIG @0x082744aa 只出**一条**
    // 形参 spill（`mov 0xc(%ebp),%eax; mov %ax,-0x1c(%ebp)`）后 `cmpw $0x64,-0x1c`
    // 内存直比 ⇒ 原码是**直接 clamp 形参本身**（P77 形参直用）；我方引入具名
    // `short n` 使 4.4 多出 spill+`movzwl/mov %ax` 双槽物化（帧 0x48 vs 0x38）。
    // buf 侧 ORIG 为两条 dword 清零（`movl $0x0,-0x18/-0x14`）⇒ `char buf[8] = {0}`
    // （P202①/P214③：显式零初始化复现 4.1 逐字清零，第三条 -0x10 是 for 的 i）。
    if (count > 0x64)
        count = 0x64;
    char buf[8] = {0};
    for (int i = 0; i < count; ++i)
    {
        unsigned char v = static_cast<unsigned char>(data[i]);
        sprintf(buf, "%02x", v);
        printf("%s ", buf);
    }
}

static unsigned int ROL(unsigned int v, int bits)
{
    // ORIG _ZL3ROLji @0x080b46??：`mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx;
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
// T 0x08ad4548/0x08ad45bf/0x081b76??/0x08ad468b/0x08ad473e（Game 目标缺失，
// SecureStdio.cpp 仅入 point/auction 目标；本批补齐到 game）。模板版
// ss_sprintf<N> / ss_strcpy<N> 为 ORIG W（头内联模板），force 实例化。
// （ORIG TU = SecureStdio.cpp）
// ===========================================================================
int ss_sprintf(char* buffer, unsigned int count, const char* format, ...)
{
    // ORIG 0x08ad4548：va_list 槽（lea 0x14(%ebp) 单条）+ 返回值具名局部槽。
    va_list args;
    va_start(args, format);
    int r = ss_vsprintf(buffer, count, format, reinterpret_cast<char*>(args));
    va_end(args);
    return r;
}

int ss_swprintf(wchar_t* buffer, unsigned int count, const wchar_t* format, ...)
{
    // ORIG 0x08ad45bf：va_list 槽（lea 0x14(%ebp)）+ 返回值具名局部槽。
    va_list args;
    va_start(args, format);
    int r = ss_vswprintf(buffer, count, format, (char*)args);
    va_end(args);
    return r;
}

int ss_wcscpy(wchar_t* dest, unsigned int count, const wchar_t* src)
{
    // ORIG 0x08ad468b：else 块（清空 dest 返回 1）fall-through，
    // memcpy 块 jg 前跳；len 槽有符号比较（jg）⇒ (int)count > len。
    int len = wcslen(src);
    if ((int)count <= len)
    {
        dest[0] = L'\0';
        return 1;
    }
    memcpy(dest, src, (len + 1) * sizeof(wchar_t));
    return 0;
}

int ss_wcscat(wchar_t* dest, unsigned int count, const wchar_t* src)
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
int ss_sprintf(char (&buffer)[N], const char* format, ...)
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
int ss_strcpy(char (&dest)[N], const char* src)
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
template <>
__attribute__((weak)) int ss_sprintf<1024>(char (&buffer)[1024], const char* format, ...)
{
    return ss_vsprintf(buffer, 1024, format, (char*)__builtin_next_arg(format));
}

// ===========================================================================
// swap_value<IntervalSet*>  ORIG W 0x0857688f（模板；WindowsPackFiles.cpp
// 注释原样，本 TU force 发射）。
// ===========================================================================
template <class T>
void swap_value(T& a, T& b)
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

void forceEmitBatch12()
{
        char buf1024[1024];
        char buf256[256];
        char buf32[32];
        char buf512[512];
        char buf30[30];
        ss_sprintf(buf1024, "%d", 1);
        ss_sprintf(buf256, "%d", 1);
        ss_sprintf(buf32, "%d", 1);
        ss_sprintf(buf512, "%d", 1);
        ss_strcpy(buf30, "x");

        IntervalSet* a = 0;
        IntervalSet* b = 0;
        swap_value(a, b);

        stCeraShopBonusInfo_t ceraA;
        stCeraShopBonusInfo_t ceraB;
        ceraA = ceraB;

        WongWork::CGMAccounts::stGMInfo_t gm1; gm1.mid = 1; gm1.type = 2;
        WongWork::CGMAccounts::stGMInfo_t gm2; gm2.mid = 1; gm2.type = 2;
        volatile bool eq = (gm1 == gm2);

        stQuestTargetCharacter_t qt1(ENUM_CHARACTERJOB_0, 0, 0);
        stQuestTargetCharacter_t qt2(ENUM_CHARACTERJOB_0, 0, 0);
        volatile bool eq2 = (qt1 == qt2);

        stSeriaBlessingScript seria;
        stSeriaBlessingScript::Value v;
        volatile bool ok1 = seria.get_value(1, v);
        volatile bool ok2 = seria.is_possible_lv(1);

        stLimitItemUsageInfo lim;
        lim.Add(1, 2, 3);

        stNPCEventItemInfo npi;
        stNPCEventItemInfo npi2;
        npi = npi2;

        rewardSubKey rk1;
        rewardSubKey rk2;
        volatile bool lt = (rk1 < rk2);

        // [b29-SymForceBatch] tagRecommendNewbieOptionTable tnt 删除：ORIG 无
        // 该派生类的 C1/C2/D1/D2（仅 importRecommendNewbieOptionScript 以引用
        // 收参，从不构造/析构）；栈对象会发明四个 mangled。Table::clear（ORIG
        // W 0x0891ad8c）已由 importRecommendNewbieOptionScript 体内 out.clear()
        // （ORIG 0x89045b2 实证）自然 ODR-use，无需此处触发。

        (void)eq; (void)eq2; (void)ok1; (void)ok2; (void)lt;
}

}  // namespace
