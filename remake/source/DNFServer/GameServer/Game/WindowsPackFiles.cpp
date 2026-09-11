// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WindowsPackFiles/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CObfuscatorC2Ev` / `_GLOBAL__I__Z14LoadClientFilePmPPvPKc`
// （ORIG 0x8585f03 / 0x8575e3c，两者同属本 TU 的两个 ORIG CU 键名，取首强符号者）。
#include <iostream>
// [b28d-64 判词汇总 · 泳道64] 本 TU 10 件（明细 /tmp/b28d_lane_64_result.tsv）：
//   LoadClientFile 66/66 AE；IsDifferentExeVersionRoughly 66/66 AE；
//   ~WindowsDataFileFamily 77/77 AE；~WindowsExeFileFamily 94/94 AE；
//   ~WindowsExeFile 151/151 AE；GenerateRange 106/109 N封顶（P198 边界）；
//   WindowsDataFileFamily::GenerateChecksum 93/106 N82；
//   WindowsExeFileFamily::GenerateChecksum 97/111 N82；
//   LoadWindowsFile 163/169 N封顶（P167 CLEAR 21/21）；
//   CodeProtectorServer::GenerateChecksum 152/162 N封顶（P167 CLEAR 10/10）；
//   MakeCommonWindowsExeConstantSections 164/179 N91（P167 EXTRA Interval D1×4）；
//   CompleteLoadingWindowsExeFiles 279/281 N91（EXTRA Interval D1×2）；
//   ReadPeHeader 551/532 N82（P167 MISSING 迭代器遍历×2 + IntervalSet 段，我方索引形）。
//   注：本清单含 13 行（TSV 10 件 + 3 件 D58 已判同 TU 归因）。
// [b28d-solo-WindowsPackFiles 判词汇总 · 单文件泳道] 8 件
//   （明细 /tmp/b28d_solo_WindowsPackFiles_result.tsv）：
//   AE/IDENTICAL 5：WindowsExeFileFamily::GenerateRange（P211 档③ 成员 unsigned long
//     + P198① 两句式预清 + 4 形槽序探针 genD）、WindowsDataFileFamily::GenerateChecksum
//     （迭代器形 + 正向 if + P247 8 字节视图）、WindowsExeFileFamily::GenerateChecksum
//     （同型）、LoadWindowsFile（exe/data 域序镜像翻正 + 单 return false 共享尾 +
//     嵌套守卫拆 && 物化）、WindowsCodeProtectorServer::GenerateChecksum
//     （P198① 预清 + if(m_loaded) 单出口包裹 + 扁平 else-if + (int)size() setle）。
//   N 封顶 3：MakeCommonWindowsExeConstantSections（N95/N82，p167 CLEAR 20/20）、
//     CompleteLoadingWindowsExeFiles（N95/N89，p167 CLEAR 37/37）、ReadPeHeader
//     （P165 双份展开/N82/N89，p167 差集为等价改写形态）。
//   [b29-R28 复核] 三件 p167 差集 = **P93 同址别名伪差**（本仓 nm 实证）：
//     ORIG 同一地址多符号 —— IntervalSet::C1Ev=C2Ev=0x8575e9c、D1Ev=D2Ev=0x8575eb0；
//     Interval::C1Ev=C2Ev=0x8575e58、C1Ell=C2Ell=0x8575e5e；
//     vector<IntervalSet*>::D1Ev=D2Ev=0x8576832。objdump 在同址多符号时选名不同
//     （C1 vs C2、D1 vs D2）⇒ MISSING/EXTRA 成对抵消，非语义缺口。
//     残余 = N95 全帧槽号浅移（shift.py 三件均 MIXED：ORIG 96/256/156 vs
//     我方 104/272/156）+ 4.4 聚合优先低槽分区（N82附）。语义等价。
//   .h 改动 2（均已 R2 广播 + 消费点核对）：WindowsPackFiles.h m_curSectionIndex
//     int→unsigned long（P211 档③）；CodeHackChecksum.h CodeHackCheckRangeEx
//     匿名 union {m_start104/m_size108; CodeHackCheckRange m_range;}（P247）。
//   P0：本文件无 register asm 绑定（0 命中，无需清绑）。
#include "WindowsPackFiles.h"
#include "AdvanceAltarZip.h"
#include "MySQL.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

extern "C" void* memset(void*, int, std::size_t);

// ---------------------------------------------------------------------------
#include "ShaSupport.h"          // StrCpyMaxPath
// LoadClientFile —— _Z14LoadClientFilePmPPvPKc T @0x08572f3a（0xf2）
// fopen("rb") → fseek END 取长 → new → fread(1) 成功即 1。
// ---------------------------------------------------------------------------
// [N 类标注] 0x08572f3a：66/65（差 1），P167 调用点 9/9 双向差集 0 ⇒ 纯指令形差：
//  ① ORIG fread 返回值为 `cmp $0x1,%eax; setne %al; test %al,%al; je`（比较结果
//     经 setne 物化后再分支，且结果不落槽），我方按具名 n 变量存槽后 `cmpl;je`
//     （范式库 setcc-fold 负例的镜像变体：4.4 恒把该形折回存槽+整数比较）；
//  ② ORIG new[] 返回值以 %edx 直载跨两句使用（无缓存槽），4.4 走 -0x10 槽存取；
//  ③ fopen==0 的失败出口 ORIG 为 `je 远端 +0xeb` 单跳，4.4 生成 `jne 跳过 +
//     mov $0 + jmp` 三句岛。三者皆编译器版本 RA/分支物化策略，源码侧不可控。
#include "WindowsPackFiles/LoadClientFile.inc"
{
    FILE* fp = fopen(path, "rb");
    // [b28d-64] P59/P63：ORIG 0x08572f5a `je <尾部 mov $0; leave; ret>` 单跳，
    // 负形早返被 4.4 内联成 `jne body; mov $0; jmp` 岛 ⇒ 改正向包裹让失败出口归共享尾。
    if (fp != 0)
    {
        fseek(fp, 0, SEEK_END);
        *outSize = static_cast<unsigned long>(ftell(fp));
        fseek(fp, 0, SEEK_SET);
        // [b28d-64] P0/P292：ORIG 0x08572fb5 new[] 结果 %edx 直载（无 buf 槽）；
        // 0x08572fe1 fread 结果寄存器直比 `setne %al; test %al; je`（无具名 n 槽）；
        // 0x08572ff3 delete 为显式 `operator delete`（无 4.4 自带判空 test/je）。
        *outBuf = new unsigned char[*outSize];
        if (fread(*outBuf, *outSize, 1, fp) != 1)
        {
            operator delete(*outBuf);   // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样）
            *outBuf = 0;
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// CompareWindowsExeConstantSectionsForSort —— t @0x08572eef（qsort 比较器）
// 比较 *(unsigned long*)*lhs 与 *(unsigned long*)*rhs。
// ---------------------------------------------------------------------------
static int CompareWindowsExeConstantSectionsForSort(const void* lhs, const void* rhs)
{
    WindowsExeConstantSection* const* a =
        static_cast<WindowsExeConstantSection* const*>(lhs);
    WindowsExeConstantSection* const* b =
        static_cast<WindowsExeConstantSection* const*>(rhs);
    // ORIG 0x08572eef 逐条：两次比较**各自重读** `(*p)->start`（+0x12..+0x20 与
    // +0x2b..+0x39 两组各 6 insns 的 mov/mov/mov/cmp），⇒ 无 sa/sb 缓存量；
    // 尾部 `cmp; jne +0x44; mov $0; jmp; +0x44: mov $1` 即三目常量岛形。
    if ((*a)->start < (*b)->start)
    {
        return -1;
    }
    // 三目形被 4.4 折成 `setne+movzbl`（−3 insns，范式库 setcc-fold 负例的
    // 比较结果变体）；改回显式 if + 常量返回即复现 ORIG `jne L; mov $0; jmp;
    // L: mov $1` 双常量岛。
    if ((*a)->start == (*b)->start)
    {
        return 0;
    }
    return 1;
}

// ===========================================================================
// CObfuscator —— 位重排混淆（7bit 有效位流水）
// ===========================================================================
#include "WindowsPackFiles/CObfuscator.inc"
{
    // [REAL 修复·TU 拆分] ORIG 0x08585888 形：base CBitManager ctor → 局部临时
    // CBitManager ctor(0x085858a1)/dtor(0x085858ac) + EH landing pad
    // (+0x2b..+0x41：temp 半构造时析构基类子对象 + _Unwind_Resume)。
    // 本件原判 N-EH（14 vs 29），根因经查不是版本不可复现，而是**TU 归属错**：
    // c6root g++ 4.4.7 见到同 TU 内 `CBitManager::~CBitManager(){}` 空体即推导
    // nothrow，从而对该 temp 不发 cleanup（4.1 无此推导）。ORIG 侧 CBitManager
    // 全族连续落在 0x0831c09c..0x0831c3af、其静态初始化组为
    // _GLOBAL__I__ZN11CBitManagerC2Ev，而 CObfuscator 在 0x085858xx、组为
    // _GLOBAL__I__ZN11CObfuscatorC2Ev ⇒ 原属不同编译单元。
    // 修复（Main 批准）：CBitManager 的 ctor/dtor/SetAll/ReadData/GetBitFromLSB/
    // ShiftRight/ShiftLeft/GetBit 定义整体迁出至 Game/CBitManager.cpp（已登记
    // source/cmake/game/CMakeLists.txt），本 TU 仅经 WindowsPackFiles.h 取声明。
    // 实测本函数 orig_insns=29 / our_insns=29，landing pad 与 -0x18(%ebp) 槽、
    // push %esi/%ebx + sub $0x20 帧全部逐条复现（P167 调用点 5/5，仅 ORIG 侧
    // C1/D1 与我们 C2/D2 的构造别名地址同源）；连带的 CObfuscator::~CObfuscator
    // 亦由 NEAR 退出。残余仅 ret 后对齐 nop（§6.1 豁免）与 .o 未 relink 的
    // call 占位（§6.3 假阳性）。
    CBitManager temp;
}

#include "WindowsPackFiles/dtor_CObfuscator.inc"
{
}

#include "WindowsPackFiles/ObfuscatePacket.inc"
{
    ReadData(data, len);
    // ORIG 0x085858ea +0x1b..+0x46：`mov 0x14(%ebp),%eax; cmp $0x1,%eax; je; 
    // cmp $0x2,%eax; je; jmp 尾` ⇒ 值升序 switch 的 4.1 决策树形（P165②）；
    // 旧 if/else-if 链被 4.4 做成 `cmpl $1,mem; jne` + 第二比较后置（少 2 insns）。
    switch (type)
    {
    case 1:
        Obfuscate1();
        break;
    case 2:
        Obfuscate2();
        break;
    default:
        break;
    }
    return reinterpret_cast<int>(m_buffer);
}

#include "WindowsPackFiles/DeobfuscatePacket.inc"
{
    ReadData(data, len);
    // 同上（ORIG 0x08585938 同型 switch 决策树）
    switch (type)
    {
    case 1:
        Deobfuscate1();
        break;
    case 2:
        Deobfuscate2();
        break;
    default:
        break;
    }
    return reinterpret_cast<int>(m_buffer);
}

#include "WindowsPackFiles/Obfuscate1.inc"
{
    // ORIG 槽序（P28/P38）：i(-0x18 函数首 `movl $0,-0x18` 预初始化) →
    // total(-0x14，`shl$3; mov slot` 一次物化，/7 与 %7 两条 magic 链均
    // `mov -0x14,%ecx` reload) → cnt(-0x10)。
    int i = 0;
    int total = m_size * 8;
    int cnt = total / 7;
    m_field0c = total % 7;                                   // 余位（this+0xc）
    for (i = 0; i <= cnt; ++i)
    {
        unsigned char bits = GetBitFromLSB(7);
        if ((i >= 1) && (i < cnt))
        {
            ShiftLeft(7, m_buffer);
        }
        else if (i == cnt)
        {
            ShiftLeft(m_field0c, m_buffer);
        }
        m_buffer[m_size - 1] |= bits;
    }
    return 0;
}

#include "WindowsPackFiles/Deobfuscate1.inc"
{
    // 同 Obfuscate1：i 提首 + total 槽物化（ORIG 0x08585aaa 头同构）。
    int i = 0;
    int total = m_size * 8;
    int cnt = total / 7;
    m_field0c = total % 7;
    for (i = 0; i <= cnt; ++i)
    {
        if ((m_field0c == 0) && (i == cnt))
        {
            break;                                            // ORIG 循环条件排除该次迭代
        }
        unsigned char bits = 0;                               // ORIG `movb $0,-0x9`
        if ((i == 0) && (m_field0c != 0))
        {
            bits = GetBitFromLSB(m_field0c);
        }
        else
        {
            bits = GetBitFromLSB(7);
        }
        if (i > 0)
        {
            ShiftLeft(7, m_buffer);
        }
        m_buffer[m_size - 1] |= bits;
    }
    return 0;
}

#include "WindowsPackFiles/Obfuscate2.inc"
{
    // ORIG 槽序（P28）：idx(-0x1c 函数首 movl$0 预初始化，for 再赋 m_size-1)
    // → outIdx(-0x18) → tail(-0x14) → total8(-0x10 dead `shl$3;mov slot`) →
    // 复用字节槽(-0x9)：hi 装载/移位/lo 装载/移位全走同一 uchar 槽（4.1 单槽
    // 复用形，源码=同变量复合赋值 `hi <<= n; hi = ...; hi >>= n;`）。
    // 变量移位 ORIG 无 and $0x1f 掩码（4.1 直发 shl/sar %cl），去掩码。
    double framePad;                     // P31：ORIG 帧 0x34（4.1 esp 16B 对齐
                                         // 进位）；double 触发 4.4 帧 +4 且 int
                                         // 槽基整体下移，idx 落回 -0x1c
    (void)&framePad;
    int idx = 0;
    int outIdx = 0;
    int tail = 6;
    int total8 = m_size * 8;
    (void)total8;
    for (idx = m_size - 1; idx >= 0; --idx)
    {
        unsigned char hi = GetBit(idx, 0, tail);
        hi <<= (7 - tail);
        m_buffer[m_size - 1] = hi;
        hi = GetBit(idx, tail + 1, 7);
        hi >>= (tail + 1);
        m_buffer[m_size - 1] |= hi;
        m_buffer[outIdx] = m_buffer[m_size - 1];
        ++outIdx;
        if (tail == 0)
        {
            tail = 6;
        }
        else
        {
            --tail;
        }
    }
    return 0;
}

#include "WindowsPackFiles/Deobfuscate2.inc"
{
    // 与 Obfuscate2 同构（同 double-pad 帧形 + 单 uchar 槽复合赋值链）；
    // tail 初值 = m_size%7，若 0 → 6 否则 --（ORIG +0x060 cmpl$0/jne 形）。
    double framePad;                     // P31 帧对齐 pad（同 Obfuscate2）
    (void)&framePad;
    int idx = 0;
    int outIdx = 0;
    int tail = 0;                          // ORIG 函数首 movl$0,-0x14 预初始化
    int total8 = m_size * 8;
    (void)total8;
    tail = m_size % 7;
    if (tail == 0)
    {
        tail = 6;
    }
    else
    {
        --tail;
    }
    for (idx = m_size - 1; idx >= 0; --idx)
    {
        unsigned char hi = GetBit(idx, 0, tail);
        hi <<= (7 - tail);
        m_buffer[m_size - 1] = hi;
        hi = GetBit(idx, tail + 1, 7);
        hi >>= (tail + 1);
        m_buffer[m_size - 1] |= hi;
        m_buffer[outIdx] = m_buffer[m_size - 1];
        ++outIdx;
        if (tail == 0)
        {
            tail = 6;
        }
        else
        {
            --tail;
        }
    }
    return 0;
}



// ===========================================================================
// WindowsDataFile —— 0x08574e3e..
// ===========================================================================
#include "WindowsPackFiles/WindowsDataFile.inc"
{
}                                                            // 0x08574e3e

#include "WindowsPackFiles/dtor_WindowsDataFile.inc"
{                                                            // 0x08574e56
    if (m_data != 0)
    {
        delete m_data;      // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样，同 LoadClientFile）
        m_data = 0;
    }
}

#include "WindowsPackFiles/LoadWindowsDataFile.inc"
{
    // ORIG 0x08574e80 尾部（+0x20..+0x37）为 `call LoadClientFile; xor $1,%eax;
    // test %al,%al; je L; mov $0,%eax; jmp end; L: mov $1,%eax; leave; ret` ——
    // bool 归一化走**取反短路物化 + 常量分支岛**（P32 xor$1 族）。裸
    // `return f(...)` 被 4.4 折成 call 后直接 ret（−6 insns），三目
    // `return r ? true : false` 只得 test/je 缺 xor（−2）；本形 20/20 逐条全等。
    if (!LoadClientFile(&m_size,
                        reinterpret_cast<void**>(&m_data), path))
    {
        return false;
    }
    return true;
}

#include "WindowsPackFiles/GenerateChecksum.inc"
                                       unsigned long offset,
                                       unsigned long len)    // 0x08574eba
{
    if (m_size <= offset)
    {
        ZeroMemoryChecksum(ck);
        return;
    }
    if (m_size < offset + len)
    {
        len = m_size - offset;
    }
    CalculateMemoryChecksum(ck, len, m_data + offset);
}

// ===========================================================================
// WindowsExeFile —— 0x085736be..
// ===========================================================================
// [b28c-lane17] ORIG 0x085736be：先构造 m_sections(+8)/m_commonSections(+0x14)，
// 再体赋值 m_size=0/m_image=0（标量不进初始化列表）。
#include "WindowsPackFiles/WindowsExeFile.inc"
    : m_sections(), m_commonSections()
{
    m_size = 0;
    m_image = 0;
}

#include "WindowsPackFiles/dtor_WindowsExeFile.inc"
{                                                            // 0x0857371a
    if (m_image != 0)
    {
        delete m_image;      // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样，同 LoadClientFile）
        m_image = 0;
    }
    // ORIG 0x08573744..：迭代器形——独立 C1Ev（0x0857374e）+ begin 临时槽
    // （-0x1c）再拷入 it（-0x14）；每轮 end() 重取临时 + operator!=/operator*，
    // delete 后 *it=0，operator++ 回边；无 size/operator[] 索引链（P50 复活）。
    std::vector<WindowsExeConstantSection*>::iterator it;
    it = m_sections.begin();
    for (; it != m_sections.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_sections.clear();
    it = m_commonSections.begin();
    for (; it != m_commonSections.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_commonSections.clear();
}

// ReadPeHeader —— T @0x085738f6（0x7de）：解析 PE 头产出常量区段表。
// ORIG 真形修正（本批 objdump+Ghidra C 双段核读）：
// ① 分支极性与直觉相反——`chars>=0 && 0x40000000` 门内，**(sh[9]&0x20000000)==0**
//    走 dir/import 解剖大分支，可执行分支只减入口头 10 字节（0x08573a1c else 体）；
// ② 两分支各持独立 IntervalSet+push 迭代器循环（dc/c4 双对象，dtor 各 1）；
// ③ hint name 长度奇偶用 (char)(s+1)+(char)nameOff，且 nm!=0 外层判断；
// ④ dll 名指针 = img+dllRva（**无** vaDelta），奇偶第二项 = thunkRva+vaDelta
//    （local_38）——前批两处误读已修正。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N95）→ 试=槽集合比对：ORIG 38 槽 / OURS 39 槽；
//   帧 ORIG 0x100 / OURS 0x110；头部 0x085738f6 `mov 0x4(%eax),%eax` 之后我方正 4 槽一组
//   `mov %eax,-0x78; mov -0x78,%eax; mov %eax,-0x70; …` 槽往返链（4.4 对具名局部/间接层恒写栈，
//   ORIG 4.1 直留寄存器）⇒ 读数 551 vs 553（gap -2）无源码可择项 ⇒ 维持 N82/N95。
// [Q8 2026-09-11 形态轴 A/B（检测器∩triage）] `opt[(d+12)*2]` 的索引强度削减（ORIG 0x08573a0e
//   等 4 处 `mov 0x4(%eax,%edx,8),%eax` = 4.1 索引折半 scale-8）两次实验均不成：
//   ① `*reinterpret_cast<unsigned long*>((char*)opt + (d+12)*8 [+4])` ⇒ **反劣**（our 553→559、
//   gap -2→-8，每处多 3 条 add/lea/mov 链）；② `(d+12)<<1` ⇒ 读数不变（553=553）。
//   ⇒ 4.4 前端不可择该折叠形，维持 N82/N95（两实验均已在 overlay 回滚）。
#include "WindowsPackFiles/ReadPeHeader.inc"
{
    // [SweepC 2026-09-09 修复判词] 0x085738f6：P165 双份物理展开 + 头部具名局部已还原。
    // ① 可执行（减入口头 10B）与非可执行（dir/import 解剖）两分支各持独立
    //    `IntervalSet workSet; Interval base; AddInterval; Interval range;
    //    iterator 遍历 new/push_back`（分支顺序照 ORIG：!=0 在前，==0 在后）；
    // ② 遍历走迭代器且每迭代仅 1 次 operator*（`const Interval& iv = *it;`
    //    P183 引用绑定；ORIG 08573aaa/08573fbb 各 1 次）——补回 ORIG-only
    //    callee `_ZN8IntervalC1Ev` + `__normal_iterator::operator*`；
    // ③ 头部 img@-0x68 / p@-0x60 / pe@-0x5c / sig@-0x64 / fileHdr@-0x58 /
    //    opt@-0x54 具名局部中转；节数循环条件每次重读 fileHdr+2（ORIG 085740b7）。
    // 结果：p167 calls 44/44，MISSING/EXTRA 仅 C1↔C2、D1↔D2 别名对（豁免）；
    // gap 91 → -29（我方多 29 条 = 4.4 槽基/寻址形态差 N82/N95，非语义缺口）；
    // TU identical 106→111 / ae 239→242 / near 1 / diff 3（无回退）。
    // [M3S2WinDrop 2026-09-11] 本波落地 3 项（547→553，orig 551；LCS ratio 0.8015→0.8134）：
    //   ① 循环前死数组 `long skipDirs[2] = {2,6}`（ORIG 0x08573c88 `movl $0x2,-0x44` +
    //      `movl $0x6,-0x40`，其后比较仍用立即数 ⇒ 死 store）⇒ +2 条；
    //   ② 去具名 `bool hasImport` ⇒ 短路直接内联在 `if` 条件（ORIG 0x08573cfe `mov $1,%eax;
    //      jmp; mov $0,%eax; test; je` 寄存器物化，无 bool 槽）+ `int unusedFlag = 1;`
    //      （ORIG local_40 `movl $0x1,-0x3c`）⇒ 净 +4 条、形状与 ORIG 对齐（去 movb×2/cmpb）。
    // 残余 2 条（判 N）：ORIG 的 `opt[(d+12)*2]` 走 scale-8 寻址（`mov 0x4(%eax,%edx,8),%eax`，
    //   0x08573a0e 等 4 处）而 4.4 用 `add %eax,%eax; lea 0x1(%eax),%edx; mov (%eax,%edx,4)`
    //   链（+2 条/处，另有 `addl $0x28,S` 的游标形差），属索引强度削减差异。
    if (m_image == 0)
    {
        return false;
    }
    // [b28d-ReadPeHeader 修形②] PE 头常量偏移访问用结构视图（4.4.7 对
    // `*(T*)(base+K)` 恒 add+mov，对 `view->m_field` 折 disp，同 sh/opt 判据）。
    struct PeImageDosHeader
    {
        unsigned char m_pad[0x3c];
        unsigned long m_lfanew;                              // +0x3c e_lfanew
    };
    struct PeCoffFileHeader
    {
        unsigned short m_machine;                            // +0x00
        unsigned short m_numberOfSections;                   // +0x02
    };
    unsigned char* img = m_image;                            // -0x68
    // [N] 试把 `sig` 声明前移到 `p` 之前（P339 期望六槽 img/sig/p/pe/fileHdr/opt
    // 对齐）：读数不变（551/553），4.4 让 sig 与 p **共用槽**（-0x70）——活跃性
    // 槽复用，声明序无法控制；判 N。
    unsigned int sig;                                        // -0x64 PE 签名
    unsigned char* p = img;                                  // -0x60 节表游标
    PeImageDosHeader* pe = reinterpret_cast<PeImageDosHeader*>(p); // -0x5c
    p = img + pe->m_lfanew;                                  // e_lfanew
    sig = *reinterpret_cast<unsigned int*>(p);
    (void)sig;
    p += 4;
    PeCoffFileHeader* fileHdr = reinterpret_cast<PeCoffFileHeader*>(p); // -0x58 COFF 头
    p += 0x14;
    // [b28d-ReadPeHeader 修形] 常量下标必须走「数组引用」：4.4.7 对 `unsigned long*`
    // 的 `p[K]` 恒发 `mov p,%eax; add $4K,%eax; mov (%eax)`（+1 条/处），ORIG 4.1.2
    // 为 `mov p,%eax; mov 4K(%eax)`（disp 折叠）。探针 /tmp/shp2.cpp g1/g2(数组引用/
    // 结构视图→disp) vs g3(指针下标→add) 实证；本函数 20+ 处常量访问即 gap -29 主体。
    unsigned long (&opt)[56] = *reinterpret_cast<unsigned long (*)[56]>(p); // -0x54 可选头
    p += 0xe0;

    for (int s = 0;
         s < static_cast<int>(fileHdr->m_numberOfSections);
         ++s, p += 0x28)
    {
        unsigned long (&sh)[10] = *reinterpret_cast<unsigned long (*)[10]>(p);
        unsigned long vaDelta = sh[5] - sh[3];
        if (static_cast<long>(sh[9]) < 0 || (sh[9] & 0x40000000UL) == 0)
        {
            continue;
        }
        unsigned long rawLen;
        if (sh[2] < sh[4])
        {
            rawLen = sh[2];
        }
        else
        {
            rawLen = sh[4];
        }
        if ((sh[9] & 0x20000000UL) != 0)               // 可执行：仅减入口头 10B
        {
            IntervalSet workSet;
            Interval base(sh[3], rawLen);
            workSet.AddInterval(base);
            Interval entryHead(opt[4], 10);             // AddressOfEntryPoint
            workSet.MinusInterval(entryHead);
            std::vector<Interval>::iterator it;
            Interval range;
            for (it = workSet.m_intervals.begin();
                 it != workSet.m_intervals.end(); ++it)
            {
                // [b28d-ReadPeHeader 修形③] ORIG 08573aaa 一次 operator* + 两读两写
                // （`mov 0x4(%eax),%edx; mov (%eax),%eax` 后双存）= 结构整体赋值；
                // 写 `const Interval& iv = *it;` 会引入引用槽（我方 -0x58 多 4 条）。
                range = *it;
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->fileOffset = range.m_start + vaDelta;
                item->size       = range.m_size;
                item->flag       = sh[9];
                m_sections.push_back(item);
            }
        }
        else                                            // 非可执行 → 解剖大分支
        {
            IntervalSet workSet;
            Interval base(sh[3], rawLen);
            workSet.AddInterval(base);
                // [M3S2WinDrop] ORIG 0x08573c88 循环前死 store `movl $0x2,-0x44(%ebp)` +
                // `movl $0x6,-0x40(%ebp)`（其后比较仍用立即数 ⇒ 死数组；P171 照抄）。
                long skipDirs[2] = {2, 6};
                (void)skipDirs;
                for (long d = 0; d < static_cast<long>(opt[0x17]); ++d)
                {
                    if (IsOverlappedInterval(
                            sh[3], sh[2],
                            opt[(d + 12) * 2], opt[(d + 12) * 2 + 1]) != 0 &&
                        d != 2 && d != 6)
                    {
                        Interval dir(opt[(d + 12) * 2], opt[(d + 12) * 2 + 1]);
                        workSet.MinusInterval(dir);
                    }
                }
                // [M3S2WinDrop] ORIG 0x08573cfe-0x08573d02：短路**直接内联在 if 条件**
                // （`mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al; je <far>` 寄存器
                // 物化，无具名 bool 槽）；具名 bool 会生成 `movb $0/movb $1/cmpb` 三槽操作。
                int unusedFlag = 1;                          // ORIG local_40 `movl $0x1,-0x3c`
                (void)unusedFlag;
                if (opt[0x17] >= 2 &&
                    IsOverlappedInterval(sh[3], sh[2], opt[0x1a], opt[0x1b]) != 0)
                {
                    unsigned char* desc = img + opt[0x1a];
                    for (int e = 0;
                         *reinterpret_cast<unsigned int*>(desc + e * 0x14) != 0 ||
                         *reinterpret_cast<unsigned int*>(desc + e * 0x14 + 0x10) != 0;
                         ++e)
                    {
                        unsigned long thunkRva =
                            *reinterpret_cast<unsigned long*>(desc + e * 0x14);
                        unsigned char* thunk = img + thunkRva + vaDelta;
                        int n = 0;
                        for (; *reinterpret_cast<unsigned int*>(thunk + n * 4) != 0; ++n)
                        {
                            long hintVal =
                                *reinterpret_cast<long*>(thunk + n * 4);
                            if (hintVal < 0)
                            {
                                // ordinal 导入：ORIG 取低 16 位入槽（local_2a），
                                // 不做减法
                                unsigned short ord =
                                    static_cast<unsigned short>(hintVal);
                                (void)ord;
                            }
                            else
                            {
                                unsigned long nameOff =
                                    static_cast<unsigned long>(hintVal) + vaDelta;
                                (void)*reinterpret_cast<unsigned short*>(
                                    img + nameOff);                     // hint 字
                                unsigned long total = 2;
                                char* nm = reinterpret_cast<char*>(img + nameOff + 2);
                                if (nm != 0)
                                {
                                    size_t sl = strlen(nm);
                                    unsigned long len = static_cast<unsigned long>(sl) + 1;
                                    if ((static_cast<char>(len) +
                                         static_cast<char>(nameOff) & 1) != 0)
                                    {
                                        len = static_cast<unsigned long>(sl) + 2;
                                    }
                                    total = 2 + len;
                                    Interval name(hintVal, total);
                                    workSet.MinusInterval(name);
                                }
                            }
                        }
                        unsigned long dllRva =
                            *reinterpret_cast<unsigned long*>(desc + e * 0x14 + 0x0c);
                        if (dllRva != 0)
                        {
                            unsigned long dl = 0;
                            char* dn = reinterpret_cast<char*>(img + dllRva);
                            size_t sdl = strlen(dn);
                            dl = static_cast<unsigned long>(sdl) + 1;
                            if ((static_cast<char>(dl) +
                                 static_cast<char>(thunkRva + vaDelta) & 1) != 0)
                            {
                                dl = static_cast<unsigned long>(sdl) + 2;
                            }
                            Interval dll(dllRva, dl);
                            workSet.MinusInterval(dll);
                        }
                        Interval table(thunkRva,
                                       static_cast<unsigned long>((n + 1) * 4));
                        workSet.MinusInterval(table);
                    }
                }
            std::vector<Interval>::iterator it;
            Interval range;
            for (it = workSet.m_intervals.begin();
                 it != workSet.m_intervals.end(); ++it)
            {
                range = *it;                             // 同上：结构整体赋值（ORIG 08573fbb）
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->fileOffset = range.m_start + vaDelta;
                item->size       = range.m_size;
                item->flag       = sh[9];
                m_sections.push_back(item);
            }
        }
    }
    return true;
}

#include "WindowsPackFiles/Sort.inc"
{
    // [b28c-lane17] ORIG 0x08574171：`test %eax; sete %al; test %al; jne <nop 岛>`
    // + body 尾 fall-through 到该岛 ⇒ 负极判定 + goto 尾空标签（P223）。
    if (m_commonSections.size() == 0)
        goto L_empty;
    qsort(&m_commonSections[0], m_commonSections.size(),
          sizeof(WindowsExeConstantSection*),
          CompareWindowsExeConstantSectionsForSort);
L_empty:;
}

// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N95）→ 试=槽集合比对 + 帧/寄存器轴：
//   ORIG `push %esi; push %ebx; sub $0x60`（callee-saved 保 sec/range 中转）vs OURS
//   `sub $0x78`（无 push，全落栈槽）；两枚 Interval ORIG -0x20/-0x24 vs OURS -0x30/-0x34
//   （整体深 0x10）；余差 8 条全为 4.4 栈槽 vs 4.1 callee-saved 的取数/回写（0x085741c6 段）
//   ⇒ 维持 N82/N95（ours 156 / ORIG 164，gap +8；P331 声明序轴已试）。
#include "WindowsPackFiles/MakeCommonWindowsExeConstantSections.inc"
    const IntervalSet& intervals)                            // 0x085741c6
{
    // [b28d-solo-WindowsPackFiles] verdict=DIFF，N 封顶（N82/N95 槽号 + 4.4 C1Ev 展开差）：
    // p167 CLEAR 20/20（调用点多集全等，无发明/缺失调用）；fresh 150/164（我方少 14 条）。
    // 已复现：头部 4 枚构造序（iterator C1Ev×2 + Interval C1Ev×2，ORIG +0x8..+0x2f）、
    // 短路物化守卫 `call size; test; je <mov$1>; call sections.size; test; jne <mov$0>`、
    // 尾部 dead size 具名 store（P171）。残余 = 4.4 槽基恒 ebp-8 起致全帧槽号浅移 4
    // （N95）+ 4.4 对 Interval 死局部 C1Ev 的内联/中转差（ORIG 4.1 逐条发射），无语义缺口。
    // ORIG 0x085741c6 逐段：两枚 Interval 默认构造（local_48 + local_40 槽对）
    // → clear（在合法检查**前**）→ `intervals.size()==0 || sections.size()==0`
    // 的短路物化链（jne 落 mov$0 ⇒ 第二项为 ==0，勿写 !=）→ 双层迭代器循环；
    // src 四字段无条件拷入独立 uint 槽后再判；尾部一次 dead size() 调用。
    // [b28d-solo-WindowsPackFiles] ORIG 头部 4 枚构造（+0x8..+0x2f）：iterator C1Ev、
    // const_iterator C1Ev、Interval C1Ev ×2 ⇒ 两个 Interval 死局部（P232 帧律 +
    // Interval 有用户默认构造 W@0x08575e58）。守卫为短路物化形
    // `call size; test; je <mov$1>; call sections.size; test; jne <mov$0>` ⇒ 合并 ||。
    // [M3S2WinDrop 2026-09-11] 本波落地 3 项（150→155，gap 14→9；四数 112/242/near2/diff3 不变）：
    //   ① 两处迭代器改「先声明后赋值」（ORIG 0x08574243/0x0857427f begin() 经临时槽后
    //      `mov -0x4c,%eax; mov %eax,-0x24/-0x20`）+4 条 mov；`++` 移到体尾；
    //   ② 新增未初始化 `bool matched`（ORIG 0x08574337 独有 `movb $0x1,-0xd`，
    //      无初始化 store、从未被读）+1 条。
    //   残余 9 条 = 4.4 指令/寄存器选择，判 N：③ ORIG 匹配体用 `%ecx/%ebx/%esi`
    //      中转算 `s1+(start-s0)`（0x0857430c..0x1430 八条）vs 4.4 `sub mem,%edx;
    //      add mem,%edx`（2 条）；④ 因用 callee-saved ⇒ ORIG 有 push %esi/%ebx +
    //      epilogue `lea S(%esp); add $0x0,%esp; pop %ebx/%esi/%ebp`（6 条）vs 4.4 `leave`；
    //      ⑤ ORIG 四字段读取共用 `%eax=sec`、载入目标 `%edx`（0x085742ac），4.4 逐语句
    //      用 `%eax` 载入致 3 条 sec reload（试过逗号表达式单语句：读数不变，N）；
    //      ⑥ 首条件 ORIG `mov s0,%edx; cmp %eax,%edx; ja`（0x085742c6）vs 我方 `cmp mem; jb`（等价）。
    // 极性判读（0x08574225）：sections.size()!=0 ⇒ jne +0x6a(mov $0=bad false)，
    // 即坏条件为 `intervals.size()==0 || sections.size()==0`（**==**，勿写 !=）。
    std::vector<WindowsExeConstantSection*>::iterator sit0;  // ORIG 头部两枚迭代器
    std::vector<Interval>::const_iterator it0;               // 默认构造 call
    Interval range0;
    Interval range1;
    (void)range0;
    (void)range1;
    // [M3S2WinDrop] ORIG 头内连续 4 个 uint 槽（-0x38/-0x34/-0x30/-0x2c，P339）⇒
    // src 四字段槽提到函数头（深→浅 = s0,s1,s2,s3；赋值仍在循环体内）。
    unsigned long s0, s1, s2, s3;
    // [M3S2WinDrop] ORIG -0xd 槽：匹配体尾 `movb $0x1,-0xd(%ebp)`（0x08574337，
    // 无初始化 store、从未被读）⇒ 未初始化 bool 局部 + 命中置位。
    bool matched;
    m_commonSections.clear();
    if (intervals.m_intervals.size() == 0 || m_sections.size() == 0)
    {
        return false;
    }
    // [M3S2WinDrop] ORIG 0x08574243/0x0857427f：两处迭代器均为**先声明后赋值**
    // （begin() 经临时槽 -0x4c 后 `mov -0x4c,%eax; mov %eax,-0x24/-0x20`，各 +2 条 mov），
    // 非 for 初始化式直构；`++` 位于循环体尾（ORIG +0x1b4/+0x177）。
    it0 = intervals.m_intervals.begin();
    while (it0 != intervals.m_intervals.end())
    {
        Interval range = *it0;                               // local_40/3c = start/size
        sit0 = m_sections.begin();
        while (sit0 != m_sections.end())
        {
            WindowsExeConstantSection* sec = *sit0;          // local_18
            s0 = sec->start;                                 // local_38
            s1 = sec->fileOffset;                            // local_34
            s2 = sec->size;                                  // local_30
            s3 = sec->flag;                                  // local_2c
            // [N] 逗号表达式（单语句连续赋值）试过：读数不变（154），4.4 仍逐条
            // reload sec 指针（`mov S(%ebp),%eax`），不采用 ORIG 的 %edx 载入形。
            if ((s0 <= (unsigned long)range.m_start) &&
                ((unsigned long)range.m_start < s0 + s2))
            {
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->size       = range.m_size;
                item->flag       = s3;
                item->fileOffset = s1 + ((unsigned long)range.m_start - s0);
                m_commonSections.push_back(item);
                matched = true;                              // ORIG movb $1,-0xd
                break;                                       // 首个匹配即止
            }
            ++sit0;
        }
        ++it0;
    }
    // ORIG 尾部 `call size; mov %eax,-0xc(%ebp)`（P171 死写入照抄：4.4 对裸
    // `m_commonSections.size();` 会 DCE，须经具名局部保留 store）。
    unsigned long deadSize = m_commonSections.size();
    (void)deadSize;
    return true;
}

#include "WindowsPackFiles/LoadWindowsExeFile.inc"
{
    if (!LoadClientFile(&m_size, reinterpret_cast<void**>(&m_image), path))
    {
        return false;
    }
    if (!ReadPeHeader())
    {
        delete m_image;      // ORIG _ZdlPv（同 ~WindowsExeFile 保留原版形态）
        m_image = 0;
        m_size = 0;
        return false;
    }
    Sort();                                                  // common 尚空——按 ORIG 形态保留调用
    return true;
}

#include "WindowsPackFiles/GenerateChecksum_1.inc"
                                      unsigned long offset, unsigned long len) // 0x085743d4
{
    // [b28d-58] P0：ORIG 无 total 槽（`cmp 0x10(%ebp),%eax; setle %al; test;
    // je`），条件内联 size()；失败返回 0、成功返回 1（见 .h 返回类型判词）。
    if (static_cast<long>(m_commonSections.size()) <= sectionIdx)
        return false;
    {
        // [b28d-58] P157/P183：ORIG 每字段重取 operator[]（两次 call），不缓存
        // 引用（ORIG 0x085743d4 段两次 `call vector::operator[]`）；求值序为
        // 先 `(offset - start)`（第一次取 start）再 fileOffset（第二次）——
        // 中间值各占一槽（ORIG -0x10/-0xc/-0x14）⇒ 具名局部形。
        unsigned char* addr;
        unsigned long d;
        unsigned long sum;
        d = offset - m_commonSections[sectionIdx]->start;
        sum = d + m_commonSections[sectionIdx]->fileOffset;
        addr = m_image + sum;
        CalculateMemoryChecksum(ck, len, addr);
    }
    return true;
}

// ===========================================================================
// WindowsDataFileFamily —— 0x08574f16..
// ===========================================================================
#include "WindowsPackFiles/WindowsDataFileFamily.inc"
    : m_files()
{
    memset(m_name, 0, sizeof(m_name));   // ORIG memset(m_name) 先于四标量清零(全于 body)
    m_scanOffset = 0;
    m_maxSize = 0;
    m_randMin = 0;
    m_randMax = 0;
}                                                            // 0x08574f16

#include "WindowsPackFiles/dtor_WindowsDataFileFamily.inc"
{                                                            // 0x08574f7c
    // [b28d-64] P50 复活：ORIG 0x08574f8a 独立 iterator::C1Ev + begin() 临时槽
    // （-0x1c）再拷入 it（-0x10）⇒ 先声明后赋值；索引 for 会少默认构造与临时槽。
    std::vector<WindowsDataFile*>::iterator it;
    it = m_files.begin();
    for (; it != m_files.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_files.clear();
}

#include "WindowsPackFiles/IsDifferentFileData.inc"
{
    // b28c-L19：ORIG 0x08575068 迭代器先默认构造（C1Ev @0x8576bb6）再经 begin()
    // 临时槽（-0x1c）拷贝赋值到 it（-0x14）——"先声明后赋值"形（P28/P50 反例，
    // ORIG 确有 C1Ev 且 begin()/de() 均为 const 版 `_ZNK`）；memcmp 长度照抄 f->m_size。
    std::vector<WindowsDataFile*>::iterator it;
    it = m_files.begin();
    for (; it != m_files.end(); ++it)
    {
        WindowsDataFile* cur = *it;
        if (cur->m_size == f->m_size &&
            memcmp(cur->m_data, f->m_data, f->m_size) == 0)
        {
            return false;                                    // 相同内容 → 非新文件
        }
    }
    return true;
}

#include "WindowsPackFiles/SetCheckingOption.inc"
                                              unsigned long maxOff) // 0x0857511e
{
    m_randMin = minOff;
    m_randMax = maxOff;
}

#include "WindowsPackFiles/LoadWindowsDataFile_1.inc"
                                                const char* name)   // 0x0857513c
{
    WindowsDataFile* f = new WindowsDataFile;
    if (m_files.empty())
    {
        StrCpyMaxPath(m_name, name);
    }
    if (!f->LoadWindowsDataFile(path))
    {
        delete f;
        return false;
    }
    // b28c-L19：ORIG 0x0857513c 重复内容分支为早返（delete 块落在 push 之前，
    // 尾部 return 1 共享），if/else 形会被 4.4 把 delete 挪到末尾 → DIFF。
    if (!IsDifferentFileData(f))
    {
        delete f;                                            // 内容重复仍返回 1
        return true;
    }
    if (m_maxSize < f->m_size)
    {
        m_maxSize = f->m_size;
    }
    m_files.push_back(f);
    return true;
}

#include "WindowsPackFiles/GenerateRange.inc"
                                          unsigned long* outLen)   // 0x08575230
{
    // ORIG 槽向 span@-0x10 / clipped@-0x9；4.4 先声明占深槽（N93 标量版，
    // 同 ncl_check_digit_str 复现），span 未初始化前置不改指令序。
    unsigned long span;
    bool clipped = false;   // ORIG prologue 后即预清 -0x9（P198①：声明/预清在 IsEmpty 前）
    if (m_files.empty())
    {
        return false;
    }
    span =
        static_cast<unsigned long>(m_randMin) +
        static_cast<unsigned long>(rand()) %
            (static_cast<unsigned long>(m_randMax) + 1U);

    // ORIG: clip+置位同在单一 if 块（ja 直跳输出区）；拆两 if 会被 GCC 合并出错序中间测试。
    if (m_maxSize <= m_scanOffset + span)
    {
        span = m_maxSize - m_scanOffset;
        clipped = true;
    }
    *outStart = m_scanOffset;
    *outLen = span;
    if (clipped)
    {
        m_scanOffset = 0;                                    // 抵尾复位游标
    }
    else
    {
        m_scanOffset += span;
    }
    return true;
}

#include "WindowsPackFiles/GenerateChecksum_2.inc"
                                             CodeHackCheckRangeEx* rangeEx) // 0x08575302
{
    // ORIG 0x08575302：GenerateRange 失败 → return 0（与 ExeFileFamily 恒 1 不同，
    // 原码两版行为各异，逐版照抄）；rangeEx 回写在成功块内。
    // [b28d-solo-WindowsPackFiles] 三处照 ORIG 改形：
    // ① 0x08575361 `call GenerateRange; test %al,%al; je <mov $0,%eax>`（无 xor）
    //    ⇒ 正向 `if (GenerateRange(...)) { … return true; } return false;`；
    // ② 循环是迭代器形（C1Ev@0x08576bb6 + begin 临时@-0x4c + end 临时@-0x10 +
    //    operator*@-0xc 具名 f + 前置 ++）——索引 for 缺 4 个模板调用点（P167）；
    // ③ rangeEx 两字段从 range 槽(-0x24/-0x20)重读回写（非直读 rangeStart/Len）。
    // [b28d-solo-WindowsPackFiles] ORIG 槽（深→浅）：sum@-0x44, it0@-0x1c,
    // rangeLen@-0x18, rangeStart@-0x14。4.4「先声明浅」⇒ it0 须声明在标量之后。
    unsigned long rangeStart;
    unsigned long rangeLen;
    std::vector<WindowsDataFile*>::iterator it0;
    CodeHackChecksum sum;
    ckEx->Clear();                                           // CodeHackChecksumData::Clear
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);
    if (GenerateRange(&rangeStart, &rangeLen))
    {
        CodeHackCheckRange range;
        range.m_start = static_cast<int>(rangeStart);
        range.m_size  = static_cast<int>(rangeLen);
        rangeEx->m_range = range;                            // P247：8 字节视图整体赋值
        it0 = m_files.begin();
        while (it0 != m_files.end())
        {
            WindowsDataFile* f = *it0;
            f->GenerateChecksum(&sum, rangeStart, rangeLen);
            ckEx->SetCodeHackCheckRange(range);
            ckEx->AddCodeHackChecksum(sum);
            ++it0;
        }
        return true;
    }
    return false;
}

#include "WindowsPackFiles/IsScanningAddressReset.inc"
{
    if (m_scanOffset == 0)
        return true;
    return false;
}

// ===========================================================================
// WindowsExeFileFamily —— 0x0857446a..
// ===========================================================================
#include "WindowsPackFiles/WindowsExeFileFamily.inc"
    : IntervalSet(),
      m_exeFiles()
{
    memset(m_name, 0, sizeof(m_name));
    m_curSectionIndex = 0;
    m_curOffset = 0;
    m_randMin = 0;
    m_randMax = 0;
}

#include "WindowsPackFiles/dtor_WindowsExeFileFamily.inc"
{
    // [b28d-64] 同 ~WindowsDataFileFamily：ORIG 0x0857450e iterator C1Ev +
    // begin 临时槽（-0x1c）⇒ 先声明后赋值。
    std::vector<WindowsExeFile*>::iterator it;
    it = m_exeFiles.begin();
    for (; it != m_exeFiles.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_exeFiles.clear();
}

#include "WindowsPackFiles/SetCheckingOption_1.inc"
                                             unsigned long maxOff)  // 0x08574620
{
    m_randMin = minOff;
    m_randMax = maxOff;
}

#include "WindowsPackFiles/LoadWindowsExeFile_1.inc"
                                              const char* name)     // 0x0857463e
{
    WindowsExeFile* exe = new WindowsExeFile;
    if (m_exeFiles.empty())
    {
        StrCpyMaxPath(m_name, name);
    }
    if (!exe->LoadWindowsExeFile(path))
    {
        delete exe;
        return false;
    }
    // ORIG 0x085746d5：`!IsDifferentFileData(exe)`（xor$1 取反）命中 → 先
    // delete 重复项并 return 1（重复内容视为成功），push 分支在其后。
    if (!IsDifferentFileData(exe))
    {
        delete exe;
        return true;
    }
    m_exeFiles.push_back(exe);
    return true;
}

#include "WindowsPackFiles/IsDifferentFileData_1.inc"
{
    // [b28c-L29] ORIG 0x08574ace：迭代器先默认构造（C1Ev 调用 @0x85766a6）再经
    // begin() 临时槽（-0x1c）拷贝赋值到 it（-0x14）——"先声明后赋值"形（P50 反例，
    // ORIG 确有 C1Ev 符号）；每轮 end/operator!=/operator* 直调。
    std::vector<WindowsExeFile*>::iterator it;
    it = m_exeFiles.begin();
    for (; it != m_exeFiles.end(); ++it)
    {
        WindowsExeFile* cur = *it;
        if (cur->m_size == f->m_size &&
            memcmp(cur->m_image, f->m_image, f->m_size) == 0)
        {
            return false;
        }
    }
    return true;
}

// swap_value<IntervalSet*> —— ORIG W 0x0857688f 全局模板（OnlyOrigBatch12.cpp
// force 发射同定义；本 TU 隐式实例化再发同名 W，链接合并）。
template <class T>
#include "WindowsPackFiles/swap_value.inc"
{
    T tmp = a;
    a = b;
    b = tmp;
}

#include "WindowsPackFiles/CompleteLoadingWindowsExeFiles.inc"
{
    // [b28d-solo-WindowsPackFiles] verdict=DIFF，N 封顶（N95 槽号 + N89 模板实例）：
    // p167 CLEAR 37/37（调用点多集全等）；fresh 277/279（gap=2）。
    // 已复现：头部 3 枚独立 C1Ev（iterator<WindowsExeFile>@-0x3c、
    // iterator<WindowsExeConstantSection>@-0x40、vector<IntervalSet*>@-0x4c —— 迭代器
    // 先声明后赋值 P50）、sets[3] 倒计数构造循环、指针级 swap_value<IntervalSet*>、
    // 双层 while（每轮 end/operator!=/operator*/ppEv）、尾部 operator=+Sort+第二循环。
    // 残余 = 4.4 全帧槽号较 ORIG 浅 4（N95：4.4 槽基恒 ebp-8 起）与
    // vector<IntervalSet*>::C1Ev 的 out-of-line/inline 差（N89），无语义缺口。
    // ORIG 0x08574736 全形转写：sets[3] 逐元素构造 + 三个 IntervalSet* 槽做
    // **指针级** swap_value<IntervalSet*>（非对象 std::swap）；未使用的局部
    // std::vector<IntervalSet*>（ORIG local_50 ctor/dtor，0x08574757/尾部）；
    // 双层迭代器循环（begin 一次入槽 + 每轮 end/operator!=/operator*/++）。
    // [b28d-solo-WindowsPackFiles] ORIG 头部 3 枚独立 C1Ev（+0x8..+0x36）：
    // iterator<WindowsExeFile>@-0x3c、iterator<WindowsExeConstantSection>@-0x40、
    // vector<IntervalSet*>@-0x4c ⇒ 迭代器**先声明后赋值**（P50，for 内直构会消 C1Ev）。
    // [M3S2WinDrop 2026-09-11] 本波落地 2 项（277→279，gap 0；verdict DIFF→**NEAR**，
    // TU 四数 diff 4→3 / near 1→2，identical 112 / ae 242 不变）：
    //   ① 第二循环复用 it0（ORIG 0x085749a8 begin() 经 -0x8c 临时后 `mov -0x8c,%eax;
    //      mov %eax,-0x3c` = 先声明后赋值；for 初始化式会原地直构、少 2 条 mov）；
    //   ② 尾部两句交换为 `m_curOffset` 先、`m_curSectionIndex = 0` 后（ORIG 0x08574a3c
    //      `mov %edx,0x120(%eax)` 先于 `movl $0x0,0x11c(%eax)`）。
    // 余差仅槽基/寄存器（LCS 槽号归一后 ratio=1.0000）。
    // vector<IntervalSet*>::C1Ev 为 ORIG-only out-of-line 模板实例（N89），保留声明。
    // [N] P331 声明序反转（unusedVec/sit0/it0）试过：读数不变（279/279）、严格差异行
    // 仍 115/279 ⇒ 4.4 槽分配不随声明序 ⇒ 回滚。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N89/N95）→ 试=①槽集合比对（ORIG 279=OURS 279，
    //   助记符全同仅操作数异）②prett/串审计：prettchk（Ndx!=UND 修正判据）0 缺口、p167 37/37
    //   ③声明序轴 P331 已试 ④残余为两组不一致平移：C1Ev 组 ORIG -0x3c/-0x40/-0x4c vs OURS
    //   -0x38/-0x3c/-0x48（浅 4）、对象基 ORIG -0x7c vs OURS -0x80（深 4）、bool ORIG -0x19 vs
    //   OURS -0x25（差 0xc）⇒ 4.4/4.1 槽分配器固有差（非单调平移）⇒ 维持 NEAR（gap 0）。
    std::vector<WindowsExeFile*>::iterator it0;
    std::vector<WindowsExeConstantSection*>::iterator sit0;
    std::vector<IntervalSet*> unusedVec;
    IntervalSet sets[3];
    IntervalSet* pCur = &sets[0];
    IntervalSet* pPrev = &sets[1];
    IntervalSet* pInter = &sets[2];
    bool first = true;

    it0 = m_exeFiles.begin();
    while (it0 != m_exeFiles.end())
    {
        WindowsExeFile* exe = *it0;
        pPrev->Clear();
        sit0 = exe->m_sections.begin();
        while (sit0 != exe->m_sections.end())
        {
            WindowsExeConstantSection* sec = *sit0;
            Interval iv(sec->start, sec->size);
            pPrev->AddInterval(iv);
            ++sit0;
        }
        if (first)
        {
            swap_value(pCur, pPrev);
            first = false;
        }
        else
        {
            IntersectIntervalSet(pInter, *pCur, *pPrev);
            if (pInter->m_intervals.size() == 0)
            {
                return false;
            }
            swap_value(pCur, pInter);
        }
        ++it0;
    }

    if (pCur->m_intervals.size() == 0)
    {
        return false;
    }
    this->IntervalSet::operator=(*pCur);                     // IntervalSet::operator=
    Sort();
    // [M3S2WinDrop] ORIG 0x085749a8 第二循环**复用 it0 槽(-0x3c)**：begin() 经临时槽
    // (local_8c) 后 `mov -0x8c,%eax; mov %eax,-0x3c`（先声明后赋值形，+2 条 mov），
    // 非 for 初始化式直构（4.4 会原地构造、0 拷贝）。
    it0 = m_exeFiles.begin();
    while (it0 != m_exeFiles.end())
    {
        WindowsExeFile* exe = *it0;
        exe->MakeCommonWindowsExeConstantSections(*this);
        ++it0;
    }
    // [M3S2WinDrop] ORIG 0x08574a3c 顺序直证：先 `mov %edx,0x120(%eax)`（m_curOffset）
    // 后 `mov 0x8(%ebp),%eax; movl $0x0,0x11c(%eax)`（m_curSectionIndex=0）。
    m_curOffset = m_intervals[0].m_start;                    // operator[](this,0) 首字
    m_curSectionIndex = 0;                                   // this+0x11c
    return true;
}

#include "WindowsPackFiles/GenerateRange_1.inc"
                                         unsigned long* outStart,
                                         unsigned long* outLen)     // 0x08574b8e
{
    // [b28d-solo-WindowsPackFiles] ORIG 槽布局（0x08574b95 起）：clipped@-0x9
    // （prologue 后立即 movb $0x0 预清）、section@-0x10、span@-0x14。4.4 探针实测：
    // 声明序 `span; const Interval* section; bool clipped=false;` + 指针赋值形
    // 精确复现该三槽（4 形探针 /tmp/wpf_probe/p.cpp genD，其余三形 -0x11/-0xd）。
    // 条件本体 ORIG 为 `cmp %eax,%edx; ja +0xae` 直跳（非 setbe 物化）⇒ if 直判。
    unsigned long span;
    const Interval* section;
    bool clipped = false;
    if (m_intervals.empty())
    {
        return false;
    }
    span =
        static_cast<unsigned long>(m_randMin) +
        static_cast<unsigned long>(rand()) %
            (static_cast<unsigned long>(m_randMax) + 1U);

    section = &m_intervals[m_curSectionIndex];
    if (static_cast<unsigned long>(section->m_start + section->m_size) <=
        m_curOffset + span)
    {
        span = static_cast<unsigned long>(section->m_start + section->m_size) -
               m_curOffset;
        clipped = true;
    }
    *outSecIdx = m_curSectionIndex;
    *outStart = m_curOffset;
    *outLen = span;
    if (clipped)
    {
        m_curSectionIndex += 1;
        // [b28d-solo-WindowsPackFiles] ORIG 0x08574c88 比较方向为
        // `call size; mov idx,%edx; cmp %edx,%eax; setbe`（size() <= idx，
        // call 后重读成员）；写 `idx >= size()` 会引 %ebx 跨 call 缓存 + setae。
        if (m_intervals.size() <= static_cast<unsigned long>(m_curSectionIndex))
        {
            m_curSectionIndex = 0;
        }
        m_curOffset = m_intervals[m_curSectionIndex].m_start;
    }
    else
    {
        m_curOffset += span;
    }
    return true;
}

#include "WindowsPackFiles/GenerateChecksum_3.inc"
                                            CodeHackCheckRangeEx* rangeEx) // 0x08574cf0
{
    // ORIG 0x08574cf0 真形（Ghidra C 全文核读）：**恒 return 1**——GenerateRange
    // 失败只是跳过聚合块，无 false 出口（前批 return false 系误转真语义 bug）；
    // rangeEx 两字段回写位于成功块内；循环为迭代器形；sum 对象在函数头部声明。
    // [b28d-solo-WindowsPackFiles] ORIG 槽（深→浅）：sum@-0x48, it0@-0x20,
    // rangeLen@-0x1c, rangeStart@-0x18, secIdx@-0x14（4.4「先声明浅」）。
    long secIdx;
    unsigned long rangeStart;
    unsigned long rangeLen;
    std::vector<WindowsExeFile*>::iterator it0;
    CodeHackChecksum sum;
    ckEx->Clear();
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);
    // [b28d-solo-WindowsPackFiles] ORIG 0x08574cf0：① `test %al,%al; je <mov $1>`
    // （无 xor，且失败与成功共用尾块 ⇒ if 正向包裹 + 恒 return true）；
    // ② 循环为迭代器形（C1Ev + begin 临时@-0x4c + operator*@-0xc 具名 f +
    //    前置 ++ + 每次 end()）；③ rangeEx 两字段从 range 槽(-0x28/-0x24)重读回写。
    if (GenerateRange(&secIdx, &rangeStart, &rangeLen))
    {
        CodeHackCheckRange range;
        range.m_start = static_cast<int>(rangeStart);
        range.m_size  = static_cast<int>(rangeLen);
        rangeEx->m_range = range;                            // P247：8 字节视图整体赋值
        it0 = m_exeFiles.begin();
        while (it0 != m_exeFiles.end())
        {
            WindowsExeFile* f = *it0;
            f->GenerateChecksum(&sum, secIdx, rangeStart, rangeLen);
            ckEx->SetCodeHackCheckRange(range);
            ckEx->AddCodeHackChecksum(sum);
            ++it0;
        }
    }
    return true;
}

// ===========================================================================
// WindowsCodeProtectorServer —— 0x08575462..
// ===========================================================================
#include "WindowsPackFiles/WindowsCodeProtectorServer.inc"
    : m_dataFamilies()
{
    Initialize();                                            // ORIG 0x08575462：仅 vector 构造 + Initialize()
}                                                            // （m_exeMin..m_dataMax 由 SetExeCheckingOption 设置，构造不初始化）

#include "WindowsPackFiles/dtor_WindowsCodeProtectorServer.inc"
{                                                            // 0x08575484 → Destroy + ~vector
    Destroy();
}

#include "WindowsPackFiles/Initialize.inc"
{
    m_exeFamily = 0;
    m_cfgLong = 0;
    m_retryCount = 0;
    m_loaded = 0;
    m_curDataFamily = 0;
}

#include "WindowsPackFiles/Destroy.inc"
{
    if (m_exeFamily != 0)
    {
        delete m_exeFamily;
        m_exeFamily = 0;
    }
    // ORIG 0x08575500：独立 iterator::C1Ev（-0x10）+ begin() 临时（-0x1c）后
    // 拷贝赋值 ⇒ P50 复活「先声明后赋值」形；while 条件（end/ne）在循环尾。
    std::vector<WindowsDataFileFamily*>::iterator it;
    it = m_dataFamilies.begin();
    while (it != m_dataFamilies.end())
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
        ++it;
    }
    m_dataFamilies.clear();
}

#include "WindowsPackFiles/FindExeFamilyFile.inc"
{
    if (m_exeFamily != 0 && strcmp(m_exeFamily->m_name, name) == 0)
    {
        return true;
    }
    return false;
}

WindowsDataFileFamily*
#include "WindowsPackFiles/FindDataFamilyFile.inc"
{
    // [b28d-58] P50 复活：ORIG 0x0857562e 先默认构造 iterator（C1Ev @0x08576da0）
    // + begin() 临时槽（-0x1c）再赋值 `it = begin()`；初始化式形被 4.4 折叠。
    std::vector<WindowsDataFileFamily*>::iterator it;
    it = m_dataFamilies.begin();
    for (; it != m_dataFamilies.end(); ++it)
    {
        WindowsDataFileFamily* f = *it;
        if (strcmp(f->m_name, name) == 0)
        {
            return f;
        }
    }
    return 0;
}

#include "WindowsPackFiles/SetExeCheckingOption.inc"
                                                      long e2, long d1,
                                                      long d2)      // 0x085756ce
{
    // [N76 封顶] 除首字段外各 store 4.1 恒 RHS(param→edx) 先于 LHS(this 重载)，
    // 4.4 恒 LHS(this→eax) 先（probe 全试不可翻）；首字段两侧同序（eax 已持有）。
    m_cfgLong = cfg;
    m_exeMin = e1;
    m_exeMax = e2;
    m_dataMin = d1;
    m_dataMax = d2;
}

#include "WindowsPackFiles/BeginLoading.inc"
{
    m_loaded = 0;
    return true;
}

#include "WindowsPackFiles/LoadWindowsFile.inc"
                                                 const char* name) // 0x08575712
{
    // [b28d-solo-WindowsPackFiles] ORIG 0x08575729 `test %eax,%eax; je +0x120`：
    // **exe 域物理在前（fall-through）、data 域在后（je 目标）** ⇒ 源码为
    // `if (strstr(...) != 0) { exe } else { data }`；且**全函数只有一枚
    // `mov $0x0,%eax`（+0x20c）**，所有失败路径 `je +0x20c` 直达共享尾 ⇒ 源码
    // 必须写成「单 return false 出口」（if/else + else-if 链），不能每支 return false
    // （4.4 会就地复制 `mov $0; jmp` 块，旧版 173 vs 163 的 +10 即此）。
    if (strstr(path, ".exe.bak") != 0)                       // exe 文件域
    {
        if (m_exeFamily == 0)                                // 首个 .exe.bak
        {
            m_exeFamily = new WindowsExeFileFamily;
            if (m_exeFamily->LoadWindowsExeFile(path, name))
            {
                m_exeFamily->SetCheckingOption(
                    static_cast<unsigned long>(m_exeMin),
                    static_cast<unsigned long>(m_exeMax));
                return true;
            }
        }
        else
        {
            // ORIG 两守卫各自 `test %al,%al; je <共享 mov $0>`（无 && 物化岛）⇒
            // P133/P225：拆两条嵌套 if，`a && b` 会被 4.4 折成 mov$1/mov$0 物化。
            if (FindExeFamilyFile(name))
            {
                if (m_exeFamily->LoadWindowsExeFile(path, name))
                {
                    m_exeFamily->SetCheckingOption(
                        static_cast<unsigned long>(m_exeMin),
                        static_cast<unsigned long>(m_exeMax));
                    return true;
                }
            }
        }
    }
    else
    {
        // data 文件域（ORIG +0x120 起）
        WindowsDataFileFamily* family = FindDataFamilyFile(name);
        if (family == 0)
        {
            family = new WindowsDataFileFamily;
            if (family->LoadWindowsDataFile(path, name))
            {
                family->SetCheckingOption(
                    static_cast<unsigned long>(m_dataMin),
                    static_cast<unsigned long>(m_dataMax));
                m_dataFamilies.push_back(family);
                return true;
            }
            delete family;
        }
        else if (family->LoadWindowsDataFile(path, name))
        {
            return true;
        }
    }
    return false;
}

#include "WindowsPackFiles/CompleteLoading.inc"
{
    // ORIG 双份 `movb $1,0x1c` 常量岛（+0x23/+0x38）= 4.1 对两条返回路径各自
    // 展开赋值；内层为正向 `if (call())`（+0x1c test%al; je 无 xor）。
    if (m_exeFamily)
    {
        if (m_exeFamily->CompleteLoadingWindowsExeFiles())
        {
            m_loaded = 1;
            return true;
        }
        return false;
    }
    m_loaded = 1;
    return true;
}

#include "WindowsPackFiles/IsDifferentExeVersionRoughly.inc"
{
    if (m_exeFamily != 0)
    {
        // [b28d-64] P50 复活：ORIG 0x0857598a 独立 `iterator::C1Ev` 默认构造 +
        // begin() 临时槽（-0x2c）再拷入 it（-0x1c）⇒ 先声明后赋值。
        std::vector<WindowsExeFile*>::iterator it;
        WindowsExeFile* f;
        unsigned long maxFileSize = 0;
        it = m_exeFamily->m_exeFiles.begin();
        for (; it != m_exeFamily->m_exeFiles.end(); ++it)
        {
            // [b28d-64] P0：ORIG 0x085759c1 具名 f（-0x14 槽往返）且每轮两读；
            // 比较方向按 ORIG `cmp -0x10(%ebp),%eax; jbe`（m_size > maxFileSize）。
            f = *it;
            if (f->m_size > maxFileSize)
            {
                maxFileSize = f->m_size;
            }
        }
        // [b28d-64] P47/P191：ORIG 0x08575a1c `ja <尾 mov $0>` + fall `mov $1` ⇒
        // if(cond) return false; return true;（原三目/直返产 setbe 物化 +4 条）。
        unsigned long total = m_exeFamily->GetTotalIntervalSize();
        if (total * 2 <= maxFileSize)
        {
            return true;
        }
    }
    return false;
}

#include "WindowsPackFiles/GenerateChecksum_4.inc"
                                                  CodeHackCheckRangeEx* rangeEx) // 0x08575a3c
{
    // [b28d-solo-WindowsPackFiles] 按 ORIG 0x08575a3c 逐段重写：
    // ① prologue 后立即预清两 bool（`movb $0,-0xa`/`-0x9`，在 m_loaded 检查之前）；
    // ② m_loaded==0 与函数尾共享 `mov $0x0,%eax`（+0x1df）⇒ 整体用
    //    `if (m_loaded != 0) { 主体 }` 包裹 + 单一 return false；
    // ③ 四路分支为**扁平 else-if**（ORIG 每支显式写 wantExe/wantData 两字节）；
    // ④ wantData 段 `call size; cmp idx; setle`（有符号）⇒ (int)size() <= idx。
    bool wantExe = false;
    bool wantData = false;
    if (m_loaded != 0)
    {
        if (m_exeFamily == 0 && m_dataFamilies.empty())
        {
            return false;
        }
        if (m_exeFamily != 0 && m_dataFamilies.empty())
        {
            wantExe = true;
            wantData = false;
        }
        else if (m_exeFamily == 0 && !m_dataFamilies.empty())
        {
            wantExe = false;
            wantData = true;
        }
        else if (m_retryCount < m_cfgLong)
        {
            wantExe = true;
            wantData = false;
            m_retryCount += 1;
        }
        else
        {
            wantExe = false;
            wantData = true;
            m_retryCount = 0;
        }

        // ORIG 0x08575b36 `cmpb $0,-0xa; je +0x13c` + `call; test %al; je +0x13c`
        // 两守卫同落点、无 mov$1/mov$0 物化岛 ⇒ 拆嵌套 if（P133/P225）。
        if (wantExe)
        {
            if (m_exeFamily->GenerateChecksum(ckEx, rangeEx))
            {
                rangeEx->SetFileName("");
                return true;
            }
        }
        if (wantData)
        {
            // ORIG：m_dataFamilies[m_curDataFamily] 各取一次（不缓存指针）
            if (m_dataFamilies[m_curDataFamily]->GenerateChecksum(ckEx, rangeEx))
            {
                if (m_dataFamilies[m_curDataFamily]->IsScanningAddressReset())
                {
                    m_curDataFamily += 1;
                    if (static_cast<int>(m_dataFamilies.size()) <=
                        m_curDataFamily)
                    {
                        m_curDataFamily = 0;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

// ===========================================================================
// CCompressedBlob —— objdump 0x0844de52..0x0844e0aa 逐指令转写
// [b28c-lane24] throw()（P216 static-local guard 边界）：ORIG put_compressed_blob_data
// @0x083fbd06 无 landing pad；W 体尺寸不变（-fno-enforce-eh-specs 无 unexpected guard）。
// [判据㉓ 符号绑定] ORIG 8 符号全 W（头内 inline 定义由使用点 TU 发射，DB_Login.cpp
//   get_compressed_blob_data / W8LT_DBTask.cpp put_compressed_blob_data 为使用点）；
//   我方唯一权威定义落本 TU ⇒ 定义处加 __attribute__((weak)) 对齐绑定（不改头，
//   mangled/尺寸/体零变化：nm 双侧 0xea/0x29/0x56/0xa5/0x18/0x30 逐值相等）。
// ===========================================================================
__attribute__((weak)) CCompressedBlob::CCompressedBlob() throw() : m_buf(0), m_size(0)
{
}                                                            // 0x0844de52

__attribute__((weak)) CCompressedBlob::~CCompressedBlob() throw()
{                                                            // 0x0844de6a
    if (m_buf != 0)
    {
        delete[] m_buf;
        m_buf = 0;
    }
}

__attribute__((weak)) void CCompressedBlob::resize(int needSize) // 0x0844e02a
{
    if (m_size < needSize)
    {
        if (m_buf != 0)
        {
            // ORIG 0x844e040 二次 test 系 4.1 jump-threading 产物（test1 失败直达
            // new 段），4.4 无线程化 + 尾 jmp/nop 岛，跨版本豁免。
            delete[] m_buf;
            m_buf = 0;
        }
        m_buf = new char[needSize];
        m_size = needSize;
    }
}

__attribute__((weak)) void CCompressedBlob::reset()          // 0x0844e080
{
    memset(m_buf, 0, m_size);
}

__attribute__((weak)) bool CCompressedBlob::compress(MySQL* db, int fieldIdx, char* src,
                                                     int srcLen,
                                                     char** outStr) // 0x0844de9a
{
    // ORIG 0x0844de9a：单一 unsigned int 槽（-0xc）既作 resize 入参又作
    // compress_zip 的 unsigned int& 出参（blob_to_str 读回同槽）——引入独立
    // outLen 变量会多占一槽并差 store/load。除法带符号展开 magic 0x66666667
    // 配 `sar $1` = **/5**（前批误读为 /10：/10 与 /5 共用 magic、以移位区分，
    // ORIG sar1 vs ours sar2 即常数不同之证；改 /5 后 4.4 展开与 ORIG 同形）。
    unsigned int needSize = srcLen + srcLen / 5 + 12;
    resize(needSize);
    reset();
    if (!compress_zip(m_buf, needSize, src, static_cast<unsigned int>(srcLen)))
    {
        return false;
    }
    *outStr = db->blob_to_str(fieldIdx, m_buf, static_cast<int>(needSize));
    return true;
}

__attribute__((weak)) bool CCompressedBlob::uncompress(MySQL* db, int fieldIdx, char* dst,
                                                       int dstLen,
                                                       int align) // 0x0844df40
{
    // ORIG 0x0844df40：needSize 单槽（-0x10）复用 uint& 出参；binaryLen 独立
    // 槽 -0xc；尾部 align 检查 `align!=0 && needSize%align!=0 && needSize!=0`。
    unsigned int needSize = dstLen + dstLen / 5 + 12;
    resize(needSize);
    reset();

    int binaryLen = db->get_binary_length(fieldIdx);
    if (!db->get_binary(fieldIdx, m_buf, needSize))
    {
        return false;
    }
    if (!uncompress_zip(dst, needSize, m_buf,
                        static_cast<unsigned int>(binaryLen)))
    {
        return false;
    }
    if (align != 0 && (needSize % static_cast<unsigned int>(align)) != 0 &&
        needSize != 0)
    {
        return false;
    }
    return true;
}
