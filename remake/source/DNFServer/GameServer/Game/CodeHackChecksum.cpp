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
// [b28c-L23 N18a/N7 残余 2 行] 本件已修一处**真语义 bug**（fread 实参序：ORIG
// @0x0808a53c 压参 `(%esp)=buffer / 0x4=chunk / 0x8=$1 / 0xc=fp` ⇒
// fread(ptr,size=chunk,nmemb=1)，原码 `(buffer,1,chunk)` 在 chunk>1 短读时返回值
// 语义分叉）。残 2 行 = `chunkSize + done` 的加法操作数寄存器来源
// （4.1 左操作数入 %eax/右入 %edx，4.4 反向 ⇒ 同助记符异寄存器，NEAR 级）。
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
        // [b28d-lane16] ORIG 0x0808a539 `mov 0x14(%ebp),%eax; mov -0x10(%ebp),%edx;
        // lea (%edx,%eax,1),%eax`：4.1 对 `done + chunkSize` 先载右操作数(chunkSize)
        // 入 %eax、左(done)入 %edx；4.4 恒先载右 —— 书写序翻转为 done+chunkSize 即
        // 逐条复现（P243 整数类比：加法书写序决定两操作数装载序）。
        if (done + chunkSize < totalSize)
            chunk = chunkSize;
        else
            chunk = totalSize - done;

        // b28c-l23 语义修正（ORIG 0x0808a53c 实参槽逐位核对）：ORIG 压参为
        // `(%esp)=buffer / 0x4=chunk / 0x8=$1 / 0xc=fp` ⇒ **fread(ptr, size=chunk,
        // nmemb=1, stream)**（每轮 1 个元素、元素大小=chunk），非 `(buffer,1,chunk)`；
        // 尾 `cmp $0x1,%eax` 判的是元素计数。原形在 chunk>1 短读时返回值语义分叉。
        if (fread(buffer, chunk, 1, fp) != 1)
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
// [b28d-Main 代落·S329 P299/P317] 用户 ctor/dtor 定义删除（声明端同步）：
// ORIG 无 C1/C2 符号，D1/D2 由使用点（new_allocator::destroy）发射 W。

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

// [b28d-solo-CodeHackChecksum AE] ORIG 0x0808a662：显式 `(const char*)` 实参形
// 闭合 memcmp 装载序（见函数体内 P268 cast 旋钮注释）；仅余 call 重定位伪差。
bool CodeHackChecksumData::IsRightChecksum(const CodeHackChecksum* p) // T @0x0808a662
{
    // [b28d-lane16] ORIG 0x0808a668 `lea -0x14(%ebp); call __normal_iterator::C1Ev`
    // = 默认构造；0x0808a680 begin() sret -0x1c 后 `mov -0x1c,%eax; mov %eax,-0x14`
    // = 先声明后赋值（P50 复活形，缺默认 ctor 弱实例即 +2 missing）；循环体
    // 0x0808a69b `mov %eax,-0xc(%ebp)` = 解引用结果具名指针局部（P0 槽往返）。
    checksum_iterator it;
    it = m_checksums.begin();
    for (; it != m_checksums.end(); ++it)
    {
        const CodeHackChecksum* e = &(*it);
        // [b28d-solo-CodeHackChecksum] ORIG 0x0808a69e 先载 p→%edx / e→%eax 再压常量；
        // 4.4 对裸指针实参恒即时右到左压参（常量先）⇒ 显式 `(const char*)` cast
        // 复现 ORIG「先求值后压栈」调度（P268 cast 旋钮族；探针 /tmp/probe_chc2.cpp
        // g1 裸形 vs g2 cast 形）。
        if (std::memcmp((const char*)e, (const char*)p, 0x20) == 0)
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

// [b28d-lane16 N84②] 拷贝构造不定义：ORIG 0x08575f10 为 W 隐式合成（基类 C1ERKS
// + m_fileName rep movsl 0x41 + m_range 双 int 直拷）；显式体恒 call memcpy（24 vs 32）。
// 隐式发射由本 TU PopData 的 `CodeHackChecksumEx tmp(m_queue.front())` 保证（ODR 使用点）。

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
// [b28d-solo-CodeHackChecksum N-P75 封顶] ORIG 0x0808a802：唯一残差 = Add 调用点
// 的 this/实参装载序（ORIG this-first，4.4 实参-first，见循环内注释），八形探针
// 均不可翻 ⇒ 跨版本求值序，语义等价。
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
        // [b28d-solo-CodeHackChecksum N-P75 封顶] ORIG 0x0808a89e `mov 0x8(%ebp),%eax;
        // lea -0x34(%ebp),%edx`（this 先求值，实参后）；4.4 对成员调用恒先求实参、
        // 后求 this（求值序右到左）。本波八形实测均不翻：裸调用/显式 this->/
        // Data:: 限定/reinterpret_cast 向上/static_cast 向上/(*this)/&buf[0]/
        // 派生类调基类方法/逗号表达式（探针 /tmp/probe_chc.cpp、probe_chc2.cpp
        // sA-sE、probe_chc3.cpp tA-tF、probe_chc4.cpp uA-uD）⇒ 跨版本实参/this
        // 求值序差异（P75），语义等价，封顶。
    }
}

// [b28d-solo-CodeHackChecksum AE] ORIG 0x0808a8c8：与 Data 版同形，显式
// `(const char*)` 实参形闭合（见函数体内注释）；仅余 call 重定位伪差。
bool CodeHackChecksumEx::IsRightChecksum(const CodeHackChecksum* p) // T @0x0808a8c8
{
    // [b28d-lane16] 与 CodeHackChecksumData::IsRightChecksum 同形：ORIG 0x0808a8ce
    // `call __normal_iterator::C1Ev` 默认构造 + begin() 临时槽 -0x1c + 解引用结果
    // 具名指针局部 -0xc（P50 复活 / P0 槽往返）。
    checksum_iterator it;
    it = m_checksums.begin();
    for (; it != m_checksums.end(); ++it)
    {
        const CodeHackChecksum* e = &(*it);
        // [b28d-solo-CodeHackChecksum] memcmp 实参须显式 `(const char*)` 形：
        // ORIG 0x0808a8ce 先载 p→%edx / e→%eax 再压常量，4.4 对裸指针实参
        // 恒即时右到左压参（常量先）；显式 cast 使 4.4 复现「先求值后压栈」
        // 调度（P268 cast 旋钮族，Data 版同形 0x0808a69e）。
        if (std::memcmp((const char*)e, (const char*)p, 0x20) == 0)
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
    // [b28c-L29] ORIG 0x0808a9cc：形参 serialized 自身自增（addl $0x4,0xc(%ebp)/
    // add %eax,0xc(%ebp)），每处重读形参槽——源码无命名局部 p（P0）。
    int nameLen = static_cast<signed char>(*serialized);  // movzbl/movsbl
    serialized += 4;                                      // addl $0x4
    std::strncpy(m_fileName, toTString(serialized), nameLen);
    serialized += nameLen;                                // add %eax,0xc(%ebp)
    std::memcpy(&m_start104, serialized, 8);              // memcpy(this+0x104,...,8)
    serialized += 8;                                      // addl $0x8
    return const_cast<char*>(serialized);
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
