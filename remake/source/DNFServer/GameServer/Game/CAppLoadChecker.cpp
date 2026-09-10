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
{
    m_baseDbq = 0;
    m_baseLogdbq = 0;
    m_baseTimerq = 0;
    m_basePacketq = 0;
    m_stateDbq = 0;
    m_stateLogdbq = 0;
    m_stateTimerq = 0;
    m_statePacketq = 0;
}

#include "CAppLoadChecker/setQueue.inc"
{
    m_baseDbq = dbq;
    m_baseLogdbq = logdbq;
    m_baseTimerq = timerq;
    m_basePacketq = packetq;
}

#include "CAppLoadChecker/checkLoad_dbq.inc"
{
    int up = cur - m_baseDbq;     // ORIG -0x8(%ebp)：param − this->+0x00
    int down = m_baseDbq - cur;   // ORIG -0x4(%ebp)

    if (m_stateDbq <= 0 && up > 10) { m_stateDbq = 1; return true; }
    if (m_stateDbq <= 1 && up > 25) { m_stateDbq = 2; return true; }
    if (m_stateDbq <= 2 && up > 40) { m_stateDbq = 3; return true; }
    if (m_stateDbq <= 3 && up > 80) { m_stateDbq = 4; return true; }
    if (m_stateDbq <= 4 && up > 150) { m_stateDbq = 5; return true; }
    if (m_stateDbq <= 5 && up > 250) { m_stateDbq = 6; return true; }
    if (m_stateDbq == 6 && up > 250) return true;

    if (m_stateDbq >= 0 && down > 10) { m_stateDbq = -1; return true; }
    if (m_stateDbq >= -1 && down > 25) { m_stateDbq = -2; return true; }
    if (m_stateDbq >= -2 && down > 40) { m_stateDbq = -3; return true; }
    if (m_stateDbq >= -3 && down > 80) { m_stateDbq = -4; return true; }
    if (m_stateDbq >= -4 && down > 150) { m_stateDbq = -5; return true; }
    if (m_stateDbq >= -5 && down > 250) { m_stateDbq = -6; return true; }
    if (m_stateDbq == -6 && down > 250) return true;
    return false;
}

#include "CAppLoadChecker/checkLoad_logdbq.inc"
{
    int up = cur - m_baseLogdbq;     // ORIG -0x8(%ebp)：param − this->+0x04
    int down = m_baseLogdbq - cur;   // ORIG -0x4(%ebp)

    if (m_stateLogdbq <= 0 && up > 10) { m_stateLogdbq = 1; return true; }
    if (m_stateLogdbq <= 1 && up > 25) { m_stateLogdbq = 2; return true; }
    if (m_stateLogdbq <= 2 && up > 40) { m_stateLogdbq = 3; return true; }
    if (m_stateLogdbq <= 3 && up > 80) { m_stateLogdbq = 4; return true; }
    if (m_stateLogdbq <= 4 && up > 150) { m_stateLogdbq = 5; return true; }
    if (m_stateLogdbq <= 5 && up > 250) { m_stateLogdbq = 6; return true; }
    if (m_stateLogdbq == 6 && up > 250) return true;

    if (m_stateLogdbq >= 0 && down > 10) { m_stateLogdbq = -1; return true; }
    if (m_stateLogdbq >= -1 && down > 25) { m_stateLogdbq = -2; return true; }
    if (m_stateLogdbq >= -2 && down > 40) { m_stateLogdbq = -3; return true; }
    if (m_stateLogdbq >= -3 && down > 80) { m_stateLogdbq = -4; return true; }
    if (m_stateLogdbq >= -4 && down > 150) { m_stateLogdbq = -5; return true; }
    if (m_stateLogdbq >= -5 && down > 250) { m_stateLogdbq = -6; return true; }
    if (m_stateLogdbq == -6 && down > 250) return true;
    return false;
}

#include "CAppLoadChecker/checkLoad_timerq.inc"
{
    int up = cur - m_baseTimerq;     // ORIG -0x8(%ebp)：param − this->+0x08
    int down = m_baseTimerq - cur;   // ORIG -0x4(%ebp)

    if (m_stateTimerq <= 0 && up > 10) { m_stateTimerq = 1; return true; }
    if (m_stateTimerq <= 1 && up > 25) { m_stateTimerq = 2; return true; }
    if (m_stateTimerq <= 2 && up > 40) { m_stateTimerq = 3; return true; }
    if (m_stateTimerq <= 3 && up > 80) { m_stateTimerq = 4; return true; }
    if (m_stateTimerq <= 4 && up > 150) { m_stateTimerq = 5; return true; }
    if (m_stateTimerq <= 5 && up > 250) { m_stateTimerq = 6; return true; }
    if (m_stateTimerq == 6 && up > 250) return true;

    if (m_stateTimerq >= 0 && down > 10) { m_stateTimerq = -1; return true; }
    if (m_stateTimerq >= -1 && down > 25) { m_stateTimerq = -2; return true; }
    if (m_stateTimerq >= -2 && down > 40) { m_stateTimerq = -3; return true; }
    if (m_stateTimerq >= -3 && down > 80) { m_stateTimerq = -4; return true; }
    if (m_stateTimerq >= -4 && down > 150) { m_stateTimerq = -5; return true; }
    if (m_stateTimerq >= -5 && down > 250) { m_stateTimerq = -6; return true; }
    if (m_stateTimerq == -6 && down > 250) return true;
    return false;
}

#include "CAppLoadChecker/checkLoad_packetq.inc"
{
    int up = cur - m_basePacketq;     // ORIG -0x8(%ebp)：param − this->+0x0c
    int down = m_basePacketq - cur;   // ORIG -0x4(%ebp)

    if (m_statePacketq <= 0 && up > 10) { m_statePacketq = 1; return true; }
    if (m_statePacketq <= 1 && up > 25) { m_statePacketq = 2; return true; }
    if (m_statePacketq <= 2 && up > 40) { m_statePacketq = 3; return true; }
    if (m_statePacketq <= 3 && up > 80) { m_statePacketq = 4; return true; }
    if (m_statePacketq <= 4 && up > 150) { m_statePacketq = 5; return true; }
    if (m_statePacketq <= 5 && up > 250) { m_statePacketq = 6; return true; }
    if (m_statePacketq == 6 && up > 250) return true;

    if (m_statePacketq >= 0 && down > 10) { m_statePacketq = -1; return true; }
    if (m_statePacketq >= -1 && down > 25) { m_statePacketq = -2; return true; }
    if (m_statePacketq >= -2 && down > 40) { m_statePacketq = -3; return true; }
    if (m_statePacketq >= -3 && down > 80) { m_statePacketq = -4; return true; }
    if (m_statePacketq >= -4 && down > 150) { m_statePacketq = -5; return true; }
    if (m_statePacketq >= -5 && down > 250) { m_statePacketq = -6; return true; }
    if (m_statePacketq == -6 && down > 250) return true;
    return false;
}

#include "CAppLoadChecker/CheckLoad.inc"
{
    bool dbqOver = checkLoad_dbq(dbq);
    bool logdbqOver = checkLoad_logdbq(logdbq);
    bool timerqOver = checkLoad_timerq(timerq);
    bool packetqOver = checkLoad_packetq(packetq);

    if (dbqOver || logdbqOver || timerqOver || packetqOver)
    {
        setQueue(dbq, logdbq, timerq, packetq);
        return true;
    }
    return false;
}

#include "CAppLoadChecker/RequestDB.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ServerLoadChecker.cpp", 0x1b5),
        true);

    (**guard) << 0x183;
    (**guard) << -1;

    SigInsertServerLoadStatisticView* view =
        reinterpret_cast<SigInsertServerLoadStatisticView*>(
            guard->GetInBuffer<SIG_INSERT_SERVER_LOAD_STATISTIC>());
    view->m_serverGroup =
        static_cast<unsigned char>(G_CEnvironment()->get_server_group());
    view->m_channelNo =
        static_cast<unsigned char>(G_CEnvironment()->get_channel_no());
    view->m_dbq = static_cast<short>(dbq);
    view->m_logdbq = static_cast<short>(logdbq);
    view->m_timerq = static_cast<short>(timerq);
    view->m_packetq = static_cast<short>(packetq);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

