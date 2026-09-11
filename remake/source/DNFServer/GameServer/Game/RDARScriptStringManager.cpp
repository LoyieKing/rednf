// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RDARScriptStringManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "RDARScriptStringManager.h"

#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cwchar>

#include "DNFFunctionLib.h"   // CodePage::script2Database
#include "LogManager.h"       // LogManager::logFormat
#include "SyncScriptDeps.h"   // toTString(const char*) / toTString(const wchar_t*)
#include "StringTypes.h"      // toUnicode（G4 域；返回 TLS 环形缓冲槽指针）

// ----------------------------------------------------------------------------
// ORIG 静态 _ZL15parseStringLinePcPS_S0_ 0x8aa6551（本 TU 文件局部）。
// 语义（objdump 0x8aa6551）：
//   1) 跳过行首空白（0x0d/0x0a/0x20/0x09）；跳空过程中遇 CR/LF → 返回 0（空行）。
//   2) 若行首为 NUL 或 "//"（注释）→ 返回 0。
//   3) 扫描查找分隔符 '>'（0x3e）：找到则置 NUL、后移、记录 value 起始；未找到 → 返回 -1。
//   4) keyLen==1（无有效 key）→ 返回 -1；keyLen-1 > 0x28（41）→ 返回 -2。
//   5) 解析 value：遇 CR/LF → 置 NUL 结束；遇 "\\n" → 替换为真实 '\n' 并前移；
//   6) *out_key = keyStart; *out_value = valueStart; 返回 0。
// 返回：0=成功（或空/注释行，此时出参不变）、-1=无分隔符或 key 过短、-2=key 过长。
// [M2S5StringRaw 2026-09-11] 三处「我方多」冗余修正（探针 /tmp/probe/Game/RDARScriptStringManager.cpp
// 4.4.6 路由复刻，新形逐条对齐 ORIG 仅余 1 条对齐 nop；旧形 -12 条）：
//   ① ORIG 0x8aa65af `je` 与 0x8aa65c6 `jne` 同指 return-0 岛 0x8aa65c8
//      ⇒ 空行/注释两条件合并为一个短路或；
//   ② ORIG 0x8aa6614 `je` 与 0x8aa6625 `jne` 同指 -1 岛 0x8aa6627
//      ⇒ valueStart==0 与 keyLen==1 合并（旧形发两份 `mov $-1; jmp`）；
//   ③ ORIG tail 左移为单指针 q（-0x4 槽，`*q = q[1]` 含 NUL 一并搬迁）
//      ⇒ 无 dst 局部、无显式补 NUL；且 ORIG 无 `p = valueStart` 重载
//      （0x8aa6643 jle 直跳循环条件 0x8aa66b8，p 槽沿用 break 时值）。
// ----------------------------------------------------------------------------
static int parseStringLine(char* line, char** outKey, char** outValue)
{
    char* p = line;

    // 1) 跳过行首空白；遇换行 → 空行
    // ORIG 0x8aa6551：循环条件在底部（`jmp cond` 入口 + `test;setne;jne`）
    // ⇒ 源码为 `while (*p)`（非 for(;;) + 显式 NUL break）。
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
            return 0;
        if (*p == ' ' || *p == '\t')
        {
            ++p;
            continue;
        }
        break;
    }

    // 2) 空行或注释（两条件共享 ORIG return-0 岛 0x8aa65c8，合并短路或）
    if (*p == '\0' || (p[0] == '/' && p[1] == '/'))
        return 0;

    // 3) 找 '>' 分隔符
    char* keyStart = p;
    char* valueStart = 0;
    while (*p != '\0')
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            valueStart = p;
            break;
        }
        ++p;
    }

    // 4) 无分隔符或 key 无效（共享 ORIG -1 岛 0x8aa6627）
    if (valueStart == 0 || (valueStart - keyStart) == 1)
        return -1;
    if ((valueStart - keyStart - 1) > 0x28)
        return -2;

    // 5) 解析 value：换行结束 / "\\n" 转义。
    // 不写 `p = valueStart`：key 扫描 break 时 p 恰为 valueStart（ORIG
    // 0x8aa6643 `jle 0x8aa66b8` 直跳循环条件，p 槽未重载）。
    while (*p != '\0')
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        if (*p == '\\' && *(p + 1) == 'n')
        {
            *p = '\n';
            // 单指针就地前移（ORIG 0x8aa6688..0x8aa66b2：q(-0x4)=p+1，
            // `*q = q[1]` 循环至 *q==0 ⇒ NUL 一并搬迁结束，无显式补 NUL）。
            for (char* q = p + 1; *q != '\0'; ++q)
                *q = *(q + 1);
        }
        ++p;
    }

    // 6) 出参
    *outKey = keyStart;
    *outValue = valueStart;
    return 0;
}

// ORIG 字符串表函数用 ss_sprintf<256>（安全限长）而非裸 sprintf：
// 模板定义与 OnlyOrigBatch12.cpp 一致（extern ss_vsprintf，直接传变参起始地址）。
extern int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList);
template <size_t N>
#include "RDARScriptStringManager/ss_sprintf.inc"


#include "RDARScriptStringManager/RDARScriptStringManager.inc"


#include "RDARScriptStringManager/dtor_RDARScriptStringManager.inc"


#include "RDARScriptStringManager/findString.inc"


#include "RDARScriptStringManager/findString_1.inc"


#include "RDARScriptStringManager/getKeyIndex.inc"


#include "RDARScriptStringManager/getTagByName.inc"


#include "RDARScriptStringManager/getTagByName_1.inc"


#include "RDARScriptStringManager/setTableValue.inc"


#include "RDARScriptStringManager/loadStringFile.inc"


#include "RDARScriptStringManager/writeStringFile.inc"


#include "RDARScriptStringManager/getTagSizeInFile.inc"


#include "RDARScriptStringManager/findChangedString.inc"


#include "RDARScriptStringManager/getStringFileList.inc"


#include "RDARScriptStringManager/changeSpacilTagStr.inc"


#include "RDARScriptStringManager/release.inc"


RDARScriptStringManager g_scriptStringManager_;