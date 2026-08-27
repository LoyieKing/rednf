#ifndef DNF_GAME_CODEHACKCHECKSUM_H
#define DNF_GAME_CODEHACKCHECKSUM_H

// ===========================================================================
// CodeHack 校验域权威类型（df_game_r ORIG 反汇编逐指令转写）
//
// 覆盖 ORIG 符号：
//   W  _ZN16CodeHackChecksumD1Ev                       @0x0808b7da
//   T  _Z18ZeroMemoryChecksumP16CodeHackChecksum       @0x0808a49c
//   T  _Z23CalculateMemoryChecksumP16CodeHackChecksummPv @0x0808a4c4
//   T  _Z21CalculateFileChecksumP16CodeHackChecksumP8_IO_FILEmmPv @0x0808a505
//   T/W CodeHackChecksumData::{C1@0808ac84, D1@0808ac98, copy C1@08575ef6,
//       Clear@0808a61a, Reserve(long)@0808a62e,
//       AddCodeHackChecksum(const&)@0808a648, IsRightChecksum(const*)@0808a662}
//   T   CodeHackChecksumEx::{C1@0x0808a700, D1@0x0808a76c, copy C1 W@08575f10,
//       SetFileName@0x0808a7ae, GetFileName@0x0808a7cc,
//       SetCodeHackCheckRange@0x0808a7d8, GetCodeHackCheckRange@0x0808a7f4,
//       setData@0x0808a802, IsRightChecksum@0x0808a8c8}
//   T/W CodeHackCheckRangeEx::{C1@0x0808a966, SetFileName@0x0808a9aa,
//       GetFileName() const@0x0808a9c4, setData@0x0808a9cc}
//   T/W CodeHackCheckStorage::{C1@0x08575c22, reset W@0x08575e74,
//       dtor W@0x08697b76, IsEmpty@0x08575c9e, IsFull@0x08575cb6,
//       PushData(const*)@0x08575cd0, PopData(char*)@0x08575d06,
//       PopData()@0x08575dac, IsRightChecksum(const*)@0x08575dc4}
// 报告：docs/class_func_reports/CodeHack{ChecksumData,ChecksumEx,CheckRangeEx,
//       CheckStorage}/ 与 docs/class_func_reports/CUser/checkCodeChecksum*.md。
//
// !!!! ODR 说明（宿主收口） !!!!
// 当前 Game/CUser.h:851 也存在一个名为 `CodeHackCheckStorage` 的类视图
// （char m_pad[0x28] 之后把 +0x28..+0x67 声明成 getStdDropRate / tradePunish /
// serverGroup 等 CUser 状态字段）。经 objdump 核实，ORIG 真实的
// CodeHackCheckStorage 只含一个 std::queue<CodeHackChecksumEx>（sizeof=0x28，
// 队列容量上限 0x30 见 IsFull）；CUser.h:851 视图中 +0x28..+0x64 的具名字段是
// 与其它 CUser 子对象的「历史误并」，并非 Storage 布局。在宿主删除
// CUser.h:851 视图并统一 include 本头之前，任何同时包含两个头的 TU 都会触发
// 重定义错误。建议宿主：删除 CUser.h 局部类定义，仅保留本头作为唯一 owner。
// ===========================================================================

#include <vector>
#include <deque>
#include <queue>


#include <cstdio>

// ---------------------------------------------------------------------------
// class CodeHackChecksum —— SHA-256 摘要 POD，固定 0x20 字节。
// 证据：Ex/Data::IsRightChecksum 均 memcmp(elem, param, 0x20)；
//       ZeroMemoryChecksum 由 sha256_finish(ctx, this) 写入；
//       ~CodeHackChecksum @0x0808b7da 为空析构（push ebp/mov/pop/ret）。
// ---------------------------------------------------------------------------
class CodeHackChecksum
{
public:
    CodeHackChecksum();                                  // POD 式清零构造（隐式 W）
    ~CodeHackChecksum();

    unsigned char m_data[0x20];                          // +0x00..+0x1f 摘要
};

void ZeroMemoryChecksum(CodeHackChecksum* ck);           // T @0x0808a49c
void CalculateMemoryChecksum(CodeHackChecksum* ck, unsigned long size, void* data); // T @0x0808a4c4
void CalculateFileChecksum(CodeHackChecksum* ck, FILE* fp, unsigned long totalSize,
                           unsigned long chunkSize, void* buffer);                  // T @0x0808a505

// ---------------------------------------------------------------------------
// class CodeHackCheckRange —— {(int)start, (int)size} 8 字节基类视图。
// 证据：CodeHackChecksumEx::SetCodeHackCheckRange(const CodeHackCheckRange&)
//       @0x0808a7d8 将 (+0,+4) 两个 int 写入 Ex+0x110/+0x114；
//       GetCodeHackCheckRange 返回 this+0x110（ORIG 无独立符号，全内联）。
// ---------------------------------------------------------------------------
class CodeHackCheckRange
{
public:
    int m_start;                                         // +0x00（RangeEx 视图 +0x104）
    int m_size;                                          // +0x04（RangeEx 视图 +0x108）
};

// ---------------------------------------------------------------------------
// class CodeHackChecksumData —— std::vector<CodeHackChecksum> 容器（12B）。
// 证据：ctor @0x0808ac84 构造空 vector；Clear=vector::clear@0808ad4c；
//       Reserve=vector::reserve；AddCodeHackChecksum=push_back；
//       IsRightChecksum 线性 memcmp(0x20) 匹配返回 1 否则 0。
// ---------------------------------------------------------------------------
class CodeHackChecksumData
{
public:
    typedef std::vector<CodeHackChecksum>::iterator checksum_iterator;

    CodeHackChecksumData();
    ~CodeHackChecksumData();
    CodeHackChecksumData(const CodeHackChecksumData& other);      // W @08575ef6

    void Clear();                                        // T @0x0808a61a
    void Reserve(long count);                            // T @0x0808a62e
    void AddCodeHackChecksum(const CodeHackChecksum& c); // T @0x0808a648
    bool IsRightChecksum(const CodeHackChecksum* p) const;        // T @0x0808a662

    std::vector<CodeHackChecksum> m_checksums;           // +0x00..+0x0b
};

// ---------------------------------------------------------------------------
// class CodeHackChecksumEx : CodeHackChecksumData，总 0x118 字节。
// 证据：C1 @0x0808a700：memset(this+0xc,0,0x104)、memset(this+0x110,0,8)、
//       *(int*)(this+0x110)=0、*(int*)(this+0x114)=0；
//       SetFileName→StrCpyMaxPath(this+0xc, src)；GetFileName→this+0xc；
//       D1 @0x0808a76c：vector::clear 后走基类析构（同隐式布局等价）。
// ---------------------------------------------------------------------------
class CodeHackChecksumEx : public CodeHackChecksumData
{
public:
    static const unsigned int MAX_FILENAME_LEN = 0x104u;   // StrCpyMaxPath 缓冲（MAX_PATH）

    CodeHackChecksumEx();                                // T @0x0808a700
    ~CodeHackChecksumEx();                               // T @0x0808a76c
    CodeHackChecksumEx(const CodeHackChecksumEx& other);          // W @08575f10

    void SetFileName(const char* name);                  // T @0x0808a7ae
    char* GetFileName();                                 // T @0x0808a7cc（非 const 版本）
    void SetCodeHackCheckRange(const CodeHackCheckRange& range);  // T @0x0808a7d8
    CodeHackCheckRange* GetCodeHackCheckRange();         // T @0x0808a7f4
    void setData(const char* serialized);                // T @0x0808a802
    bool IsRightChecksum(const CodeHackChecksum* p) const;        // T @0x0808a8c8

    char m_fileName[0x104];                              // +0x0c..+0x10f（基类 12B 起）
    CodeHackCheckRange m_range;                          // +0x110..+0x117 {start,size}
};

static_assert(sizeof(CodeHackChecksum) == 0x20, "CodeHackChecksum ABI drift");
static_assert(sizeof(CodeHackChecksumData) == 0xc, "CodeHackChecksumData ABI drift");
static_assert(sizeof(CodeHackChecksumEx) == 0x118, "CodeHackChecksumEx ABI drift");
static_assert(sizeof(CodeHackCheckRange) == 0x8, "CodeHackCheckRange ABI drift");

// ---------------------------------------------------------------------------
// class CodeHackCheckRangeEx —— 独立 0x10c 字节（非继承，与 Ex.m_range 平行）。
// 证据：C1 @0x0808a966：memset(this,0,0x20)、memset(this+0x104,0,8)；
//       SetFileName→StrCpyMaxPath(this, src)；GetFileName() const→return this；
//       setData 把 8 字节范围拷到 this+0x104 并推进读指针（返回尾部）。
// 文件名有效长度受 StrCpyMaxPath（MAX_PATH=0x104）限制。
// ---------------------------------------------------------------------------
class CodeHackCheckRangeEx
{
public:
    static const unsigned int MAX_FILENAME_LEN = 0x104u;

    CodeHackCheckRangeEx();                              // T @0x0808a966

    void SetFileName(const char* name);                  // T @0x0808a9aa
    char* GetFileName() const;                           // T @0x0808a9c4
    char* setData(const char* serialized);               // T @0x0808a9cc

    char m_fileName[0x104];                              // +0x000..+0x103（StrCpyMaxPath 目标）
    int m_start104;                                      // +0x104
    int m_size108;                                       // +0x108
};

static_assert(sizeof(CodeHackCheckRangeEx) == 0x10c, "CodeHackCheckRangeEx ABI drift");

// ---------------------------------------------------------------------------
// class CodeHackCheckStorage —— queue<CodeHackChecksumEx> 校验包队列。
// 证据：唯一成员 std::queue<CodeHackChecksumEx, deque<...>>（GCC4.4 32 位
//       deque=0x28 → 对象 0x28）；IsFull=size()==0x30（容量 0x30）；
//       C1 先构造临时 deque 再以 queue(deque&&) 初始化后调 reset()；
//       reset()（W@0x08575e74）：while(!empty()) pop();
//       PushData：满则返回 false 否则 push 并返回 true；
//       PopData(char*)：front 拷贝出队并 strcpy 文件名；PopData()/空弹占位恒真。
//
// 与 CUser.h:851 旧视图差异见文件头部注释。
// ---------------------------------------------------------------------------
class CodeHackCheckStorage
{
public:
    static const unsigned int MAX_STORED_CHECKS = 0x30u;

    CodeHackCheckStorage();                              // T @0x08575c22
    ~CodeHackCheckStorage();                             // W @0x08697b76（仅队列析构）

    bool IsEmpty();                                      // T @0x08575c9e
    bool IsFull();                                       // T @0x08575cb6
    bool PushData(const CodeHackChecksumEx* item);       // T @0x08575cd0
    bool PopData(char* fileNameOut);                     // T @0x08575d06
    bool PopData();                                      // T @0x08575dac（恒返回 true）
    bool IsRightChecksum(const CodeHackChecksum* p);     // T @0x08575dc4
    void reset();                                        // W @0x08575e74

    std::queue<CodeHackChecksumEx, std::deque<CodeHackChecksumEx> > m_queue;
};
static_assert(sizeof(CodeHackCheckStorage) == 0x28, "CodeHackCheckStorage ABI drift");

#endif // DNF_GAME_CODEHACKCHECKSUM_H
