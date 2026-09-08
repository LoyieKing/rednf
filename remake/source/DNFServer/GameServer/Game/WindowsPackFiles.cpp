// ============================================================================
// WindowsPackFiles.cpp —— WindowsExe/DataFile/PackFile 校验族实现
//
// 语义来源：docs/class_func_reports/{CObfuscator,WindowsDataFile,
//   WindowsDataFileFamily,WindowsExeFile,WindowsExeFileFamily,
//   WindowsCodeProtectorServer}/ 各 md 的 ## 反编译 C；CCompressedBlob 与
//   CompareWindowsExeConstantSectionsForSort/LoadClientFile 由 objdump 直接转写。
// CBitManager 的实现不在本文件：ORIG 属独立编译单元，已迁至 Game/CBitManager.cpp
// （见 CObfuscator::CObfuscator 上方注释的取证与实测）。本 TU 经 WindowsPackFiles.h
// 只取其声明。
// C++98 / 32 位 / O0；字段均为具名成员，无偏移宏。
// ============================================================================
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
bool LoadClientFile(unsigned long* outSize, void** outBuf, const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (fp == 0)
    {
        return false;
    }
    fseek(fp, 0, SEEK_END);
    *outSize = static_cast<unsigned long>(ftell(fp));
    fseek(fp, 0, SEEK_SET);
    unsigned char* buf = new unsigned char[*outSize];
    *outBuf = buf;
    std::size_t n = fread(*outBuf, *outSize, 1, fp);
    if (n != 1)
    {
        delete buf;          // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样）
        *outBuf = 0;
        fclose(fp);
        return false;
    }
    fclose(fp);
    return true;
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
CObfuscator::CObfuscator()                                   // 0x08585888
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

CObfuscator::~CObfuscator()                                  // 0x085858d6
{
}

int CObfuscator::ObfuscatePacket(unsigned char* data, int len, int type) // 0x085858ea
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

int CObfuscator::DeobfuscatePacket(unsigned char* data, int len, int type) // 0x08585938
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

int CObfuscator::Obfuscate1()                                // 0x08585986
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

int CObfuscator::Deobfuscate1()                              // 0x08585aaa
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

int CObfuscator::Obfuscate2()                                // 0x08585be4
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

int CObfuscator::Deobfuscate2()                              // 0x08585d2e
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
WindowsDataFile::WindowsDataFile() : m_size(0), m_data(0)
{
}                                                            // 0x08574e3e

WindowsDataFile::~WindowsDataFile()
{                                                            // 0x08574e56
    if (m_data != 0)
    {
        delete m_data;      // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样，同 LoadClientFile）
        m_data = 0;
    }
}

bool WindowsDataFile::LoadWindowsDataFile(const char* path)  // 0x08574e80
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

void WindowsDataFile::GenerateChecksum(CodeHackChecksum* ck,
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
WindowsExeFile::WindowsExeFile()
    : m_sections(), m_commonSections()
{
    m_size = 0;
    m_image = 0;
}

WindowsExeFile::~WindowsExeFile()
{                                                            // 0x0857371a
    if (m_image != 0)
    {
        delete m_image;      // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样，同 LoadClientFile）
        m_image = 0;
    }
    // ORIG 0x08573744..：迭代器形——begin 一次性入临时槽（-0x14），每轮 end()
    // 重取临时（-0x1c）+ operator!=/operator*（多次直调），delete 后 *it=0，
    // operator++ 回边；无 size/operator[] 索引链。
    for (std::vector<WindowsExeConstantSection*>::iterator it = m_sections.begin();
         it != m_sections.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_sections.clear();
    for (std::vector<WindowsExeConstantSection*>::iterator it = m_commonSections.begin();
         it != m_commonSections.end(); ++it)
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
bool WindowsExeFile::ReadPeHeader()
{
    if (m_image == 0)
    {
        return false;
    }
    unsigned char* img = m_image;
    unsigned char* pe = img + *reinterpret_cast<unsigned int*>(img + 0x3c);
    (void)*reinterpret_cast<unsigned int*>(pe);            // PE 签名读（local_68）
    unsigned short numberOfSections =
        *reinterpret_cast<unsigned short*>(pe + 6);        // COFF+2
    unsigned long* opt = reinterpret_cast<unsigned long*>(pe + 0x18);
    unsigned char* sechdr = pe + 0xf8;

    for (int s = 0; s < static_cast<int>(numberOfSections); ++s, sechdr += 0x28)
    {
        unsigned long* sh = reinterpret_cast<unsigned long*>(sechdr);
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
        IntervalSet workSet;
        Interval base(sh[3], rawLen);
        workSet.AddInterval(base);
        if ((sh[9] & 0x20000000UL) == 0)               // 非可执行 → 解剖大分支
        {
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
                bool hasImport = false;
                if (opt[0x17] >= 2)
                {
                    hasImport =
                        IsOverlappedInterval(sh[3], sh[2], opt[0x1a], opt[0x1b]) != 0;
                }
                if (hasImport)
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
            }
            else                                            // 可执行：仅减入口头 10B
            {
                Interval entryHead(opt[4], 10);             // AddressOfEntryPoint
                workSet.MinusInterval(entryHead);
            }
        for (std::size_t iv = 0; iv < workSet.m_intervals.size(); ++iv)
        {
            long rangeStart = workSet.m_intervals[iv].m_start;
            long rangeSize = workSet.m_intervals[iv].m_size;
            WindowsExeConstantSection* item = new WindowsExeConstantSection;
            item->start      = rangeStart;
            item->fileOffset = rangeStart + vaDelta;
            item->size       = rangeSize;
            item->flag       = sh[9];
            m_sections.push_back(item);
        }
    }
    return true;
}

void WindowsExeFile::Sort()                                  // 0x0857415c
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

bool WindowsExeFile::MakeCommonWindowsExeConstantSections(
    const IntervalSet& intervals)                            // 0x085741c6
{
    // ORIG 0x085741c6 逐段：两枚 Interval 默认构造（local_48 + local_40 槽对）
    // → clear（在合法检查**前**）→ `intervals.size()==0 || sections.size()!=0`
    // 的 bVar 双否链 → 双层迭代器循环（每轮 end/operator!=/operator*/++）；
    // src 四字段无条件拷入独立 uint 槽后再判；尾部一次 dead size() 调用。
    Interval range0;                                         // local_48 第二默认构造
    (void)range0;
    std::vector<WindowsExeConstantSection*>::iterator sit0;  // ORIG 头部两枚迭代器
    std::vector<Interval>::const_iterator it0;               // 默认构造 call
    m_commonSections.clear();
    if (intervals.m_intervals.size() == 0)
    {
        return false;
    }
    if (m_sections.size() == 0)
    {
        return false;
    }
    for (std::vector<Interval>::const_iterator it = intervals.m_intervals.begin();
         it != intervals.m_intervals.end(); ++it)
    {
        Interval range = *it;                                // local_40/3c = start/size
        for (std::vector<WindowsExeConstantSection*>::iterator sit = m_sections.begin();
             sit != m_sections.end(); ++sit)
        {
            WindowsExeConstantSection* sec = *sit;           // local_18
            unsigned long s0 = sec->start;                   // local_38
            unsigned long s1 = sec->fileOffset;              // local_34
            unsigned long s2 = sec->size;                    // local_30
            unsigned long s3 = sec->flag;                    // local_2c
            if ((s0 <= (unsigned long)range.m_start) &&
                ((unsigned long)range.m_start < s0 + s2))
            {
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->size       = range.m_size;
                item->flag       = s3;
                item->fileOffset = s1 + ((unsigned long)range.m_start - s0);
                m_commonSections.push_back(item);
                break;                                       // 首个匹配即止
            }
        }
    }
    m_commonSections.size();                                 // ORIG 尾部 dead size
    return true;
}

bool WindowsExeFile::LoadWindowsExeFile(const char* path)    // 0x085740de
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

bool WindowsExeFile::GenerateChecksum(CodeHackChecksum* ck, long sectionIdx,
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
WindowsDataFileFamily::WindowsDataFileFamily()
    : m_files()
{
    memset(m_name, 0, sizeof(m_name));   // ORIG memset(m_name) 先于四标量清零(全于 body)
    m_scanOffset = 0;
    m_maxSize = 0;
    m_randMin = 0;
    m_randMax = 0;
}                                                            // 0x08574f16

WindowsDataFileFamily::~WindowsDataFileFamily()
{                                                            // 0x08574f7c
    for (std::size_t i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i] != 0)
        {
            delete m_files[i];
            m_files[i] = 0;
        }
    }
    m_files.clear();
}

bool WindowsDataFileFamily::IsDifferentFileData(WindowsDataFile* f)    // 0x08575068（非 const）
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

void WindowsDataFileFamily::SetCheckingOption(unsigned long minOff,
                                              unsigned long maxOff) // 0x0857511e
{
    m_randMin = minOff;
    m_randMax = maxOff;
}

bool WindowsDataFileFamily::LoadWindowsDataFile(const char* path,
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

bool WindowsDataFileFamily::GenerateRange(unsigned long* outStart,
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

bool WindowsDataFileFamily::GenerateChecksum(CodeHackChecksumEx* ckEx,
                                             CodeHackCheckRangeEx* rangeEx) // 0x08575302
{
    // ORIG 0x08575302：GenerateRange 失败 → return 0（与 ExeFileFamily 恒 1 不同，
    // 原码两版行为各异，逐版照抄）；rangeEx 回写在成功块内。
    std::vector<WindowsDataFile*>::iterator it0;
    CodeHackChecksum sum;
    unsigned long rangeStart = 0;
    unsigned long rangeLen = 0;
    ckEx->Clear();                                           // CodeHackChecksumData::Clear
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);
    if (!GenerateRange(&rangeStart, &rangeLen))
    {
        return false;
    }
    {
        CodeHackCheckRange range;
        range.m_start = static_cast<int>(rangeStart);
        range.m_size  = static_cast<int>(rangeLen);
        rangeEx->m_start104 = static_cast<int>(rangeStart);
        rangeEx->m_size108 = static_cast<int>(rangeLen);
        for (std::size_t i = 0; i < m_files.size(); ++i)
        {
            m_files[i]->GenerateChecksum(&sum, rangeStart, rangeLen);
            ckEx->SetCodeHackCheckRange(range);
            ckEx->AddCodeHackChecksum(sum);
        }
    }
    return true;
}

bool WindowsDataFileFamily::IsScanningAddressReset()   // 0x08575444
{
    if (m_scanOffset == 0)
        return true;
    return false;
}

// ===========================================================================
// WindowsExeFileFamily —— 0x0857446a..
// ===========================================================================
WindowsExeFileFamily::WindowsExeFileFamily()
    : IntervalSet(),
      m_exeFiles()
{
    memset(m_name, 0, sizeof(m_name));
    m_curSectionIndex = 0;
    m_curOffset = 0;
    m_randMin = 0;
    m_randMax = 0;
}

WindowsExeFileFamily::~WindowsExeFileFamily()                // 0x08574500
{
    for (std::size_t i = 0; i < m_exeFiles.size(); ++i)
    {
        if (m_exeFiles[i] != 0)
        {
            delete m_exeFiles[i];
            m_exeFiles[i] = 0;
        }
    }
    m_exeFiles.clear();
}

void WindowsExeFileFamily::SetCheckingOption(unsigned long minOff,
                                             unsigned long maxOff)  // 0x08574620
{
    m_randMin = minOff;
    m_randMax = maxOff;
}

bool WindowsExeFileFamily::LoadWindowsExeFile(const char* path,
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

bool WindowsExeFileFamily::IsDifferentFileData(WindowsExeFile* f) // 0x08574ace
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
void swap_value(T& a, T& b)
{
    T tmp = a;
    a = b;
    b = tmp;
}

bool WindowsExeFileFamily::CompleteLoadingWindowsExeFiles()  // 0x08574736
{
    // ORIG 0x08574736 全形转写：sets[3] 逐元素构造 + 三个 IntervalSet* 槽做
    // **指针级** swap_value<IntervalSet*>（非对象 std::swap）；未使用的局部
    // std::vector<IntervalSet*>（ORIG local_50 ctor/dtor，0x08574757/尾部）；
    // 双层迭代器循环（begin 一次入槽 + 每轮 end/operator!=/operator*/++）。
    IntervalSet sets[3];
    IntervalSet* pCur = &sets[0];
    IntervalSet* pPrev = &sets[1];
    IntervalSet* pInter = &sets[2];
    bool first = true;
    std::vector<IntervalSet*> unusedVec;

    for (std::vector<WindowsExeFile*>::iterator it = m_exeFiles.begin();
         it != m_exeFiles.end(); ++it)
    {
        WindowsExeFile* exe = *it;
        pPrev->Clear();
        for (std::vector<WindowsExeConstantSection*>::iterator sit =
                 exe->m_sections.begin();
             sit != exe->m_sections.end(); ++sit)
        {
            WindowsExeConstantSection* sec = *sit;
            Interval iv(sec->start, sec->size);
            pPrev->AddInterval(iv);
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
    }

    if (pCur->m_intervals.size() == 0)
    {
        return false;
    }
    this->IntervalSet::operator=(*pCur);                     // IntervalSet::operator=
    Sort();
    for (std::vector<WindowsExeFile*>::iterator it = m_exeFiles.begin();
         it != m_exeFiles.end(); ++it)
    {
        WindowsExeFile* exe = *it;
        exe->MakeCommonWindowsExeConstantSections(*this);
    }
    m_curSectionIndex = 0;                                   // this+0x11c
    m_curOffset = m_intervals[0].m_start;                    // operator[](this,0) 首字
    return true;
}

bool WindowsExeFileFamily::GenerateRange(long* outSecIdx,
                                         unsigned long* outStart,
                                         unsigned long* outLen)     // 0x08574b8e
{
    if (m_intervals.empty())
    {
        return false;
    }
    unsigned long span =
        static_cast<unsigned long>(m_randMin) +
        static_cast<unsigned long>(rand()) %
            (static_cast<unsigned long>(m_randMax) + 1U);

    const Interval& section = m_intervals[m_curSectionIndex];
    bool clipped =
        static_cast<unsigned long>(section.m_start + section.m_size) <=
        m_curOffset + span;
    if (clipped)
    {
        span = static_cast<unsigned long>(section.m_start + section.m_size) -
               m_curOffset;
    }
    *outSecIdx = m_curSectionIndex;
    *outStart = m_curOffset;
    *outLen = span;
    if (clipped)
    {
        m_curSectionIndex += 1;
        if (static_cast<std::size_t>(m_curSectionIndex) >= m_intervals.size())
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

bool WindowsExeFileFamily::GenerateChecksum(CodeHackChecksumEx* ckEx,
                                            CodeHackCheckRangeEx* rangeEx) // 0x08574cf0
{
    // ORIG 0x08574cf0 真形（Ghidra C 全文核读）：**恒 return 1**——GenerateRange
    // 失败只是跳过聚合块，无 false 出口（前批 return false 系误转真语义 bug）；
    // rangeEx 两字段回写位于成功块内；循环为迭代器形；sum 对象在函数头部声明。
    std::vector<WindowsExeFile*>::iterator it0;
    CodeHackChecksum sum;
    long secIdx = 0;
    unsigned long rangeStart = 0;
    unsigned long rangeLen = 0;
    ckEx->Clear();
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);
    if (GenerateRange(&secIdx, &rangeStart, &rangeLen))
    {
        CodeHackCheckRange range;
        range.m_start = rangeStart;
        range.m_size  = rangeLen;
        rangeEx->m_start104 = static_cast<int>(rangeStart);
        rangeEx->m_size108 = static_cast<int>(rangeLen);
        for (std::size_t i = 0; i < m_exeFiles.size(); ++i)
        {
            m_exeFiles[i]->GenerateChecksum(&sum, secIdx, rangeStart, rangeLen);
            ckEx->SetCodeHackCheckRange(range);
            ckEx->AddCodeHackChecksum(sum);
        }
    }
    return true;
}

// ===========================================================================
// WindowsCodeProtectorServer —— 0x08575462..
// ===========================================================================
WindowsCodeProtectorServer::WindowsCodeProtectorServer()
    : m_dataFamilies()
{
    Initialize();                                            // ORIG 0x08575462：仅 vector 构造 + Initialize()
}                                                            // （m_exeMin..m_dataMax 由 SetExeCheckingOption 设置，构造不初始化）

WindowsCodeProtectorServer::~WindowsCodeProtectorServer()
{                                                            // 0x08575484 → Destroy + ~vector
    Destroy();
}

void WindowsCodeProtectorServer::Initialize()                // 0x085754cc
{
    m_exeFamily = 0;
    m_cfgLong = 0;
    m_retryCount = 0;
    m_loaded = 0;
    m_curDataFamily = 0;
}

void WindowsCodeProtectorServer::Destroy()                   // 0x08575500
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

bool WindowsCodeProtectorServer::FindExeFamilyFile(const char* name) // 0x085755f6
{
    if (m_exeFamily != 0 && strcmp(m_exeFamily->m_name, name) == 0)
    {
        return true;
    }
    return false;
}

WindowsDataFileFamily*
WindowsCodeProtectorServer::FindDataFamilyFile(const char* name) // 0x0857562e
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

void WindowsCodeProtectorServer::SetExeCheckingOption(long cfg, long e1,
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

bool WindowsCodeProtectorServer::BeginLoading()              // 0x08575700
{
    m_loaded = 0;
    return true;
}

bool WindowsCodeProtectorServer::LoadWindowsFile(const char* path,
                                                 const char* name) // 0x08575712
{
    // b21 verdict=DIFF（N 布局残余）：163/175，21=21 调用点全等（strstr/FindExe*/FindData*/
    //   LoadWindows{Exe,Data}File/SetCheckingOption×3/Windows{Exe,Data}FileFamily C1+D1/
    //   operator_new/operator_delete/push_back/Unwind_Resume）。exe(.exe.bak)↔data 频域、
    //   new+Load→Set-check→push_back 语义、失败删 `delete family` 均已核。残余仅 4.1 共享
    //   尾块 fall-through vs 4.4 独立 return 块、寄存器分配、块调度。语义完成，N 封顶。
    if (strstr(path, ".exe.bak") == 0)                       // data 文件域
    {
        WindowsDataFileFamily* family = FindDataFamilyFile(name);
        if (family == 0)
        {
            family = new WindowsDataFileFamily;
            if (!family->LoadWindowsDataFile(path, name))
            {
                delete family;
                return false;
            }
            family->SetCheckingOption(
                static_cast<unsigned long>(m_dataMin),
                static_cast<unsigned long>(m_dataMax));
            m_dataFamilies.push_back(family);
            return true;
        }
        return family->LoadWindowsDataFile(path, name);
    }

    if (m_exeFamily == 0)                                    // 首个 .exe.bak
    {
        m_exeFamily = new WindowsExeFileFamily;
        if (m_exeFamily->LoadWindowsExeFile(path, name))
        {
            m_exeFamily->SetCheckingOption(
                static_cast<unsigned long>(m_exeMin),
                static_cast<unsigned long>(m_exeMax));
            return true;
        }
        return false;
    }

    if (FindExeFamilyFile(name) &&
        m_exeFamily->LoadWindowsExeFile(path, name))
    {
        m_exeFamily->SetCheckingOption(
            static_cast<unsigned long>(m_exeMin),
            static_cast<unsigned long>(m_exeMax));
        return true;
    }
    return false;
}

bool WindowsCodeProtectorServer::CompleteLoading()           // 0x0857592c
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

bool WindowsCodeProtectorServer::IsDifferentExeVersionRoughly() // 0x08575970
{
    if (m_exeFamily != 0)
    {
        unsigned long maxFileSize = 0;
        // ORIG：迭代器 for 循环（begin/end/++/de）
        for (std::vector<WindowsExeFile*>::iterator it =
                 m_exeFamily->m_exeFiles.begin();
             it != m_exeFamily->m_exeFiles.end(); ++it)
        {
            if (maxFileSize < (*it)->m_size)
            {
                maxFileSize = (*it)->m_size;
            }
        }
        if (static_cast<unsigned long>(
                m_exeFamily->GetTotalIntervalSize() * 2) <= maxFileSize)
        {
            return true;
        }
    }
    return false;
}

bool WindowsCodeProtectorServer::GenerateChecksum(CodeHackChecksumEx* ckEx,
                                                  CodeHackCheckRangeEx* rangeEx) // 0x08575a3c
{
    if (m_loaded == 0)
    {
        return false;
    }
    if ((m_exeFamily == 0) && m_dataFamilies.empty())
    {
        return false;
    }

    bool wantExe = false;
    bool wantData = false;
    if (m_exeFamily != 0 && m_dataFamilies.empty())
    {
        wantExe = true;
    }
    else if (m_exeFamily == 0 || !m_dataFamilies.empty())
    {
        if (m_exeFamily == 0)
        {
            wantData = true;
        }
        else if (m_retryCount < m_cfgLong)
        {
            wantExe = true;
            m_retryCount += 1;
        }
        else
        {
            wantData = true;
            m_retryCount = 0;
        }
    }

    if (wantExe && m_exeFamily->GenerateChecksum(ckEx, rangeEx))
    {
        rangeEx->SetFileName("");
        return true;
    }
    if (wantData)
    {
        // ORIG：m_dataFamilies[m_curDataFamily] 各取一次（不缓存指针）
        if (m_dataFamilies[m_curDataFamily]->GenerateChecksum(ckEx, rangeEx))
        {
            if (m_dataFamilies[m_curDataFamily]->IsScanningAddressReset())
            {
                m_curDataFamily += 1;
                if (static_cast<std::size_t>(m_curDataFamily) >=
                    m_dataFamilies.size())
                {
                    m_curDataFamily = 0;
                }
            }
            return true;
        }
    }
    return false;
}

// ===========================================================================
// CCompressedBlob —— objdump 0x0844de52..0x0844e0aa 逐指令转写
// [b28c-lane24] throw()（P216 static-local guard 边界）：ORIG put_compressed_blob_data
// @0x083fbd06 无 landing pad；W 体尺寸不变（-fno-enforce-eh-specs 无 unexpected guard）。
// ===========================================================================
CCompressedBlob::CCompressedBlob() throw() : m_buf(0), m_size(0)
{
}                                                            // 0x0844de52

CCompressedBlob::~CCompressedBlob() throw()
{                                                            // 0x0844de6a
    if (m_buf != 0)
    {
        delete[] m_buf;
        m_buf = 0;
    }
}

void CCompressedBlob::resize(int needSize)                   // 0x0844e02a
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

void CCompressedBlob::reset()                                // 0x0844e080
{
    memset(m_buf, 0, m_size);
}

bool CCompressedBlob::compress(MySQL* db, int fieldIdx, char* src, int srcLen,
                               char** outStr)                // 0x0844de9a
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

bool CCompressedBlob::uncompress(MySQL* db, int fieldIdx, char* dst,
                                 int dstLen, int align)      // 0x0844df40
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
