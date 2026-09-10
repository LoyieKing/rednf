// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ScanJobStr/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <string>

#include "DNFLexWrapper.h"
#include "RDARScriptStringManager.h"
#include "SyncScriptDeps.h"

// [b28d-11b] 修法：去具名 p 局部（P183/P172 完整下标重算）+ if-else 链改 switch（P251 载入寄存器 cmp 链）→ IDENTICAL 117 @0x08a6143f
#include "ScanJobStr/ScanJobStr.inc"
{
    bool found = false;
    int token = g_dnfLex.getToken();
    switch (token)
    {
    case DNF_TOK_STRING:
        *jobName = toTString(g_dnfLex.getString());
        found = true;
        break;
    case DNF_TOK_PAIR_INT:
    {
        int key = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
        {
            *jobName = g_scriptStringManager_.findString(
                key, g_dnfLex.getPairStr(), 0);
            found = true;
        }
        if (out != 0)
        {
            for (int i = 0; i <= 10; ++i)
            {
                char buf[0x400];
                std::snprintf(buf, sizeof(buf), "%s_%d",
                              g_dnfLex.getPairStr(), i);
                bool ok = false;
                char const* s = g_scriptStringManager_.findString(
                    key, buf, &ok);
                if (s != 0 && ok)
                {
                    out[i] = new std::string();
                    *out[i] = s;
                }
            }
        }
        break;
    }
    }
    return found;
}
