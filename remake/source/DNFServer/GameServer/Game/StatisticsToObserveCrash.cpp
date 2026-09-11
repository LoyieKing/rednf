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


#include "StatisticsToObserveCrash/dtor_StatisticsToObserveCrash.inc"

#include "StatisticsToObserveCrash/reset.inc"


#include "StatisticsToObserveCrash/getAbnormalDownUserCount.inc"


#include "StatisticsToObserveCrash/setAbnormalDownUserCount.inc"


#include "StatisticsToObserveCrash/incAbnormalDownUserCount.inc"


#include "StatisticsToObserveCrash/incUserLastCrashPacketID.inc"


#include "StatisticsToObserveCrash/getUserLastCrashPacketID.inc"


#include "StatisticsToObserveCrash/incUserLastCrashDungeonID.inc"


#include "StatisticsToObserveCrash/getUserLastCrashDungeonID.inc"


#include "StatisticsToObserveCrash/incUserLastCrashPlayStatus.inc"


#include "StatisticsToObserveCrash/getUserLastCrashPlayStatus.inc"

