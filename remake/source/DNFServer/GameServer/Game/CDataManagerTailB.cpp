// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDataManagerTailB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <cstring>
#include <cstddef>
#include <vector>

#include "GameTypes.h"      // cMyTrace
#include "CDataManager.h"
#include "CStackableItem.h"   // booster/package 检查的完整类型
#include "STStackableScript.h" // STStackablePackageData（pkg reinterpret 目标）
#include "CMap.h"           // VerifyMap → CMap 完整类型
#include "CDungeon.h"       // VerifyDungeon → CDungeon::VerifyMaze / m_clearItemMap
#include "CSkill.h"         // Verify → CSkill 技能遍历
#include "CMonster.h"       // Verify → CMonster 等级检查 / get_index
#include "Quest.h"          // Verify → RewardItemInfo / Quest 遍历
#include "CDungeonClear.h"  // WongWork::CDungeonClear

// ---------------------------------------------------------------------------
// 原 5 个 ORIG-W 访问器已按 P233 迁回 CDataManager.h 类内 inline 定义
// （IsAvailablePvPChannel 0x0819ede0 / GetMinLevelPvPChannel 0x0819ee2e /
// GetMaxLevelPvPChannel 0x0819ee3c / GetLimitItemUsageInfo 0x8693f16 /
// is_AuctionRegFreeCeraItem 0x84e92a4：ORIG 绑定全 W、地址分散于消费 TU 段
// ARAD_LoginSecurity/InterDispatcher/Inter_LoadEtc_impl/CUserOnlyOrig）；
// 本 TU 不再定义这 5 个符号，由各使用点发射 W 副本。
// ---------------------------------------------------------------------------

#include "CDataManagerTailB/VerifyDungeon.inc"


#include "CDataManagerTailB/Verify.inc"


#include "CDataManagerTailB/_subPunctuationMark.inc"


#include "CDataManagerTailB/rewriteUsableEmoticon.inc"


#include "CDataManagerTailB/hasPreventPrefixCharacName.inc"



#include "CDataManagerTailB/hasSpecialCharacterExceptSpace.inc"


#include "CDataManagerTailB/get_createCharacterGift.inc"


#include "CDataManagerTailB/get_useJobFatigueMatchGift.inc"


#include "CDataManagerTailB/get_useJobFatigueMatchGiftMsg.inc"


#include "CDataManagerTailB/get_server_name.inc"

