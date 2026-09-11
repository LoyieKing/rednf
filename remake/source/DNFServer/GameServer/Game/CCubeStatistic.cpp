// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCubeStatistic/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <string>

#include "CCubeStatistic.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CUser.h"
#include "CEnvironment.h"
#include "Packet_Cube_Statistic.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"

// ORIG _ZN26SECRET_SHOP_STATISTIC_DATAC1Ev @ 0x85fd45a：清零 m_data[0..4]（5 int）。
struct SECRET_SHOP_STATISTIC_DATA
{
    SECRET_SHOP_STATISTIC_DATA();
    int m_data[5];   // [0]=dungeonIdx [1]=showCount [2]=showPrice
                     // [3]=buyCount [4]=buyPrice
} __attribute__((packed));

#include "CCubeStatistic/SECRET_SHOP_STATISTIC_DATA.inc"


// ---- getItemGroupNameTable（ORIG 0x88c0409）：__cxa_guard 单例 map ----
// findCubeStuff 拷贝本表后查 key "cube stuff"。ORIG 单例体仅默认构造空 map
//（条目由其它加载逻辑填充），此处保持一致。
std::map<std::string, int>& getItemGroupNameTable()
{
    static std::map<std::string, int> mapItemGroupTable;
    return mapItemGroupTable;
}

// ---- GetInstanceCubeStatistic（ORIG 0x860de50）：单例 ----
CCubeStatistic* GetInstanceCubeStatistic()
{
    static CCubeStatistic instance;
    return &instance;
}

#include "CCubeStatistic/CCubeStatistic.inc"


#include "CCubeStatistic/findCubeStuff.inc"


#include "CCubeStatistic/sendCubeStatistic.inc"


#include "CCubeStatistic/collectCubeStatistics.inc"


#include "CCubeStatistic/collectCubeStatistics_1.inc"
