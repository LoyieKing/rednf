// ============================================================================
// WindowsPackFiles.cpp —— WindowsExe/DataFile/PackFile 校验族实现
//
// 语义来源：docs/class_func_reports/{CBitManager,CObfuscator,WindowsDataFile,
//   WindowsDataFileFamily,WindowsExeFile,WindowsExeFileFamily,
//   WindowsCodeProtectorServer}/ 各 md 的 ## 反编译 C；CCompressedBlob 与
//   CompareWindowsExeConstantSectionsForSort/LoadClientFile 由 objdump 直接转写。
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
    unsigned long sa = (*a)->start;
    unsigned long sb = (*b)->start;
    if (sa < sb)
    {
        return -1;
    }
    return (sa == sb) ? 0 : 1;
}

// ===========================================================================
// CBitManager
// ===========================================================================
CBitManager::CBitManager()
    : m_data(0), m_buffer(0), m_size(0), m_field0c(0)
{
}                                                // 0x0831c09c

CBitManager::~CBitManager()
{
}                                                // 0x0831c0c8 空析构

void CBitManager::SetAll(int value, unsigned char* buf)     // 0x0831c0ce
{
    // ORIG 0x0831c0d4/0x0831c0db：i 声明置 0 + for 头再置 0（两条 movl $0 同
    // 槽）——原码即 `int i = 0; for (i = 0; ...)` 形，忠实保留。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        if (value == 0)
        {
            buf[i] = 0x00;
        }
        else
        {
            buf[i] = 0xff;
        }
    }
}

void CBitManager::ReadData(unsigned char* data, int size)   // 0x0831c116
{
    m_data = data;
    m_size = size;
    m_buffer = static_cast<unsigned char*>(malloc(size));
    memset(m_buffer, 0, size);
}

unsigned char CBitManager::GetBitFromLSB(int bits)          // 0x0831c160
{
    // ORIG 0x0831c160：栈上 8 字节掩码表**逐元素赋值**（非初始化列表——P60
    // 列表形变 .rodata 模板+rep movs，ORIG 为 8 条 movb 立即数）；下标 bits-1；
    // -0x10 dead int 零初始化槽（P31）；ShiftRight 第 3 实参 = mov (%eax) =
    // this+0 即 m_data（此前 m_buffer 系误转，0x0831c1b8 实证）。
    unsigned char table[7];
    table[0] = 0x01;
    table[1] = 0x03;
    table[2] = 0x07;
    table[3] = 0x0f;
    table[4] = 0x1f;
    table[5] = 0x3f;
    table[6] = 0x7f;
    unsigned char ret = 0;
    int unused = 0;
    (void)unused;
    unsigned char mask = table[bits - 1];
    ret = m_data[m_size - 1] & mask;
    ShiftRight(bits, m_data);
    return ret;
}

void CBitManager::ShiftRight(int bits, unsigned char* buf)  // 0x0831c1d6
{
    for (int i = m_size; i > 0; --i)
    {
        buf[i - 1] = static_cast<unsigned char>(buf[i - 1] >> (bits & 0x1f));
        if ((i - 1) != 0)
        {
            buf[i - 1] = static_cast<unsigned char>(
                (static_cast<unsigned int>(buf[i - 2])
                     << ((8 - (bits & 0x1f)) & 0x1f)) |
                buf[i - 1]);
        }
    }
}

void CBitManager::ShiftLeft(int bits, unsigned char* buf)   // 0x0831c266
{
    for (int i = 0; i < m_size; ++i)
    {
        buf[i] = static_cast<unsigned char>(buf[i] << (bits & 0x1f));
        if (i != m_size - 1)
        {
            buf[i] = static_cast<unsigned char>(
                buf[i] | (buf[i + 1] >> ((8 - (bits & 0x1f)) & 0x1f)));
        }
    }
}

unsigned char CBitManager::GetBit(int idx, int bitLo, int bitHi) // 0x0831c2fc
{
    // ORIG 0x0831c2fc：int 循环槽先置 0（-0x8）、acc/ret 置 0（-0x2/-0x1），
    // 栈掩码表逐元素赋值，(int)b<=bitHi setle 形循环。
    int b = 0;
    unsigned char acc = 0;
    unsigned char ret = 0;
    unsigned char masks[8];
    masks[0] = 0x01;
    masks[1] = 0x02;
    masks[2] = 0x04;
    masks[3] = 0x08;
    masks[4] = 0x10;
    masks[5] = 0x20;
    masks[6] = 0x40;
    masks[7] = 0x80;
    for (b = bitLo; b <= bitHi; ++b)
    {
        acc = static_cast<unsigned char>(acc | masks[b]);
    }
    ret = static_cast<unsigned char>(m_data[idx] & acc);
    return ret;
}

// ===========================================================================
// CObfuscator —— 位重排混淆（7bit 有效位流水）
// ===========================================================================
CObfuscator::CObfuscator()                                   // 0x08585888
{
    // ORIG 0x08585888：base CBitManager ctor → 局部临时 CBitManager ctor/dtor
    // （0x085858a1/0x085858ac）。ORIG 4.1 对 temp ctor call 点带 EH landing pad
    // （+0x2b..+0x41 dtor+Unwind_Resume，"new-expression landing-pad 抑制"反例
    // 族：4.4 判定半构造对象无需 cleanup 不发 landing，14 vs 30 的 16 条全在
    // 此块 + push esi/ebx 帧差），正常路径指令逐条同形。
    CBitManager temp;
}

CObfuscator::~CObfuscator()                                  // 0x085858d6
{
}

int CObfuscator::ObfuscatePacket(unsigned char* data, int len, int type) // 0x085858ea
{
    ReadData(data, len);
    if (type == 1)
    {
        Obfuscate1();
    }
    else if (type == 2)
    {
        Obfuscate2();
    }
    return reinterpret_cast<int>(m_buffer);
}

int CObfuscator::DeobfuscatePacket(unsigned char* data, int len, int type) // 0x08585938
{
    ReadData(data, len);
    if (type == 1)
    {
        Deobfuscate1();
    }
    else if (type == 2)
    {
        Deobfuscate2();
    }
    return reinterpret_cast<int>(m_buffer);
}

int CObfuscator::Obfuscate1()                                // 0x08585986
{
    int cnt = (m_size * 8) / 7;
    m_field0c = (m_size * 8) % 7;                            // 余位（this+0xc）
    for (int i = 0; i <= cnt; ++i)
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
    int cnt = (m_size * 8) / 7;
    m_field0c = (m_size * 8) % 7;
    for (int i = 0; i <= cnt; ++i)
    {
        if ((m_field0c == 0) && (i == cnt))
        {
            break;                                            // ORIG 循环条件排除该次迭代
        }
        unsigned char bits;
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
    // ORIG 单函数 103 条指令（CObfuscator/Obfuscate2.md 报告）：
    // 局部 i(this+8 区 size-1 倒序)、outIdx、tail 初值 6；
    // 每轮 GetBit(idx,0,tail)<<（7-tail）与 GetBit(idx,tail+1,7)>>(tail+1)
    // 合成字节写 m_buffer[size-1]，再搬到 m_buffer[outIdx]。
    int outIdx = 0;
    int tail = 6;
    for (int idx = m_size - 1; idx >= 0; --idx)
    {
        unsigned char hi = static_cast<unsigned char>(
            static_cast<unsigned int>(GetBit(idx, 0, tail)) <<
                ((7 - tail) & 0x1f));
        m_buffer[m_size - 1] = hi;
        unsigned char lo = GetBit(idx, tail + 1, 7);
        m_buffer[m_size - 1] |= static_cast<unsigned char>(
            (static_cast<unsigned int>(lo) >> ((tail + 1) & 0x1f)));
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
    // ORIG tail 初值：m_size%7==0 → 6；否则 m_size%7 - 1（0→6, 1→0 … 6→5）。
    // 与 Obfuscate2 的固定 tail=6 不同。循环体两函数同构。
    int outIdx = 0;
    int tail = (m_size % 7 == 0) ? 6 : (m_size % 7) - 1;
    for (int idx = m_size - 1; idx >= 0; --idx)
    {
        unsigned char hi = static_cast<unsigned char>(
            static_cast<unsigned int>(GetBit(idx, 0, tail)) <<
                ((7 - tail) & 0x1f));
        m_buffer[m_size - 1] = hi;
        unsigned char lo = GetBit(idx, tail + 1, 7);
        m_buffer[m_size - 1] |= static_cast<unsigned char>(
            (static_cast<unsigned int>(lo) >> ((tail + 1) & 0x1f)));
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
    return LoadClientFile(&m_size,
                          reinterpret_cast<void**>(&m_data), path);
}

void WindowsDataFile::GenerateChecksum(CodeHackChecksum* ck,
                                       unsigned long offset,
                                       unsigned long len)    // 0x08574eba
{
    if (offset < m_size)
    {
        if (m_size < offset + len)
        {
            len = m_size - offset;
        }
        CalculateMemoryChecksum(ck, len, m_data + offset);
    }
    else
    {
        ZeroMemoryChecksum(ck);
    }
}

// ===========================================================================
// WindowsExeFile —— 0x085736be..
// ===========================================================================
WindowsExeFile::WindowsExeFile()
    : m_size(0), m_image(0), m_sections(), m_commonSections()
{
}                                                            // 0x085736be

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

// ReadPeHeader —— T @0x085738f6（0x7de）：解析 PE 头产出常量区段表
bool WindowsExeFile::ReadPeHeader()
{
    if (m_image == 0)
    {
        return false;
    }

    unsigned int peOff = *reinterpret_cast<unsigned int*>(m_image + 0x3c);
    unsigned char* pe = m_image + peOff;                 // "PE\0\0"

    unsigned short numberOfSections =
        *reinterpret_cast<unsigned short*>(pe + 4 + 2);  // COFF+2 NumberOfSections
    unsigned long* opt =
        reinterpret_cast<unsigned long*>(pe + 0x18);     // OptionalHeader
    unsigned long numOfRvaAndSizes = opt[0x17];          // +0x5c

    unsigned char* sechdr = pe + 0xf8;                   // 首个 SectionHeader（标准 PE32）

    for (int s = 0; s < numberOfSections; ++s, sechdr += 0x28)
    {
        unsigned long* sh = reinterpret_cast<unsigned long*>(sechdr);
        unsigned int vaDelta = sh[5] - sh[3];            // PtrRawData - VirtualAddress
        long chars = static_cast<long>(sh[9]);           // Characteristics
        if (chars < 0 || (sh[9] & 0x40000000u) == 0)     // 未加载/不可读 → 跳过
        {
            continue;
        }
        unsigned int rawLen = (sh[2] < sh[4]) ? sh[2] : sh[4]; // min(VirtSize,RawSize)

        IntervalSet workSet;
        Interval base(sh[3], rawLen);
        workSet.AddInterval(base);

        if ((sh[9] & 0x20000000u) != 0)                  // CODE 可执行分支
        {
            for (unsigned long d = 0; d < numOfRvaAndSizes; ++d)
            {
                bool overlapped = IsOverlappedInterval(
                    sh[3], sh[2],
                    opt[(d + 12) * 2], opt[(d + 12) * 2 + 1]);   // DataDirectory[@opt+0x60]
                if (overlapped && d != 2 && d != 6)
                {
                    Interval dir(opt[(d + 12) * 2],
                                 opt[(d + 12) * 2 + 1]);
                    workSet.MinusInterval(dir);
                }
            }

            // DataDirectory idx2 = Import Table：解剖描述符字符串/INT 表
            bool hasImport =
                (numOfRvaAndSizes >= 2) &&
                IsOverlappedInterval(sh[3], sh[2], opt[0x1a], opt[0x1b]);
            if (hasImport)
            {
                unsigned char* descBase = m_image + opt[0x1a];
                for (int e = 0;
                     *reinterpret_cast<unsigned int*>(descBase + e * 0x14) != 0 ||
                     *reinterpret_cast<unsigned int*>(descBase + e * 0x14 + 0x10) != 0;
                     ++e)
                {
                    unsigned int originalFirstThunk =
                        *reinterpret_cast<unsigned int*>(descBase + e * 0x14);
                    unsigned char* thunkArr =
                        m_image + originalFirstThunk + vaDelta;
                    int hintCount = 0;
                    for (; *reinterpret_cast<unsigned int*>(thunkArr +
                                                            hintCount * 4) != 0;
                         ++hintCount)
                    {
                        unsigned int hintVal =
                            *reinterpret_cast<unsigned int*>(thunkArr +
                                                             hintCount * 4);
                        long hint = static_cast<long>(hintVal);
                        if (hint >= 0)
                        {
                            unsigned int nameOff = hintVal + vaDelta;
                            const char* nm =
                                reinterpret_cast<const char*>(m_image + nameOff);
                            unsigned int strLen =
                                static_cast<unsigned int>(strlen(nm)) + 1;
                            if (((strLen + nameOff) & 1U) != 0)
                            {
                                strLen += 1;              // 对齐到偶数
                            }
                            Interval name(hint, strLen);
                            workSet.MinusInterval(name);
                        }
                        // hint<0（ordinal 导入）不减 name 段，仅计数推进
                    }
                    unsigned int dllNameRva =
                        *reinterpret_cast<unsigned int*>(descBase + e * 0x14 + 0x0c);
                    if (dllNameRva != 0)
                    {
                        const char* dllName = reinterpret_cast<const char*>(
                            m_image + dllNameRva + vaDelta);
                        unsigned int dl =
                            static_cast<unsigned int>(strlen(dllName)) + 1;
                        if (((dl + dllNameRva + vaDelta) & 1U) != 0)
                        {
                            dl += 1;
                        }
                        Interval dll(static_cast<long>(dllNameRva), dl);
                        workSet.MinusInterval(dll);
                    }
                    Interval table(static_cast<long>(originalFirstThunk),
                                   (hintCount + 1) * 4);
                    workSet.MinusInterval(table);
                }
            }
        }
        else                                             // 数据段：减入口前 10 字节
        {
            Interval entryHead(static_cast<long>(opt[4]), 10);   // AddressOfEntryPoint
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
    if (!m_commonSections.empty())
    {
        qsort(&m_commonSections[0], m_commonSections.size(),
              sizeof(WindowsExeConstantSection*),
              CompareWindowsExeConstantSectionsForSort);
    }
}

bool WindowsExeFile::MakeCommonWindowsExeConstantSections(
    const IntervalSet& intervals)                            // 0x085741c6
{
    bool valid =
        !(intervals.m_intervals.empty() || m_sections.empty());

    m_commonSections.clear();
    if (!valid)
    {
        return false;
    }

    for (std::size_t iv = 0; iv < intervals.m_intervals.size(); ++iv)
    {
        long rangeStart = intervals.m_intervals[iv].m_start;
        long rangeSize = intervals.m_intervals[iv].m_size;
        for (std::size_t k = 0; k < m_sections.size(); ++k)
        {
            const WindowsExeConstantSection& src = *m_sections[k];
            if ((src.start <= rangeStart) &&
                (rangeStart < src.start + src.size))
            {
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = rangeStart;
                item->fileOffset = src.fileOffset + (rangeStart - src.start);
                item->size       = rangeSize;
                item->flag       = src.flag;
                m_commonSections.push_back(item);
                break;                                       // 首个匹配即止
            }
        }
    }
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

void WindowsExeFile::GenerateChecksum(CodeHackChecksum* ck, long sectionIdx,
                                      unsigned long offset, unsigned long len) // 0x085743d4
{
    long total = static_cast<long>(m_commonSections.size());
    if (sectionIdx < total)
    {
        const WindowsExeConstantSection& sec = *m_commonSections[sectionIdx];
        CalculateMemoryChecksum(ck, len,
                                m_image + sec.fileOffset + (offset - sec.start));
    }
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
    // ORIG 0x08575068：迭代器循环（begin 临时槽 + 每轮 end/operator!=/
    // operator* 直调）；memcmp 长度实参照抄取 f->m_size。
    for (std::vector<WindowsDataFile*>::iterator it = m_files.begin();
         it != m_files.end(); ++it)
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
    if (IsDifferentFileData(f))
    {
        if (m_maxSize < f->m_size)
        {
            m_maxSize = f->m_size;
        }
        m_files.push_back(f);
    }
    else
    {
        delete f;                                            // 内容重复仍返回 1
    }
    return true;
}

bool WindowsDataFileFamily::GenerateRange(unsigned long* outStart,
                                          unsigned long* outLen)   // 0x08575230
{
    if (m_files.empty())
    {
        return false;
    }
    unsigned long span =
        static_cast<unsigned long>(m_randMin) +
        static_cast<unsigned long>(rand()) %
            (static_cast<unsigned long>(m_randMax) + 1U);

    bool clipped = (m_maxSize <= m_scanOffset + span);
    if (clipped)
    {
        span = m_maxSize - m_scanOffset;
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
    ckEx->Clear();                                           // CodeHackChecksumData::Clear
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);

    unsigned long rangeStart = 0;
    unsigned long rangeLen = 0;
    if (!GenerateRange(&rangeStart, &rangeLen))
    {
        return false;
    }
    rangeEx->m_start104 = static_cast<int>(rangeStart);
    rangeEx->m_size108 = static_cast<int>(rangeLen);

    CodeHackCheckRange range;
    range.m_start = static_cast<int>(rangeStart);
    range.m_size  = static_cast<int>(rangeLen);
    for (std::size_t i = 0; i < m_files.size(); ++i)
    {
        CodeHackChecksum sum;
        m_files[i]->GenerateChecksum(&sum, rangeStart, rangeLen);
        ckEx->SetCodeHackCheckRange(range);
        ckEx->AddCodeHackChecksum(sum);
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
    if (IsDifferentFileData(exe))
    {
        m_exeFiles.push_back(exe);
        return true;
    }
    delete exe;
    return true;                                             // 重复内容视为成功
}

bool WindowsExeFileFamily::IsDifferentFileData(WindowsExeFile* f) // 0x08574ace
{
    // ORIG 0x08574ace：迭代器循环形（begin 临时槽 + 每轮 end/operator!=/operator*
    // 直调，无 size/[] 索引链）。
    for (std::vector<WindowsExeFile*>::iterator it = m_exeFiles.begin();
         it != m_exeFiles.end(); ++it)
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
    ckEx->Clear();
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);

    long secIdx = 0;
    unsigned long rangeStart = 0;
    unsigned long rangeLen = 0;
    if (!GenerateRange(&secIdx, &rangeStart, &rangeLen))
    {
        return false;
    }
    rangeEx->m_start104 = static_cast<int>(rangeStart);
    rangeEx->m_size108 = static_cast<int>(rangeLen);

    CodeHackCheckRange range;
    range.m_start = static_cast<int>(rangeStart);
    range.m_size  = static_cast<int>(rangeLen);
    for (std::size_t i = 0; i < m_exeFiles.size(); ++i)
    {
        CodeHackChecksum sum;
        m_exeFiles[i]->GenerateChecksum(&sum, secIdx, rangeStart, rangeLen);
        ckEx->SetCodeHackCheckRange(range);
        ckEx->AddCodeHackChecksum(sum);
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
    // ORIG 0x08575500：迭代器 for 循环（begin/end/++/de）
    for (std::vector<WindowsDataFileFamily*>::iterator it =
             m_dataFamilies.begin(); it != m_dataFamilies.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
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
    // ORIG：迭代器 for 循环（begin/end/++/de）
    for (std::vector<WindowsDataFileFamily*>::iterator it =
             m_dataFamilies.begin(); it != m_dataFamilies.end(); ++it)
    {
        if (strcmp((*it)->m_name, name) == 0)
        {
            return *it;
        }
    }
    return 0;
}

void WindowsCodeProtectorServer::SetExeCheckingOption(long cfg, long e1,
                                                      long e2, long d1,
                                                      long d2)      // 0x085756ce
{
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
    if (m_exeFamily != 0)
    {
        if (!m_exeFamily->CompleteLoadingWindowsExeFiles())
        {
            return false;
        }
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
// ===========================================================================
CCompressedBlob::CCompressedBlob() : m_buf(0), m_size(0)
{
}                                                            // 0x0844de52

CCompressedBlob::~CCompressedBlob()
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
