// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAppLoadChecker/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit + 把键名符号
// （CAppLoadCheckerInstance，ORIG 0x86006ec）定义前移为本 TU 首强符号。
#include <iostream>

#include "CAppLoadChecker.h"

#include "CEnvironment.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"

namespace {

// SIG_INSERT_SERVER_LOAD_STATISTIC 视图（SigTypes.h 权威类型为 m_pad[0x100]
// 占位）。ORIG RequestDB 写入布局：+0/+1 byte、+2/+4/+6/+8 short。
struct SigInsertServerLoadStatisticView
{
    unsigned char m_serverGroup;  // +0x00
    unsigned char m_channelNo;    // +0x01
    short m_dbq;                  // +0x02
    short m_logdbq;               // +0x04
    short m_timerq;               // +0x06
    short m_packetq;              // +0x08
};

}  // namespace

// [GI-ioinit L21] 本 TU 首个强外部符号定义（_GLOBAL__I_ 键名来源，P327）。
CAppLoadChecker* CAppLoadCheckerInstance()
{
    static CAppLoadChecker instance;   // ORIG _ZZ23...E8instance @0x9473208
    return &instance;
}

#include "CAppLoadChecker/CAppLoadChecker.inc"


#include "CAppLoadChecker/setQueue.inc"


#include "CAppLoadChecker/checkLoad_dbq.inc"


#include "CAppLoadChecker/checkLoad_logdbq.inc"


#include "CAppLoadChecker/checkLoad_timerq.inc"


#include "CAppLoadChecker/checkLoad_packetq.inc"


#include "CAppLoadChecker/CheckLoad.inc"


#include "CAppLoadChecker/RequestDB.inc"


