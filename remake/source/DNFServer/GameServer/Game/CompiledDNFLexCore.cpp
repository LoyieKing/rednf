// ============================================================================
// CompiledDNFLex 成员函数（词法引擎编译态层；ORIG 地址与汇编证据见
// docs/class_func_reports/CompiledDNFLex/ 目录）。
// ----------------------------------------------------------------------------
// 本 TU 提供（_ZN14CompiledDNFLex*，全部真实 C++ 成员函数；vtable/typeinfo
// 由编译器生成 —— ORIG _ZTV14CompiledDNFLex 仅 D1/D0 两虚析构槽）：
//   getStringManager()   0x8ad0070   setStringManager(...) 0x8ad0050
//   GetInputStream()     0x8acfea0   CloseInputStream()    0x8ad0e90
//   PopInputStream(bool) 0x8ad0c80   PushInputStream(...)  0x8ad0a00
//   SwitchInputStream()  0x8ad0f40   SetHandler(...)       0x8acfdf0
//   EnableInclusion()    0x8ad0090   DisableInclusion()    0x8ad0080
//   IsStream()           0x8acfe80   GetOffset()           0x8acff20
//   getStreamName()      0x8ad0290   isEnd()               0x8ad0000
//   LexerError(...)      0x8acfe10   LoadStream(...)       0x8acfe40
//   getToken()           0x8ad0fa0   getInt/getFloat/getStr 0x8acff70/0x8acffc0/
//                                                     0x8ad00d0/0x8ad0190/0x8ad0250
//   create(bool)         0x8ad03d0   destroy()             0x8ad0370
//   CompiledDNFLex()     0x8ad07c0 / (ScriptStringManager*) 0x8ad0580
//   ~CompiledDNFLex()    0x8ad0ed0 D1 / 0x8ad0f20 D0
//
// 布局（0x204c）：+0x00 vptr、+0x04 offset_、+0x08 lastToken_、+0x0c
// stringManagerDelete_、+0x10 stringManager_、+0x14 stringBuffer_、
// +0x18 tokenBuffer_[0x2000]、+0x2018 std::deque<stream_t> streamStack_
// （libstdc++ _Deque_base 同为 0x28 字节：map/map_size/start/finish）、
// +0x2040 allow_inclusion_、+0x2044 lexerError_、+0x2048 loadStream_。
//
// stream_t（0x14）：dir/filename 为真实 std::string（COW），size/buffer/offset。
// deque 节点容量由 libstdc++ 决定（sizeof(stream_t)==0x14 → 每节点
// 0x1f4/0x14=25 元素，与 ORIG _M_initialize_map 一致）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <new>
#include <string>

#include "CompiledDNFLex.h"
#include "ScriptStringManager.h"

// toMbcs/toTString（UnicodeConvert.cpp 提供，恒等转换）。
const char* toMbcs(const char* src);
const char* toMbcs(const std::string& src);
const char* toTString(const std::string& src);

namespace
{

const unsigned int STRING_BUFFER_SIZE = 0x400;
const unsigned int STREAM_NODE_ELEMS = 0x1f4 / sizeof(CompiledDNFLex::stream_t);

}  // namespace

// ORIG `_ZZ17readIntFromBufferRPcE9intBuffer` / `_ZZ18readCharFromBufferRPcE10charBuffer`
// / `_ZZ19readFloatFromBufferRPcE11floatBuffer`：外部链接、函数内 static，-O2 内联进
// getInt/getStr/getToken/getFloat。
static int readIntFromBuffer(char*& offset)
{
    static int intBuffer;
    intBuffer = *reinterpret_cast<int*>(offset);
    offset += 4;
    return intBuffer;
}

// [b28d-Main·检查点18] static 化：ORIG 符号表无 `_Z18readCharFromBufferRPc`
// （仅函数内 static `_ZZ18readCharFromBufferRPcE10charBuffer`）⇒ ORIG 为文件内静态
// 且被 -O2 内联；本 TU 与 readIntFromBuffer 同款。
static int readCharFromBuffer(char*& offset)
{
    static char charBuffer;
    signed char c = *reinterpret_cast<signed char*>(offset);
    ++offset;
    charBuffer = c;
    return c;
}

// [b28d-Main·检查点18] static 化：同上（ORIG 仅 `_ZZ19readFloatFromBufferRPcE11floatBuffer`）。
static float readFloatFromBuffer(char*& offset)
{
    static float floatBuffer;
    floatBuffer = *reinterpret_cast<float*>(offset);
    offset += 4;
    return floatBuffer;
}

// ---- getStringManager（ORIG 0x8ad0070）：返回 stringManager_。----
ScriptStringManager* CompiledDNFLex::getStringManager()
{
    return stringManager_;
}

// ---- setStringManager（ORIG 0x8ad0050）----
void CompiledDNFLex::setStringManager(ScriptStringManager* pStringManager,
                                      bool allowOwner)
{
    stringManagerDelete_ = allowOwner;
    stringManager_ = pStringManager;
}

// ---- GetInputStream（ORIG 0x8acfea0）：栈空返回 0，否则栈顶流。----
CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream()
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}

const CompiledDNFLex::stream_t* CompiledDNFLex::GetInputStream() const
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}

// ---- PopInputStream（ORIG 0x8ad0c80）----
// 栈空返回 false；!pop_all 且元素数 <2 返回 false（保留最底层流）。
// 否则弹出一个：释放 buffer、析构 dir/filename、pop_back；弹出后若栈仍非空，
// offset_ = 新栈顶 offset。返回 true。
bool CompiledDNFLex::PopInputStream(bool pop_all)
{
    // [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：本 TU 路由 4.4.6 -O2，迭代器 16B
    // 与 ORIG 一致（0x2030/0x2034/0x203c 偏移全等，旧 N108「12B vs 16B」不适用）。
    // 实测 ORIG 0x8ad0c80 134 条 vs 我们 172 条，差异三类：
    // ①ORIG 的 `_M_pop_back_aux` 路径 call out-of-line `_ZN14CompiledDNFLex8stream_tD1Ev`
    //   （ORIG nm 为 W 0x8d @0x8ad1320），4.4.6 内联该隐式析构（两棵 COW string 的
    //   引用计数递减，~30 条/处）；用户声明 ~stream_t() 已证更劣（check_dnflex_set.py
    //   :182 注释：会内联 ~deque 进 Pop/D1），属编译器内联决策差；
    // ②prologue：ORIG `sub $0x48,%esp` + 栈槽保存 ebx/esi/edi，我们 push×3+sub 0x3c（N7 族 RA）；
    // ③`cmp %eax,%edx` vs `cmp %edx,%eax` 操作数序（N18b）。
    // 栈空/单元素/多元素三分支、delete[] buffer、pop_back、offset_ 回写语义全等 ⇒ N 封顶。
    // [b28d-solo 复验 2026-09-09] gnu++0x 路由后 fresh .o：134 vs 172，ndiff 83。
    // p167 差集：MISSING `_ZN14CompiledDNFLex8stream_tD1Ev` ×1、EXTRA
    // `_ZNSs4_Rep10_M_destroyERKSaIcE` ×2（=①内联展开的可见后果，语义同）。
    // 试形：`~stream_t() __attribute__((noinline)) {}`（头内）⇒ D1Ev/deque D1Ev 两件
    // 转 DIFF（ae 25→23），已回退 ⇒ 内联决策差不可源码控，维持 N。
    if (streamStack_.empty())
        return false;
    if (!pop_all)
        goto check_size;
do_pop:
    {
        stream_t& top = streamStack_.back();
        if (top.buffer != 0)
            delete[] top.buffer;
        streamStack_.pop_back();
        if (!streamStack_.empty())
            offset_ = streamStack_.back().offset;
        return true;
    }
check_size:
    if (streamStack_.size() > 1)
        goto do_pop;
    return false;
}

// ---- CloseInputStream（ORIG 0x8ad0e90）：反复弹出至空。----
void CompiledDNFLex::CloseInputStream()
{
    while (!streamStack_.empty())
    {
        PopInputStream(true);
    }
}

// ---- PushInputStream（ORIG 0x8ad0a00）----
// buffer 魔数校验（首 short == 0xd0b0；不匹配释放 buffer 并失败）；
// 非空栈先把旧栈顶 offset 保存；push 新流并置 offset_（魔数匹配时跳过
// 2 字节头）。
bool CompiledDNFLex::PushInputStream(const char* dir, const char* filename,
                                     char* buffer, unsigned int bufferSize)
{
    // [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：同 PopInputStream 族。
    // ORIG 0x8ad0a00 170 条 vs 我们 207 条，ndiff 52（b28d-solo 前 219 条/200）。
    // 已修两处（正向收益）：①`dir/filename` 的 assign 由「if(dir==0){assign("",0);}
    //   else{assign(dir,strlen(dir));}」四调用点合并为三目单调用点
    //   `assign(dir==0?"":dir, dir==0?0:strlen(dir))` ⇒ 复现 ORIG 的
    //   `test %edx,%edx; je <空串>; …; call assign` 单点形，p167 EXTRA
    //   `_ZNSs6assignEPKcj`×2 归零；②`stream.size/buffer` 赋值前置到 empty_file
    //   检查之前（ORIG +0x72..+0x77 顺序）。残余：①析构内联 vs ORIG out-of-line
    //   stream_t D1（见 PopInputStream 判词①，p167 MISSING ×1 + EXTRA
    //   `_ZNSs4_Rep10_M_destroyERKSaIcE`×2）；②empty_file=false 的 store 落点
    //   （ORIG 在 `cmpw; je` 目标内，我们在 cmpw 后直落）；③offset_ 回写用
    //   edi vs ORIG 的 edx（寄存器分配）。语义全等 ⇒ N 封顶。
    stream_t stream;
    stream.dir.assign(dir == 0 ? "" : dir,
                      dir == 0 ? static_cast<size_t>(0) : std::strlen(dir));
    stream.filename.assign(filename == 0 ? "" : filename,
                           filename == 0 ? static_cast<size_t>(0)
                                         : std::strlen(filename));

    stream.size = bufferSize;
    stream.buffer = buffer;

    bool empty_file = false;
    if (buffer == 0 || (empty_file = true, bufferSize != 0))
    {
        if (bufferSize < 2)
            return false;
        if (*reinterpret_cast<short*>(buffer) != static_cast<short>(0xd0b0))
        {
            delete[] buffer;
            return false;
        }
        empty_file = false;
    }

    if (!streamStack_.empty())
        streamStack_.back().offset = offset_;

    streamStack_.push_back(stream);

    offset_ = buffer;
    if (!empty_file)
        offset_ = buffer + 2;
    return true;
}

// ---- SwitchInputStream（ORIG 0x8ad0f40）：Close 后尾调 Push。----
bool CompiledDNFLex::SwitchInputStream(const char* dir, const char* filename,
                                       char* buffer, unsigned int bufferSize)
{
    CloseInputStream();
    return PushInputStream(dir, filename, buffer, bufferSize);
}

// ---- SetHandler（ORIG 0x8acfdf0）----
void CompiledDNFLex::SetHandler(TLexerError lexerErrorFunc,
                                TLoadStream loadStreamFunc)
{
    lexerError_ = lexerErrorFunc;
    loadStream_ = loadStreamFunc;
}

// ---- EnableInclusion / DisableInclusion（ORIG 0x8ad0090/0x8ad0080）----
void CompiledDNFLex::EnableInclusion()
{
    flag2040_ = 1;
}

void CompiledDNFLex::DisableInclusion()
{
    flag2040_ = 0;
}

// ---- IsStream（ORIG 0x8acfe80）：栈非空。----
bool CompiledDNFLex::IsStream() const
{
    return !streamStack_.empty();
}

// ---- GetOffset（ORIG 0x8acff20）：--finish 后取 buffer；空/指针 0/buffer 0 → -1。----
int CompiledDNFLex::GetOffset() const
{
    // [b28d-solo-CompiledDNFLexCore 判词·AE] 已修：同 isEnd 的块序旋钮。
    // 原 goto-fail 形被 4.4.6 -O2 反转成 `jne have` 且 fail 块前移；
    // `__builtin_expect(p == begin._M_cur, 0)` + `expect(p->buffer == 0, 0)`
    // 使 fail 块回到 ORIG 0x8acff20 的「主路径 → fail → have → boundary」
    // 顺序（含 xchg 对齐填充），逐条同形。
    stream_t* p = streamStack_.end()._M_cur;
    if (__builtin_expect(p == streamStack_.begin()._M_cur, 0))
        goto fail;
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (p != 0)
            goto have;
    }
fail:
    return -1;
have:
    if (__builtin_expect(p->buffer == 0, 0))
        goto fail;
    return (int)(offset_ - p->buffer);
}

// ---- getStreamName（ORIG 0x8ad0290）：NRVO 槽上 copy dir 再 append(filename)；空/空指针 "NONE"。----
// [b28d-solo-CompiledDNFLexCore 判词·N（残余）/已修 NRVO] 4.4.6 -O2 在「混合 return」
// 下禁用 NRVO（P306 配套负例）。ORIG 0x8ad0290 主路径把 `std::string r(p->dir)` 直接
// 构造在 sret（C1ERKSs this=sret）后 append；两条 "NONE" 路径走临时 RVO（C1EPKc
// this=sret）。本波突破口（新范式，探针 /tmp/probe_src/Game/CompiledDNFLexCore.cpp
// f1..f5 实证）：**单一 return + 具名 result + 三目初始化** 即恢复 NRVO——
//   `bool none=…; std::string result = none ? std::string("NONE") : std::string(p[-1].dir);
//    if (!none) result.append(p[-1].filename); return result;`
// 4.4.6 把两个三目分支各自直接构造到 sret（C1EPKc / C1ERKSs），与 ORIG 逐条同形：
// 83 条→49 条，ndiff 78→33。p 保持 finish 不自减（`p[-1]` 两次独立取址，对齐 ORIG
// 的 `sub $0x14,%eax`/`sub $0x10,%esi` 分算）。残余（跨版本 -O2 寄存器分配/块调度）：
// ①帧 0x38 vs 0x28（ORIG 两个 NONE 块各占一个 allocator 临时槽 -0x9/-0xa，三目合并
// 后仅 -0x9；`char framePad[0x10]` 被 -O2 DCE，无效）；②ORIG `mov %esi,%eax;
// sub $0x14,%eax` 保留 esi=finish、我们直接在 esi 上减（N7 族 RA）；③主路径 epilogue
// 落点。语义等价 ⇒ 残余 N 封顶。
std::string CompiledDNFLex::getStreamName() const
{
    stream_t* p = streamStack_.end()._M_cur;
    bool none = true;
    if (p != streamStack_.begin()._M_cur)
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        if (p - 1 != 0)
            none = false;
    }
    std::string result = none ? std::string("NONE") : std::string(p[-1].dir);
    if (!none)
        result.append(p[-1].filename);
    return result;
}

// ---- isEnd（ORIG 0x8ad0000）：--finish 后 buffer+size <= offset_；空/空指针返回 0。----
bool CompiledDNFLex::isEnd()
{
    // [b28d-solo-CompiledDNFLexCore 判词·AE] 已修：本 TU 路由 4.4.6 -O2，
    // 原形把共享 `return false` 块放函数尾（4.1.2 放在 `_M_first` 边界块之前）。
    // 关键旋钮 = `__builtin_expect(p == begin._M_cur, 0)` 标记失败路径 unlikely，
    // 4.4.6 即把 L_zero 移到 boundary 块之后，与 ORIG 0x8ad0000 逐条同形
    // （含边界块 `mov -0x4(%ecx),%edx; add $0x1f4,%edx; jmp` 的中间位置）。
    stream_t* p = streamStack_.end()._M_cur;
    if (!__builtin_expect(p == streamStack_.begin()._M_cur, 0))
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (__builtin_expect(p == 0, 0))
            return false;
        return p->buffer + p->size <= offset_;
    }
    return false;
}

// ---- LexerError（ORIG 0x8acfe10）----
void CompiledDNFLex::LexerError(const char* msg)
{
    if (lexerError_ != 0)
        lexerError_(msg);
}

// ---- LoadStream（ORIG 0x8acfe40）：转发注册回调。----
bool CompiledDNFLex::LoadStream(const char* dir, const char* filename,
                                DNFLex::stream_data_t* s)
{
    if (loadStream_ == 0)
        return false;
    return loadStream_(dir, filename, s);
}

// ---- getStr(std::string&)（ORIG 0x8ad00d0）：按字符串表索引取串。----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：跨版本 -O2 共享 `return false` 块布局。
// 本 TU 路由 4.4.6 -O2 -std=gnu++0x（dnf_game_cxx.sh: *Game/CompiledDNFLexCore.cpp）。
// ORIG 0x8ad00d0 把 fail 块放检查链中段（`cmp $0x9; jne more` 的 fall-through），
// 4.4.6 -O2 恒放函数尾并把该判断反转为 `je fail`。六形实测均不可翻：
// ①goto fail/more ②短路 && 链 ③独立守卫 ④显式 return+goto ⑤__builtin_expect(0/1)
// ⑥per-function optimize("no-reorder-blocks")（退化为 -O0 形态）。
// 比较序 9/4/0/1 与全部指令逐条相同，仅块位置差 2 处 ⇒ 语义等价，N 封顶。
// [b28d-solo 复验] 32 vs 32，ndiff 7（全 TU 最小残余，仅 fail 块 3 行位置差）。
// 本波补测：`__builtin_expect(d<=1,1)` 只把 fail 块搬到第一守卫后（成 `ja more`
// 反向布局，2 处跳转方向差），`expect(...,0)` 与原形逐条相同 ⇒ 保持自然形。
bool CompiledDNFLex::getStr(std::string& stringBuffer)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d <= 1)
        return false;
    if (tok == DNF_TOK_PAIR_INT)
        return false;
    if (tok == DNF_TOK_FLOAT)
        return false;
    if (tok == DNF_TOK_EOF)
        return false;
    if (tok == DNF_TOK_ERROR)
        return false;
    int index = readIntFromBuffer(offset_);
    return stringManager_->getString(index, &stringBuffer);
}

// ---- getStr(char*, int)（ORIG 0x8ad0190）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：与 getStr(std::string&) 同族。
// ORIG 0x8ad0190 的 fail 块位于 `cmp $0x9; jne more` 的 fall-through（检查链中段），
// 4.4.6 -O2 恒反转为 `je fail` 并置于函数尾；overflow 路径 memcpy 实参装载序亦反
// （ORIG size,dest,src vs 我们 size,src,dest，P75 跨版本调度）。比较序 9/4/0/1 与
// 全部指令逐条相同，仅块位置/装载序差 ⇒ 语义等价，N 封顶（同 getStr 六形实测）。
// [b28d-solo 复验] 66 vs 61，ndiff 22。本波补测：overflow 的 memcpy 改用
// `r_string->c_str()` 内联实参 / 具名 `int n=maxSize-1` 两形均不改装载序
// （仍 size,src,dest；且局部 n 被 -O2 折叠）⇒ 维持 ORIG 形不可控的 P75 判词。
bool CompiledDNFLex::getStr(char* buffer, int maxSize)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d <= 1)
        return false;
    if (tok == DNF_TOK_PAIR_INT)
        return false;
    if (tok == DNF_TOK_FLOAT)
        return false;
    if (tok == DNF_TOK_EOF)
        return false;
    if (tok == DNF_TOK_ERROR)
        return false;
    int index = readIntFromBuffer(offset_);
    const std::string* r_string = stringManager_->getString(index);
    const char* src = r_string->c_str();
    int string_length = (int)r_string->size();
    if (__builtin_expect(string_length > maxSize, 0))
        goto overflow;
    std::memcpy(buffer, src, (unsigned)string_length);
    buffer[string_length] = 0;
    return true;
overflow:
    std::memcpy(buffer, src, (unsigned)(maxSize - 1));
    buffer[string_length - 1] = 0;
    return false;
}

// ---- getStr(bool*)（ORIG 0x8ad0250）：经 stringBuffer_ 缓冲。----
char* CompiledDNFLex::getStr(bool* result)
{
    bool rslt = getStr(stringBuffer_, STRING_BUFFER_SIZE);
    if (result != 0)
        *result = rslt;
    return stringBuffer_;
}

// ---- getInt（ORIG 0x8acff70）：DECIMAL/HEXADECIMAL/PAIR_INT 有效。----
int CompiledDNFLex::getInt(bool* result)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d > 1 && tok != DNF_TOK_PAIR_INT)
    {
        if (result != 0)
            *result = false;
        return 0;
    }
    if (result != 0)
        *result = true;
    static int intBuffer;
    intBuffer = *reinterpret_cast<int*>(offset_);
    offset_ += 4;
    return intBuffer;
}

// ---- getFloat（ORIG 0x8acffc0）：仅 FLOAT 有效。----
float CompiledDNFLex::getFloat(bool* result)
{
    if (lastToken_ != DNF_TOK_FLOAT)
    {
        if (result != 0)
            *result = false;
        return 0.0f;
    }
    if (result != 0)
        *result = true;
    static float floatBuffer;
    floatBuffer = *reinterpret_cast<float*>(offset_);
    offset_ += 4;
    return floatBuffer;
}

// ---- create（ORIG 0x8ad03d0）----
void CompiledDNFLex::create(bool createStringTable)
{
    if (createStringTable)
    {
        if (stringManager_ == 0)
            stringManager_ = new ScriptStringManager();
        stringManagerDelete_ = true;
    }
    if (stringBuffer_ == 0)
        stringBuffer_ = new char[STRING_BUFFER_SIZE];
}

// ---- destroy（ORIG 0x8ad0370）----
void CompiledDNFLex::destroy()
{
    if (stringManagerDelete_)
    {
        ScriptStringManager* p = stringManager_;
        if (p != 0)
        {
            void (**vt)(void*) = *(void (***)(void*))p;
            vt[1](p);
            stringManager_ = 0;
        }
        stringManagerDelete_ = false;
    }
    else
        stringManager_ = 0;
    if (stringBuffer_ != 0)
    {
        ::operator delete[](stringBuffer_);
        stringBuffer_ = 0;
    }
}

// ---- getToken（ORIG 0x8ad0fa0）----
// 主循环：EOF 处理（PopInputStream(false)，失败保持 EOF）；INCLUDE(11) 时
// 读下一个 STRING 记号作文件名，LoadStream+PushInputStream 切入包含文件。
int CompiledDNFLex::getToken()
{
    // [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：本 TU 路由 4.4.6 -O2。
    // 已按 P157/P183 去掉 endIt/beginIt 迭代器缓存，改为逐点
    // `streamStack_.end()._M_cur/_M_first/_M_node` 直读（与 ORIG 0x8ad0fa0 同源形，
    // 头部 8 条现逐条一致）。剩余差异为跨版本 RA/装载调度：①ORIG 在
    // `cmp 0x2034(%ebx),%eax` 后预载 `mov 0x203c(%ebx),%edx`，4.4 延迟到边界块内；
    // ②ecx↔edx 寄存器互换（N7 族）；③PopInputStream 调用后 ORIG `jne`/我们 `je`
    // 的块序（与 getStr 同族）；④析构内联 vs ORIG out-of-line stream_t D1。
    // EOF→PopInputStream(false)→递归、INCLUDE(11)→getStr→LoadStream→PushInputStream→
    // 递归、sprintf/LexerError 四条错误路径语义全等 ⇒ N 封顶。
    // [b28d-solo 复验] 199 vs 200，ndiff 112。p167 差集 **CLEAR**（ORIG 25 calls =
    // 我们 25 calls，0 MISSING / 0 EXTRA）⇒ 语义闭合，残余纯 -O2 RA/块调度
    // （①-④ 同判词），维持 N 封顶。
    stream_t* p = streamStack_.end()._M_cur;
    if (p != streamStack_.begin()._M_cur)
    {
        if (p == streamStack_.end()._M_first)
            p = *(streamStack_.end()._M_node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (p != 0)
        {
            if (offset_ >= p->buffer + p->size)
            {
                lastToken_ = DNF_TOK_EOF;
                if (!PopInputStream(false))
                    return lastToken_;
                goto recurse;
            }
        }
    }

    {
    int c = readCharFromBuffer(offset_);
    lastToken_ = c;
    if (flag2040_ == 0)
        return lastToken_;
    if (c != DNF_TOK_INCLUDE)
        return lastToken_;

    // ORIG 内联计算当前流指针（栈顶 = finish._M_cur - 1），不调 GetInputStream()；
    // 边界节点回退 node[-1]+0x1f4-1，栈空时为 0。
    stream_t* incl = streamStack_.end()._M_cur;
    if (incl != streamStack_.begin()._M_cur)
    {
        if (incl == streamStack_.end()._M_first)
            incl = *(streamStack_.end()._M_node - 1) + STREAM_NODE_ELEMS;
        incl = incl - 1;
    }
    else
    {
        incl = 0;
    }
    int next = readCharFromBuffer(offset_);
    lastToken_ = next;
    if (next != DNF_TOK_STRING)
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s/%s, invalid inclusion.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    std::string include_path;
    if (!getStr(include_path))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, invalid inclusion.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    DNFLex::stream_data_t sd;
    if (!LoadStream("", toTString(include_path), &sd))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, LoadStream failed.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    char* sd_buf = sd.buffer;
    if (sd_buf == 0)
    {
        sd_buf = new char[sd.size];
        reinterpret_cast<std::istream*>(sd.stream)->read(sd_buf, sd.size);
    }

    if (!PushInputStream("", toTString(include_path), sd_buf, sd.size))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, PushInputStream failed.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    if (sd.stream != 0)
    {
        void (**vtable)(void*) =
            *reinterpret_cast<void (***)(void*)>(sd.stream);
        vtable[1](sd.stream);
    }
    }
recurse:
    return getToken();
}

// ---- CompiledDNFLex()（ORIG 0x8ad07c0，默认构造：create(true) 自建串表）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：libstdc++ deque 实现 + 构造调度差。
// 本 TU 路由 4.4.6 -O2；ORIG 0x8ad07c0 118 条 vs 我们 111 条，差异三处：
// ①ORIG 把局部 tmp deque 的构造（清零+_M_initialize_map）**排在成员 streamStack_
//   构造之前**（4.1 -O2 独立调用重排；C++ 语义要求成员先构造，4.4 照语义发射）；
// ②ORIG 的 `streamStack_.swap(tmp)` 带 `test tmp.map; je` 守卫（4.1.2 deque::swap
//   内联形），4.4.6 无守卫直换；
// ③tmp 析构位置：ORIG 在 swap 后立即（源码为块作用域），我们当前在函数尾。
// ①②属跨版本/库实现差异不可翻；③可改块作用域但不改变 DIFF 判定（①仍存）。
// 指令集合与成员偏移（vptr/offset_/lastToken_/stringManagerDelete_/stringBuffer_/
// tokenBuffer_ 0x2000 rep stos/flag2040_/lexerError_/loadStream_/create(true)）全等，
// 语义等价 ⇒ N 封顶。
// [b28d-solo 复验] gnu++0x 下 118 vs 111，ndiff 81（与 gnu++98 逐条相同）。
// 帧 0x6c vs 0x5c（差 0x10，随 tmp 构造先后互换）。①②为 GCC 4.1/4.4 的调用重排与
// libstdc++ deque::swap 内联形差异，源码不可控（成员先构造是 C++ 语义）⇒ 维持 N。
CompiledDNFLex::CompiledDNFLex()
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(0),
      stringBuffer_(0)
{
    std::deque<stream_t> tmp;
    streamStack_.swap(tmp);
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(true);
}

// ---- CompiledDNFLex(ScriptStringManager*)（ORIG 0x8ad0580：create(false)）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：同 C1Ev（libstdc++ deque swap 守卫 +
// 局部 tmp 构造调度在成员 streamStack_ 之前 + tmp 析构位置）。ORIG 0x8ad0580 119 条
// vs 我们 112 条；成员初始化仅差 `stringManager_ = pStringManager`（0x10(%ebx) 直存
// 两形一致），create(false) 一致。语义等价 ⇒ N 封顶。
// [b28d-solo 复验] gnu++0x 下 119 vs 112，ndiff 81。与 C1Ev 同源差异，维持 N。
CompiledDNFLex::CompiledDNFLex(ScriptStringManager* pStringManager)
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(pStringManager),
      stringBuffer_(0)
{
    std::deque<stream_t> tmp;
    streamStack_.swap(tmp);
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(false);
}

// ---- ~CompiledDNFLex（ORIG 0x8ad0ed0 D1：Close → destroy → ~deque）----
CompiledDNFLex::~CompiledDNFLex()
{
    CloseInputStream();
    destroy();
}
