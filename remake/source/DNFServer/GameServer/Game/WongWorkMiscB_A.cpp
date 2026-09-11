// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkMiscB_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "AvatarItem.h"
#include "CAvatarItemMgr.h"
#include "CHandlePremium.h"
#include "CCompoundAvatar.h"

#include <vector>

#include "CUser.h"                  // CUser / SUserPremiumInfo / SendCmdErrorPacket
#include "CUserCharacInfo.h"        // SetOneDayLetheFlag / getCurCharacName / getCurCharacInvenR
#include "CDataManager.h"           // G_CDataManager()->getItemList()
#include "CItemList.h"             // CItemList::GetRestrictCode
#include "CPremiumLetheManager.h"  // CPremiumLetheManager::NOT_USE_LETHE_STATE
#include "GameTypes.h"             // cMyTrace
#include "GlobalData.h"            // GlobalData::s_systemTime_ (CSystemTime)
#include "CInventory.h"            // GetInvenSlot / GetAvatarItemMgrR
#include "SPremiumInfo.h"           // g_SPremiumInfo（GetAddQuestItemDropRate 表源）
#include "STStackableScript.h"      // STEnchantSystemMapKey/Data 权威定义
#include "CItemAmplifier.h"         // upgrade_table_t 权威布局（+0x48c/+0x490 视图）
#include "CStackableItem.h"         // ProcUpgradeChronicle 材料券脚本（GetEmancipate 视图）
#include "CEquipItem.h"             // m_vec198/m_vec1a4（emancipate 材料/结果表）
#include "WongWorkMiscB_A/GetEmancipateMaterialList.inc"
{
    return m_vec198;
}

#include "WongWorkMiscB_A/GetEmancipateResultList.inc"
{
    return m_vec1a4;
}

namespace WongWork
{

#include "WongWorkMiscB_A/setColor.inc"


#include "WongWorkMiscB_A/updateEmblemSocket.inc"


#include "WongWorkMiscB_A/Avatar_Item.inc"

#include "WongWorkMiscB_A/dtor_Avatar_Item.inc"

#include "WongWorkMiscB_A/CAvatarItemMgr.inc"


#include "WongWorkMiscB_A/CAvatarItemMgr_1.inc"


#include "WongWorkMiscB_A/handleReturn2CharacView.inc"


#include "WongWorkMiscB_A/handleRemoveOneDayLethePremium.inc"


#include "WongWorkMiscB_A/tagCompoundInfo.inc"


#include "WongWorkMiscB_A/clear.inc"


#include "WongWorkMiscB_A/dtor_tagCompoundInfo.inc"

#include "WongWorkMiscB_A/_SendError.inc"


#include "WongWorkMiscB_A/_SendResult.inc"


#include "WongWorkMiscB_A/ProcCompound.inc"


#include "WongWorkMiscB_A/GetGoldBonus.inc"


#include "WongWorkMiscB_A/GetAddQuestItemDropRate.inc"


#include "WongWorkMiscB_A/find3rdChronicleEnchantData.inc"
    STEnchantSystemMapKey key, int type)
{
    CItem* itemScript = G_CDataManager()->find_item(type);
    if (itemScript == 0)
        return 0;
    typedef std::map<STEnchantSystemMapKey, STEnchantSystemMapData> EnchantMap;
    EnchantMap::iterator it =
        ((CStackableItem*)itemScript)->m_enchantMap.find(key);
    if (it != ((CStackableItem*)itemScript)->m_enchantMap.end())
        return &it->second;
    return 0;
}

#include "WongWorkMiscB_A/_getAdditionalDisjointItem.inc"


#include "WongWorkMiscB_A/ProcUpgradeChronicle.inc"


}  // namespace WongWork

