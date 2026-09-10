// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CompiledDNFLexCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// [b28d-G11 链接性修正] ORIG nm 直证三个变量均为 **V**（unique/外部链接），
// 且符号名**不含 `L`**（`_ZZ17readIntFromBufferRPcE9intBuffer`）⇒ 原码 helper 是
// 外部链接的 `inline` 函数（非 static）；写成 static 会得到 `_ZZL17…`（b, local）
// 且 -O2 把函数级 static 折成栈临时（getStr ORIG `mov %eax,0x9501a10` vs 我方
// `mov %eax,0xc`）。helper 全内联 ⇒ 不发射 `_Z17readIntFromBufferRPc` weak 定义
// （ORIG 符号表亦无该函数符号）。C++98 合法、mangled 不变。
inline int readIntFromBuffer(char*& offset)
{
    static int intBuffer;
    intBuffer = *reinterpret_cast<int*>(offset);
    offset += 4;
    return intBuffer;
}

// [b28d-G11] 同 readIntFromBuffer：ORIG 变量符号 `_ZZ18readCharFromBufferRPcE10charBuffer`
// 为 **V**（无 L）⇒ 原码 helper 是 extern-inline；改回 inline（旧 static 判断作废）。
inline int readCharFromBuffer(char*& offset)
{
    static char charBuffer;
    signed char c = *reinterpret_cast<signed char*>(offset);
    ++offset;
    charBuffer = c;
    return c;
}

// [b28d-G11] 同上：ORIG `_ZZ19readFloatFromBufferRPcE11floatBuffer` 为 **V**（无 L）。
inline float readFloatFromBuffer(char*& offset)
{
    static float floatBuffer;
    floatBuffer = *reinterpret_cast<float*>(offset);
    offset += 4;
    return floatBuffer;
}

// ---- getStringManager（ORIG 0x8ad0070）：返回 stringManager_。----
#include "CompiledDNFLexCore/getStringManager.inc"
{
    return stringManager_;
}

// ---- setStringManager（ORIG 0x8ad0050）----
#include "CompiledDNFLexCore/setStringManager.inc"
                                      bool allowOwner)
{
    stringManagerDelete_ = allowOwner;
    stringManager_ = pStringManager;
}

// ---- GetInputStream（ORIG 0x8acfea0）：栈空返回 0，否则栈顶流。----
#include "CompiledDNFLexCore/GetInputStream.inc"
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}

#include "CompiledDNFLexCore/GetInputStream_1.inc"
{
    if (streamStack_.empty())
        return 0;
    return &streamStack_.back();
}

// ---- PopInputStream（ORIG 0x8ad0c80）----
// 栈空返回 false；!pop_all 且元素数 <2 返回 false（保留最底层流）。
// 否则弹出一个：释放 buffer、析构 dir/filename、pop_back；弹出后若栈仍非空，
// offset_ = 新栈顶 offset。返回 true。
#include "CompiledDNFLexCore/PopInputStream.inc"
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
#include "CompiledDNFLexCore/CloseInputStream.inc"
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
#include "CompiledDNFLexCore/PushInputStream.inc"
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
#include "CompiledDNFLexCore/SwitchInputStream.inc"
                                       char* buffer, unsigned int bufferSize)
{
    CloseInputStream();
    return PushInputStream(dir, filename, buffer, bufferSize);
}

// ---- SetHandler（ORIG 0x8acfdf0）----
#include "CompiledDNFLexCore/SetHandler.inc"
                                TLoadStream loadStreamFunc)
{
    lexerError_ = lexerErrorFunc;
    loadStream_ = loadStreamFunc;
}

// ---- EnableInclusion / DisableInclusion（ORIG 0x8ad0090/0x8ad0080）----
#include "CompiledDNFLexCore/EnableInclusion.inc"
{
    flag2040_ = 1;
}

#include "CompiledDNFLexCore/DisableInclusion.inc"
{
    flag2040_ = 0;
}

// ---- IsStream（ORIG 0x8acfe80）：栈非空。----
#include "CompiledDNFLexCore/IsStream.inc"
{
    return !streamStack_.empty();
}

// ---- GetOffset（ORIG 0x8acff20）：--finish 后取 buffer；空/指针 0/buffer 0 → -1。----
#include "CompiledDNFLexCore/GetOffset.inc"
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
#include "CompiledDNFLexCore/getStreamName.inc"
{
    // [G11-LaneCompiledDNFLexCore 2026-09-10 试形·已回滚] 曾按 ORIG 0x8ad0290 的
    // 两处独立 `_ZNSsC1EPKcRKSaIcE`（08ad0312 用 allocator 槽 -0xa、08ad033a 用 -0x9，
    // 各自出口 08ad0322/08ad033f）改写为「两条 return std::string("NONE") 早返回 +
    // 去具名 node」⇒ fresh 读数 **81 条 vs 原 49 条（ORIG 60）严重劣化** ⇒ 回滚，
    // 维持 `bool none` + 三元合并原形（N108/deque 族：4.4 恒内联 deque 访问器）。
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
#include "CompiledDNFLexCore/isEnd.inc"
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
#include "CompiledDNFLexCore/LexerError.inc"
{
    if (lexerError_ != 0)
        lexerError_(msg);
}

// ---- LoadStream（ORIG 0x8acfe40）：转发注册回调。----
#include "CompiledDNFLexCore/LoadStream.inc"
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
#include "CompiledDNFLexCore/getStr.inc"
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    // [b28d-G11 负结论] ORIG 08ad00e2/08ad00e7：`jbe 08ad00e9` + `jne 08ad00f0`
    // ⇒ 共享 return-false 块 08ad00e9 物理紧跟第 2 判定、body 在 +0x20；
    // 4.4.6 -O2 恒把该块排到函数尾（全 `je` 形）。实测三形同形（fresh .o 32/32、
    // diff 8 行恒定）：① 平铺独立守卫；② `__builtin_expect(tok==PAIR_INT,0)`
    // 与 `__builtin_expect(d<=1,0)`（P306 isEnd 法）；③ 正条件包裹 /
    // `d<=1 || tok==PAIR_INT` 合并短路。⇒ 块布局由后端 reorder 决定，N 封顶。
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
#include "CompiledDNFLexCore/getStr_1.inc"
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
#include "CompiledDNFLexCore/getStr_2.inc"
{
    bool rslt = getStr(stringBuffer_, STRING_BUFFER_SIZE);
    if (result != 0)
        *result = rslt;
    return stringBuffer_;
}

// ---- getInt（ORIG 0x8acff70）：DECIMAL/HEXADECIMAL/PAIR_INT 有效。----
#include "CompiledDNFLexCore/getInt.inc"
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
    // [b28d-G11 直证] ORIG 08acffac 尾段 `mov %eax,&_ZZ17readIntFromBufferRPcE9intBuffer`
    // = helper 内联尾调用 ⇒ 原码就是 `return readIntFromBuffer(offset_);`
    // （旧自写 static intBuffer 既造出发明符号 getInt()::intBuffer, ORIG 无，
    //  又把 buffer 折成栈临时 `mov %eax,0x8` 而非 ORIG 的全局 unique 槽）。
    return readIntFromBuffer(offset_);
}

// ---- getFloat（ORIG 0x8acffc0）：仅 FLOAT 有效。----
#include "CompiledDNFLexCore/getFloat.inc"
{
    if (lastToken_ != DNF_TOK_FLOAT)
    {
        if (result != 0)
            *result = false;
        return 0.0f;
    }
    if (result != 0)
        *result = true;
    // [b28d-G11 直证] 同 getInt：ORIG 符号表只有 helper 的
    // `_ZZ19readFloatFromBufferRPcE11floatBuffer`（V），无 getFloat()::floatBuffer
    // ⇒ 原码为 `return readFloatFromBuffer(offset_);`。
    return readFloatFromBuffer(offset_);
}

// ---- create（ORIG 0x8ad03d0）----
#include "CompiledDNFLexCore/create.inc"
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
#include "CompiledDNFLexCore/destroy.inc"
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
#include "CompiledDNFLexCore/getToken.inc"
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
#include "CompiledDNFLexCore/CompiledDNFLex.inc"
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(0),
      stringBuffer_(0)
{
    // ORIG 08ad073e/08ad0741（`mov %esi,(%esp)` + `call <_ZNSt5dequeI…stream_t…D1Ev>`）
    // 位于 08ad0746（tokenBuffer_ memset 准备）之前 ⇒ tmp 的来源作用域在 swap 后即结束
    // （析构提前到 memset/create 之前）。块作用域复现该提前析构位置。
    {
        std::deque<stream_t> tmp;
        streamStack_.swap(tmp);
    }
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
#include "CompiledDNFLexCore/CompiledDNFLex_1.inc"
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(pStringManager),
      stringBuffer_(0)
{
    // ORIG 08ad073e/08ad0741（`mov %esi,(%esp)` + `call <_ZNSt5dequeI…stream_t…D1Ev>`）
    // 位于 08ad0746（tokenBuffer_ memset 准备）之前 ⇒ tmp 的来源作用域在 swap 后即结束
    // （析构提前到 memset/create 之前）。块作用域复现该提前析构位置。
    {
        std::deque<stream_t> tmp;
        streamStack_.swap(tmp);
    }
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(false);
}

// ---- ~CompiledDNFLex（ORIG 0x8ad0ed0 D1：Close → destroy → ~deque）----
#include "CompiledDNFLexCore/dtor_CompiledDNFLex.inc"
{
    CloseInputStream();
    destroy();
}
