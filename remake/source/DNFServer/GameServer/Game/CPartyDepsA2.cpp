// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyDepsA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CParty.h"

#include <algorithm>
#include <stdio.h>
#include <vector>

#include "CDungeon.h"               // get_index/get_min_level/get_standard_level
#include "CDataManager.h"           // G_CDataManager/find_dungeon/m_conditionDungeon
#include "DataManagerInitDepends.h" // TowerOfDespairMgr/TOD_Layer/TOD_UserState
#include "TimerQueue.h"             // G_TimerQueue/TimerQueue::InsertTimer
#include "GlobalData.h"             // GlobalData::s_pTowerOfDespairMgr
#include "CWorldMap.h"              // IsInHellDungeon/m_field1c/m_field34
#include "PacketGuard.h"            // PacketGuard（InterfacePacketBuf 子类）
#include "CInventory.h"             // check_item_exist/delete_item 完整类型
#include "cMyTraceNoop.h"

namespace
{

// ORIG：user+0xcac 为当前 CDungeon*。CUser 该偏移区未建模（无具名字段/访问器），
// 按 DisPatcherCore.cpp 对 party+0xcac 的只读视图同口径处理【推断：布局口径】。
struct CUserCurDungeonView
{
    char m_pad[0xcac];
    CDungeon* m_dungeon;
};

}  // namespace

#include "CPartyDepsA2/getHellPartyEntranceItemNeedCount.inc"

#include "CPartyDepsA2/GetLayer.inc"


// ORIG T 0x086439c8 _ZNK9TOD_LayerplEi（报告缺失，按 nm 反汇编实现）：
// 构造 ret(m_layer+n)；ret.GetLayer()>100 → resetLayer()；sret 返回。
TOD_Layer TOD_Layer::operator+(int n) const
{
    TOD_Layer ret(static_cast<unsigned short>(m_layer + n));
    if (ret.GetLayer() > 100)
    {
        ret.resetLayer();
    }
    return ret;
}

#include "CPartyDepsA2/isEventDungeon.inc"


#include "CPartyDepsA2/isApplyLevel.inc"


#include "CPartyDepsA2/getTodayEnterLayer.inc"


#include "CPartyDepsA2/Get_CDungeonByLayer.inc"


#include "CPartyDepsA2/Get_DungeonIndexByLayer.inc"


#include "CPartyDepsA2/Get_APCIndexByLayer.inc"


#include "CPartyDepsA2/GetBaseLayerIndex.inc"


#include "CPartyDepsA2/IsExistCharacter.inc"


#include "CPartyDepsA2/FindUserByCharId.inc"


#include "CPartyDepsA2/BloodHandleDieUser.inc"


#include "CPartyDepsA2/CheckClearQuestHellParty.inc"


#include "CPartyDepsA2/UseSealDoorItems.inc"


#include "CPartyDepsA2/CheckHaveHellPartyPassItem.inc"


#include "CPartyDepsA2/SendPartyMemberTagChracInfo.inc"


#include "CPartyDepsA2/GetDungeonIndex_isTOD.inc"


#include "CPartyDepsA2/SetProperDungeonEvent.inc"


#include "CPartyDepsA2/SetProperLevelDungeon.inc"

