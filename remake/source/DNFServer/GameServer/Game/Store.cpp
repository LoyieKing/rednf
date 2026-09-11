// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Store/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r Game/ Store 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Store.md：
//   GetSellItemPrice(Inven_Item&,...) 0x08618ea0
//   GetSellItemPrice(CUser*,...)      0x08619008
//   repair_equip                      0x08619cec
//   user_buy_item                     0x08618a44
//   user_sell_item                    0x086193f8
#include "CInventory.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CGameManager.h"
#include "CEnvironment.h"   // GlobalInstance<Store>（ORIG G_Store 调 inst_ptr）
#include "CDungeon.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>
#include <vector>
#include "CHackLog_InvitationPaperSellToNPC.h"   // user_sell_item hack 检查
#include "CHackAnalyzer.h"                        // addServerHackCnt
#include "HistoryLog.h"                           // WriteSellItem

#include "CStatisticServerProxy.h"
#include "Packet_DeathTower_Statistic_Value.h"   // 类带 PacketHeader(0xc3b,0x15) ctor（ORIG C1 0x0860ffc2）

#include "Store/user_buy_item.inc"


// ---- 前向声明 ----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};

class CDeathTowerValueStatistic
{
public:
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field, char a, char b, unsigned int c);
};

extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic()
{
    static CDeathTowerValueStatistic instance;
    return &instance;
}

#include "Store/SendDeathTowerValueStatistic.inc"


// 修理价格系数（由 InitEquipmentScript 从脚本加载）
static float g_repairRate = 0.0f;

#include "Store/getEquipmentRepairPrice.inc"


#include "Store/getItemSellPrice.inc"


#include "Store/GetSellItemPrice.inc"


#include "Store/GetSellItemPrice_1.inc"


#include "Store/user_sell_item.inc"

// ---- repair_equip 辅助类型 ----
struct tagRepairItemInfo
{
    INVEN_TYPE m_type;       // 0=背包, 1=装备栏
    int m_slot;              // 槽位索引
    CEquipItem const* m_item;

    // ORIG C1/C2 0x0861b08e（W）：三形参顺序赋入（INVEN_TYPE, int, CEquipItem const*）。
    tagRepairItemInfo(INVEN_TYPE type, int slot, CEquipItem const* item)
        : m_type(type), m_slot(slot), m_item(item) {}
};

extern int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                                   int growthGrade, bool flag, float rate);

#include "Store/repair_equip.inc"


// ---- force 发射 tagRepairItemInfo C1/C2（ORIG W 0x0861b08e）----
namespace {
#include "Store/forceTagRepairItemInfoEmission.inc"

}  // namespace
