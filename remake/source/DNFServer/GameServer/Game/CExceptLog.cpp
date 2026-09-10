// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CExceptLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10CExceptLog13PrintTextFileEPcS0_`（ORIG 0x846d7e5）；键名符号为
// PrintTextFile（非 ctor），待移序为 TU 首强符号。
#include <iostream>

#include "CExceptLog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x0846d640。
#include "CExceptLog/PrintTextFile.inc"
{
    FILE* stream = fopen(name, "a");
    if (stream != NULL)
    {
        fprintf(stream, content);
        fclose(stream);
    }
}

// ORIG ctor @0x0846d736：memset(this,0,2000) → Set_Time_String(buf) →
// fPrintTextFile("Except.txt","%s\n%s\n",buf,str.c_str())。
#include "CExceptLog/CExceptLog.inc"
{
    memset((char*)this, 0, 2000);

    char buf[128];
    Set_Time_String(buf);
    fPrintTextFile("Except.txt", "%s\n%s\n", buf, str.c_str());
}

// ORIG @0x0846d682：vsprintf 写入 (char*)this，再 PrintTextFile(name,(char*)this)。
#include "CExceptLog/fPrintTextFile.inc"
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(reinterpret_cast<char*>(this), fmt, ap);
    va_end(ap);

    PrintTextFile(name, reinterpret_cast<char*>(this));
}

// ORIG @0x0846d6c2：time(&t) → localtime_r(&t,&tm) → sprintf 时间串。
#include "CExceptLog/Set_Time_String.inc"
{
    struct tm tm;
    time_t t;

    time(&t);
    localtime_r(&t, &tm);
    sprintf(buf, "%4d.%02d.%02d %02d:%02d:%02d",
            tm.tm_year + 0x76c, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}
