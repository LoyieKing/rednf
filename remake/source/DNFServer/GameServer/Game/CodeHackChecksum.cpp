// ===========================================================================
// CodeHackChecksum.cpp —— CodeHack 校验域 ORIG 忠实实现（df_game_r 反汇编逐指令转写）
//
// 符号地址证据见 CodeHackChecksum.h 文件头。外部依赖：
//   sha256_starts/update/finish、StrCpyMaxPath → 权威 producer 见
//   ShaSupport.h/.cpp（W7AB 组新建；sha256_process 为其内部压缩函数）。
// ===========================================================================

#include "CodeHackChecksum.h"

#include <cstring>

#include "SyncScriptDeps.h"   // toTString(const char*)

// W7_BuildHarmonize：struct/extern 由权威头 ShaSupport.h 取代（原 TU 局部
// 定义移出，保证唯一 owner 与全局命名空间 mangled 匹配）。
#include "ShaSupport.h"

static_assert(sizeof(sha256_context) == 0x68, "sha256_context layout drift");

// ---------------------------------------------------------------------------
// ZeroMemoryChecksum(CodeHackChecksum*)  T @0x0808a49c
//   starts(ctx); finish(ctx, ck->m_data);
// ---------------------------------------------------------------------------
void ZeroMemoryChecksum(CodeHackChecksum* ck)
{
    sha256_context ctx;
    sha256_starts(&ctx);
    sha256_finish(&ctx, ck->m_data);
}

// ---------------------------------------------------------------------------
// CalculateMemoryChecksum(CodeHackChecksum*, unsigned long, void*) T @0x0808a4c4
//   starts(ctx); update(ctx, data, size); finish(ctx, ck->m_data);
// ---------------------------------------------------------------------------
void CalculateMemoryChecksum(CodeHackChecksum* ck, unsigned long size, void* data)
{
    sha256_context ctx;
    sha256_starts(&ctx);
    sha256_update(&ctx, static_cast<unsigned char*>(data), size);
    sha256_finish(&ctx, ck->m_data);
}

// ---------------------------------------------------------------------------
// CalculateFileChecksum(CodeHackChecksum*, FILE*, ulong, ulong, void*) T @0x0808a505
//   total==0 → ZeroMemoryChecksum；否则按 chunkSize 循环 fread（每次 1 元素），
//   fread 失败即 finish 收尾；成功则 update 并累计，读满 total 后 finish。
// 第 4 参为每轮读取块大小（ORIG 分支：chunk = min(chunkSize, total - done)）。
// ---------------------------------------------------------------------------
void CalculateFileChecksum(CodeHackChecksum* ck, FILE* fp, unsigned long totalSize,
                           unsigned long chunkSize, void* buffer)
{
    unsigned long done = 0;
    if (totalSize == 0)
    {
        ZeroMemoryChecksum(ck);
        return;
    }

    sha256_context ctx;
    sha256_starts(&ctx);
    for (;;)
    {
        unsigned long chunk;
        if (chunkSize + done < totalSize)
            chunk = chunkSize;
        else
            chunk = totalSize - done;

        if (fread(buffer, 1, chunk, fp) != 1)
        {
            sha256_finish(&ctx, ck->m_data);
            return;
        }
        sha256_update(&ctx, static_cast<unsigned char*>(buffer), chunk);
        done += chunk;
        if (totalSize <= done)
            break;
    }
    sha256_finish(&ctx, ck->m_data);
}

// ===========================================================================
// class CodeHackChecksum —— POD；~CodeHackChecksum W @0x0808b7da 空体。
// ===========================================================================
CodeHackChecksum::CodeHackChecksum()
{
}

CodeHackChecksum::~CodeHackChecksum()
{
}

// ===========================================================================
// class CodeHackChecksumData
// ===========================================================================
CodeHackChecksumData::CodeHackChecksumData()              // W @0x0808ac84（空 vector）
{
}
void CodeHackChecksumEx::SetCodeHackCheckRange(const CodeHackCheckRange& range)  // T @0x0808a7d8
{
    m_range = range;                                     // ORIG 内联 2 mov（0x8(%eax)/0x4(%eax)→+0x110/+0x114），不 memcpy
}

CodeHackChecksumData::~CodeHackChecksumData()             // T @0x0808ac98
{
    m_checksums.clear();                                  // ORIG 先显式 vector::clear@0808ad4c，再走成员析构
}

CodeHackChecksumData::CodeHackChecksumData(const CodeHackChecksumData& other)  // W @08575ef6
    : m_checksums(other.m_checksums)
{
}

void CodeHackChecksumData::Clear()                        // T @0x0808a61a = vector::clear
{
    m_checksums.clear();
}

void CodeHackChecksumData::Reserve(long count)            // T @0x0808a62e = vector::reserve
{
    m_checksums.reserve(static_cast<unsigned int>(count));
}

void CodeHackChecksumData::AddCodeHackChecksum(const CodeHackChecksum& c)      // T @0x0808a648
{
    m_checksums.push_back(c);
}

bool CodeHackChecksumData::IsRightChecksum(const CodeHackChecksum* p) // T @0x0808a662
{
    for (checksum_iterator it = const_cast<CodeHackChecksumData*>(this)->m_checksums.begin();
         it != const_cast<CodeHackChecksumData*>(this)->m_checksums.end(); ++it)
    {
        if (std::memcmp(&(*it), p, 0x20) == 0)
            return true;
    }
    return false;
}

// ===========================================================================
// class CodeHackChecksumEx
// ===========================================================================
CodeHackChecksumEx::CodeHackChecksumEx()                  // T @0x0808a700
{
    std::memset(m_fileName, 0, sizeof(m_fileName));       // memset(this+0xc,0,0x104)
    std::memset(&m_range, 0, sizeof(m_range));            // memset(this+0x110,0,8)
    // ORIG 0x0808a74f..0x0808a766：memset 后对 m_range 两 int 预清直写（0x114 先/0x110 后）。
    m_range.m_size = 0;
    m_range.m_start = 0;
}

CodeHackChecksumEx::~CodeHackChecksumEx()                 // T @0x0808a76c
{
    m_checksums.clear();                                  // ORIG 直接 vector::clear
}                                                         // 随后隐式基类/vector 析构

CodeHackChecksumEx::CodeHackChecksumEx(const CodeHackChecksumEx& other)        // W @08575f10
    : CodeHackChecksumData(other)
    , m_range(other.m_range)
{
    std::memcpy(m_fileName, other.m_fileName, sizeof(m_fileName));
    // [N84 判词] ORIG 0x08575f10 该拷贝为 rep movsl 0x41 展开（4.1 常量尺寸
    // 大块拷贝启发式）；4.4.7 -O0 恒 call memcpy（探针 z1/z2：循环 int/char
    // 形均不折叠）。源形不可择，语义等价，N 封顶。
}

void CodeHackChecksumEx::SetFileName(const char* name)    // T @0x0808a7ae
{
    StrCpyMaxPath(m_fileName, name);
}

char* CodeHackChecksumEx::GetFileName()                   // T @0x0808a7cc
{
    return m_fileName;
}


CodeHackCheckRange* CodeHackChecksumEx::GetCodeHackCheckRange()                 // T @0x0808a7f4
{
    return &m_range;
}

// setData(char*) T @0x0808a802。
// 序列化布局：<int8 len> <len 字节文件名> <8 字节 range> <int8 count>
//             <count × 32 字节 CodeHackCheckpoint>……（长度字节按 ORIG 以
//             movzbl/movsbl 有符号读取转写）。
void CodeHackChecksumEx::setData(const char* serialized)
{
    // ORIG 直接推进形参槽（addl $0x4,0xc(%ebp)）——下文直接以 serialized 使用。
    int nameLen = static_cast<signed char>(*serialized);  // movzbl+movsbl
    serialized += 4;                                      // addl $0x4
    std::strncpy(m_fileName, toTString(serialized), nameLen);
    serialized += nameLen;                                // 名称缓冲推进
    std::memcpy(&m_range, serialized, sizeof(m_range));   // memcpy(this+0x110,__src,8)
    serialized += 8;                                      // addl $0x8
    int count = static_cast<signed char>(*serialized);    // local_14
    serialized += 4;                                      // addl $0x4
    for (int i = 0; i < count; ++i)
    {
        // ORIG 循环体仅 memcpy 到栈缓冲 + Add（无 entry 构造——ORIG 的
        // CodeHackChecksum ctor 为弱符号从未被调用）；reinterpret 避免 C1/D1。
        unsigned char buf[0x20];
        std::memcpy(buf, serialized, sizeof(buf));        // memcpy(local_34,p,0x20)
        serialized += 0x20;                               // addl $0x20
        AddCodeHackChecksum(*reinterpret_cast<CodeHackChecksum*>(buf));  // push_back
        // [N 残差] ORIG 0x0808a894 实参 lea -0x34 在 this 装载之后（mov 0x8(%ebp),%eax
        // 先）；4.4 恒 lea 先/this 后（N76 store 载入序族，实参地址先形），源形不可择。
    }
}

bool CodeHackChecksumEx::IsRightChecksum(const CodeHackChecksum* p) // T @0x0808a8c8
{
    for (checksum_iterator it = const_cast<CodeHackChecksumEx*>(this)->m_checksums.begin();
         it != const_cast<CodeHackChecksumEx*>(this)->m_checksums.end(); ++it)
    {
        if (std::memcmp(&(*it), p, 0x20) == 0)            // memcmp(local_10,param_1,0x20)
            return true;
    }
    return false;
}

// ===========================================================================
// class CodeHackCheckRangeEx
// ===========================================================================
CodeHackCheckRangeEx::CodeHackCheckRangeEx()              // T @0x0808a966
{
    std::memset(m_fileName, 0, 0x20);                     // memset(this,0,0x20) 仅前 0x20
    std::memset(&m_start104, 0, 8);                       // memset(this+0x104,0,8)
}

void CodeHackCheckRangeEx::SetFileName(const char* name)  // T @0x0808a9aa
{
    StrCpyMaxPath(m_fileName, name);
}

char* CodeHackCheckRangeEx::GetFileName() const           // T @0x0808a9c4
{
    return const_cast<char*>(m_fileName);
}

char* CodeHackCheckRangeEx::setData(const char* serialized)   // T @0x0808a9cc
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(serialized);
    int nameLen = static_cast<signed char>(*p);           // movzbl/movsbl
    p += 4;                                               // addl $0x4
    std::strncpy(m_fileName, toTString(reinterpret_cast<const char*>(p)), nameLen);
    p += nameLen;                                         // add %eax,0xc(%ebp)
    std::memcpy(&m_start104, p, 8);                       // memcpy(this+0x104,...,8)
    p += 8;                                               // addl $0x8
    return const_cast<char*>(reinterpret_cast<const char*>(p));
}

// ===========================================================================
// class CodeHackCheckStorage
// ===========================================================================
CodeHackCheckStorage::CodeHackCheckStorage()              // T @0x08575c22
{
    reset();                                              // call 08575e74
}

CodeHackCheckStorage::~CodeHackCheckStorage()             // W @0x08697b76（仅队列析构）
{
}

bool CodeHackCheckStorage::IsEmpty()                      // T @0x08575c9e
{
    return m_queue.size() == 0;
}

bool CodeHackCheckStorage::IsFull()                       // T @0x08575cb6
{
    return m_queue.size() == MAX_STORED_CHECKS;           // cmp $0x30,%eax / sete
}

bool CodeHackCheckStorage::PushData(const CodeHackChecksumEx* item)     // T @0x08575cd0
{
    if (IsFull())
        return false;                                     // mov $0x0,%eax
    m_queue.push(*item);
    return true;                                          // mov $0x1,%eax
}

bool CodeHackCheckStorage::PopData(char* fileNameOut)     // T @0x08575d06
{
    if (IsEmpty())
        return false;

    CodeHackChecksumEx tmp(m_queue.front());              // copy ctor front() 快照
    m_queue.pop();
    std::strcpy(fileNameOut, tmp.GetFileName());
    return true;
}

bool CodeHackCheckStorage::PopData()                      // T @0x08575dac（恒返回 true）
{
    m_queue.pop();
    return true;
}

bool CodeHackCheckStorage::IsRightChecksum(const CodeHackChecksum* p)   // T @0x08575dc4
{
    if (IsEmpty())
        return false;
    return m_queue.front().IsRightChecksum(p);
}

void CodeHackCheckStorage::reset()                        // W @0x08575e74
{
    while (!m_queue.empty())                              // empty→xor 1 判定
        m_queue.pop();
}
