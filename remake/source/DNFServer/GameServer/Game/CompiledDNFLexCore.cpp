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

#include "CompiledDNFLexCore/getStringManager.inc"


#include "CompiledDNFLexCore/setStringManager.inc"


#include "CompiledDNFLexCore/GetInputStream.inc"


#include "CompiledDNFLexCore/GetInputStream_1.inc"


#include "CompiledDNFLexCore/PopInputStream.inc"


#include "CompiledDNFLexCore/CloseInputStream.inc"


#include "CompiledDNFLexCore/PushInputStream.inc"


#include "CompiledDNFLexCore/SwitchInputStream.inc"


#include "CompiledDNFLexCore/SetHandler.inc"


#include "CompiledDNFLexCore/EnableInclusion.inc"


#include "CompiledDNFLexCore/DisableInclusion.inc"


#include "CompiledDNFLexCore/IsStream.inc"


#include "CompiledDNFLexCore/GetOffset.inc"


#include "CompiledDNFLexCore/getStreamName.inc"


#include "CompiledDNFLexCore/isEnd.inc"


#include "CompiledDNFLexCore/LexerError.inc"


#include "CompiledDNFLexCore/LoadStream.inc"


#include "CompiledDNFLexCore/getStr.inc"


#include "CompiledDNFLexCore/getStr_1.inc"


#include "CompiledDNFLexCore/getStr_2.inc"


#include "CompiledDNFLexCore/getInt.inc"


#include "CompiledDNFLexCore/getFloat.inc"


#include "CompiledDNFLexCore/create.inc"


#include "CompiledDNFLexCore/destroy.inc"


#include "CompiledDNFLexCore/getToken.inc"


#include "CompiledDNFLexCore/CompiledDNFLex.inc"


#include "CompiledDNFLexCore/CompiledDNFLex_1.inc"


#include "CompiledDNFLexCore/dtor_CompiledDNFLex.inc"

