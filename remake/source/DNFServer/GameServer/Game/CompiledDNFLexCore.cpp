// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CompiledDNFLexCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "CompiledDNFLexCore/getStringManager.inc"
}

// ---- setStringManager（ORIG 0x8ad0050）----
#include "CompiledDNFLexCore/setStringManager.inc"
}

// ---- GetInputStream（ORIG 0x8acfea0）：栈空返回 0，否则栈顶流。----
#include "CompiledDNFLexCore/GetInputStream.inc"
}

#include "CompiledDNFLexCore/GetInputStream_1.inc"
}

// ---- PopInputStream（ORIG 0x8ad0c80）----
// 栈空返回 false；!pop_all 且元素数 <2 返回 false（保留最底层流）。
// 否则弹出一个：释放 buffer、析构 dir/filename、pop_back；弹出后若栈仍非空，
// offset_ = 新栈顶 offset。返回 true。
#include "CompiledDNFLexCore/PopInputStream.inc"
}

// ---- CloseInputStream（ORIG 0x8ad0e90）：反复弹出至空。----
#include "CompiledDNFLexCore/CloseInputStream.inc"
}

// ---- PushInputStream（ORIG 0x8ad0a00）----
// buffer 魔数校验（首 short == 0xd0b0；不匹配释放 buffer 并失败）；
// 非空栈先把旧栈顶 offset 保存；push 新流并置 offset_（魔数匹配时跳过
// 2 字节头）。
#include "CompiledDNFLexCore/PushInputStream.inc"
}

// ---- SwitchInputStream（ORIG 0x8ad0f40）：Close 后尾调 Push。----
#include "CompiledDNFLexCore/SwitchInputStream.inc"
}

// ---- SetHandler（ORIG 0x8acfdf0）----
#include "CompiledDNFLexCore/SetHandler.inc"
}

// ---- EnableInclusion / DisableInclusion（ORIG 0x8ad0090/0x8ad0080）----
#include "CompiledDNFLexCore/EnableInclusion.inc"
}

#include "CompiledDNFLexCore/DisableInclusion.inc"
}

// ---- IsStream（ORIG 0x8acfe80）：栈非空。----
#include "CompiledDNFLexCore/IsStream.inc"
}

// ---- GetOffset（ORIG 0x8acff20）：--finish 后取 buffer；空/指针 0/buffer 0 → -1。----
#include "CompiledDNFLexCore/GetOffset.inc"
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
}

// ---- isEnd（ORIG 0x8ad0000）：--finish 后 buffer+size <= offset_；空/空指针返回 0。----
#include "CompiledDNFLexCore/isEnd.inc"
}

// ---- LexerError（ORIG 0x8acfe10）----
#include "CompiledDNFLexCore/LexerError.inc"
}

// ---- LoadStream（ORIG 0x8acfe40）：转发注册回调。----
#include "CompiledDNFLexCore/LoadStream.inc"
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
}

// ---- getStr(bool*)（ORIG 0x8ad0250）：经 stringBuffer_ 缓冲。----
#include "CompiledDNFLexCore/getStr_2.inc"
}

// ---- getInt（ORIG 0x8acff70）：DECIMAL/HEXADECIMAL/PAIR_INT 有效。----
#include "CompiledDNFLexCore/getInt.inc"
}

// ---- getFloat（ORIG 0x8acffc0）：仅 FLOAT 有效。----
#include "CompiledDNFLexCore/getFloat.inc"
}

// ---- create（ORIG 0x8ad03d0）----
#include "CompiledDNFLexCore/create.inc"
}

// ---- destroy（ORIG 0x8ad0370）----
#include "CompiledDNFLexCore/destroy.inc"
}

// ---- getToken（ORIG 0x8ad0fa0）----
// 主循环：EOF 处理（PopInputStream(false)，失败保持 EOF）；INCLUDE(11) 时
// 读下一个 STRING 记号作文件名，LoadStream+PushInputStream 切入包含文件。
#include "CompiledDNFLexCore/getToken.inc"
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
}

// ---- CompiledDNFLex(ScriptStringManager*)（ORIG 0x8ad0580：create(false)）----
// [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：同 C1Ev（libstdc++ deque swap 守卫 +
// 局部 tmp 构造调度在成员 streamStack_ 之前 + tmp 析构位置）。ORIG 0x8ad0580 119 条
// vs 我们 112 条；成员初始化仅差 `stringManager_ = pStringManager`（0x10(%ebx) 直存
// 两形一致），create(false) 一致。语义等价 ⇒ N 封顶。
// [b28d-solo 复验] gnu++0x 下 119 vs 112，ndiff 81。与 C1Ev 同源差异，维持 N。
#include "CompiledDNFLexCore/CompiledDNFLex_1.inc"
}

// ---- ~CompiledDNFLex（ORIG 0x8ad0ed0 D1：Close → destroy → ~deque）----
#include "CompiledDNFLexCore/dtor_CompiledDNFLex.inc"
}
