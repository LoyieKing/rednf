// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSwitchLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "CLog.h"
#include "CSwitchLog.h"   // 权威声明（本 TU 唯一实现）

char CSwitchLog::log_write_[1];   // ORIG 0x0943df78，1 字节
#include "CSwitchLog/CSwitchLog.inc"
{
    m_name = name;
    m_line = line;
    m_level = level;
    m_flagIndex = flagIndex;
}

#include "CSwitchLog/SetLogWriteFlag.inc"
{
    switch (flag)
    {
    case 0:
        log_write_[0] = 0;
        break;
    case 1:
        log_write_[0] = 1;
        break;
    }
}

// [b28d-23 N·N82] ⇒ N。ORIG 0x0854fd64
#include "CSwitchLog/operator.inc"
{
    if (log_write_[m_flagIndex]) {
        char buf[4096];
        sprintf(buf, "%s(%d): ", m_name, m_line);
        va_list ap;
        va_start(ap, fmt);
        int len = strlen(buf);
        vsnprintf(buf + len, 0xfff - len, fmt, ap);
        // [L23-r4 P0/P183] ORIG 槽表无 log 具名槽（我方多 `mov %eax,-0x14(%ebp)`
        //   使 buf 基址由 -0x1014 漂到 -0x1018）⇒ instance() 结果实参位直传。
        Guard<Mutex> g((Mutex*)((char*)CLog::instance() + m_level * 0x18));
        switch (m_level) {
        case 0:
            CLog::instance()->logNotice(buf);
            break;
        case 1:
            CLog::instance()->logError(buf);
            break;
        case 2:
            CLog::instance()->logDebug(buf);
            break;
        case 3:
            CLog::instance()->logConsole(buf);
            break;
        case 4:
            CLog::instance()->logMoney(buf);
            break;
        case 5:
            CLog::instance()->logCritical(buf);
            break;
        }
    }
}
