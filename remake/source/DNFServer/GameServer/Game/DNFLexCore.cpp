// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFLexCore/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <string>
#include <iostream>

#include "DNFLex.h"
#include "SyncScriptDeps.h"

static int dnf_yylex(DNFLex* self)
{
    void** vptr = *reinterpret_cast<void***>(self);
    typedef int (*Fn)(DNFLex*);
    return ((Fn)vptr[0x18 / sizeof(void*)])(self);
}

#include "DNFLexCore/clear.inc"


#include "DNFLexCore/GetInputStream.inc"


#include "DNFLexCore/getStreamName.inc"


#include "DNFLexCore/CloseInputStream.inc"


#include "DNFLexCore/destroy.inc"


#include "DNFLexCore/PopInputStream.inc"


#include "DNFLexCore/LoadStream.inc"


#include "DNFLexCore/PushInputStream.inc"


#include "DNFLexCore/SwitchInputStream.inc"


#include "DNFLexCore/SetHandler.inc"


#include "DNFLexCore/LexerOutput.inc"


#include "DNFLexCore/LexerError.inc"


#include "DNFLexCore/EnableInclusion.inc"


#include "DNFLexCore/DisableInclusion.inc"


#include "DNFLexCore/IsStream.inc"


#include "DNFLexCore/tok_str.inc"


#include "DNFLexCore/tok_str_len.inc"


#include "DNFLexCore/getLineNumber.inc"


#include "DNFLexCore/get_next_token.inc"


#include "DNFLexCore/DNFLex.inc"


#include "DNFLexCore/dtor_DNFLex.inc"

