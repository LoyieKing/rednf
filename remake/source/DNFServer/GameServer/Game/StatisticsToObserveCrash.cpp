// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StatisticsToObserveCrash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsToObserveCrash
{
public:
    StatisticsToObserveCrash();
    ~StatisticsToObserveCrash();

    void reset();
    int getAbnormalDownUserCount();
    void setAbnormalDownUserCount(int count);
    void incAbnormalDownUserCount();
    void incUserLastCrashPacketID(int userIdx);
    int getUserLastCrashPacketID(int userIdx);
    void incUserLastCrashDungeonID(int userIdx);
    int getUserLastCrashDungeonID(int userIdx);
    void incUserLastCrashPlayStatus(int userIdx);
    int getUserLastCrashPlayStatus(int userIdx);

    int m_abnormalDownUserCount;        // +0x00
    Mutex m_mutex;                      // +0x04
    std::map<int, int> m_crashPacketID;   // +0x1c
    std::map<int, int> m_crashDungeonID;  // +0x34
    std::map<int, int> m_crashPlayStatus; // +0x4c
};

#include "StatisticsToObserveCrash/StatisticsToObserveCrash.inc"
    : m_mutex(), m_crashPacketID(), m_crashDungeonID(), m_crashPlayStatus()
{
    m_abnormalDownUserCount = 0;
}

#include "StatisticsToObserveCrash/dtor_StatisticsToObserveCrash.inc"

#include "StatisticsToObserveCrash/reset.inc"
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount = 0;
    m_crashPacketID.clear();
    m_crashDungeonID.clear();
    m_crashPlayStatus.clear();
}

#include "StatisticsToObserveCrash/getAbnormalDownUserCount.inc"
{
    Guard<Mutex> guard(&m_mutex);
    return m_abnormalDownUserCount;
}

#include "StatisticsToObserveCrash/setAbnormalDownUserCount.inc"
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount = count;
}

#include "StatisticsToObserveCrash/incAbnormalDownUserCount.inc"
{
    Guard<Mutex> guard(&m_mutex);
    m_abnormalDownUserCount += 1;
}

// [b28d-62] AE：value_type 直构 + `(*it).second`（P167：去 make_pair 转换 ctor
//   与 ptEv）。ORIG 0x0860bb88
#include "StatisticsToObserveCrash/incUserLastCrashPacketID.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPacketID.find(userIdx);
    if (it == m_crashPacketID.end())
    {
        // [b28d-62] P167：ORIG 直接构造 value_type（pair<int&,int> 模板 ctor），
        //   非 make_pair（多一对 make_pair+转换 ctor call）。
        m_crashPacketID.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        // [b28d-62] P167：ORIG 走 operator*（deEv），非 operator->（ptEv）。
        (*it).second += 1;
    }
}

#include "StatisticsToObserveCrash/getUserLastCrashPacketID.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPacketID.find(userIdx);
    if (it != m_crashPacketID.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}

// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bd2e
#include "StatisticsToObserveCrash/incUserLastCrashDungeonID.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashDungeonID.find(userIdx);
    if (it == m_crashDungeonID.end())
    {
        m_crashDungeonID.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        (*it).second += 1;
    }
}

#include "StatisticsToObserveCrash/getUserLastCrashDungeonID.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashDungeonID.find(userIdx);
    if (it != m_crashDungeonID.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}

// [b28d-62] AE：同 incUserLastCrashPacketID。ORIG 0x0860bed4
#include "StatisticsToObserveCrash/incUserLastCrashPlayStatus.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPlayStatus.find(userIdx);
    if (it == m_crashPlayStatus.end())
    {
        m_crashPlayStatus.insert(std::map<int, int>::value_type(userIdx, 1));
    }
    else
    {
        (*it).second += 1;
    }
}

#include "StatisticsToObserveCrash/getUserLastCrashPlayStatus.inc"
{
    Guard<Mutex> guard(&m_mutex);
    std::map<int, int>::iterator it = m_crashPlayStatus.find(userIdx);
    if (it != m_crashPlayStatus.end())
    {
        return (*it).second;   // ORIG 用 operator*（de）
    }
    return 0;
}
