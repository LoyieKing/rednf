// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertExtraction/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CExtraction.h"
#include "CAlchemistExtraction.h"
#include "CEnchanterExtraction.h"
#include "CDollControllerExtraction.h"
#include "CExtractionMgr.h"

#include "CUser.h"          // CUserCharacInfo / CUser::CheckItemLock / InventoryMemory
#include "CInventory.h"     // CInventory / CSecu_ProtectionField / GlobalData::s_pSecuProtectionField
#include "CDataManager.h"   // G_CDataManager / find_item / GetExpertJobScript
#include "CItem.h"
#include "CEquipItem.h"     // CEquipItem::m_itemType（+0x234）
#include "CStackableItem.h" // m_int334/+0x334、m_int338/+0x338、m_int33c/+0x33c
#include "STExpertJobScript.h"
#include "STItemScript.h"   // ENUM_EXPERT_JOB_TYPE
#include "GameWorld.h"      // G_GameWorld / IsPVPChannel
#include "GameTypes.h"      // cMyTrace / LogManager::logFormat
#include "PvPTypes.h"       // CMTRand::randInt

#include <utility>

namespace expert_extraction
{

#include "ExpertExtraction/GetExtracterType.inc"


#include "ExpertExtraction/_check_need_slot.inc"


#include "ExpertExtraction/check_error.inc"


#include "ExpertExtraction/inven_process.inc"


#include "ExpertExtraction/get_extracter.inc"


#include "ExpertExtraction/get_alchemist_extraction_type.inc"


#include "ExpertExtraction/get_extraction_count.inc"


#include "ExpertExtraction/_check_error.inc"


#include "ExpertExtraction/check_error_1.inc"


#include "ExpertExtraction/extraction.inc"


#include "ExpertExtraction/get_extraction_count_1.inc"


#include "ExpertExtraction/_check_error_1.inc"


#include "ExpertExtraction/check_error_2.inc"


#include "ExpertExtraction/extraction_1.inc"


#include "ExpertExtraction/get_doll_controller_extraction_type.inc"


#include "ExpertExtraction/get_extraction_count_2.inc"


#include "ExpertExtraction/_check_error_2.inc"


#include "ExpertExtraction/check_error_3.inc"


#include "ExpertExtraction/extraction_2.inc"


}  // namespace expert_extraction
