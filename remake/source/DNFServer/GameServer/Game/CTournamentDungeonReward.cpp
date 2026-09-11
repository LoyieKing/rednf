// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CTournamentDungeonReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CTournamentDungeonReward.h"
#include <cstring>
#include <iostream>            // P338：ORIG 本 TU 含 std::ios_base::Init __ioinit
                               //（static_init 0x082854e9 有 InitC1+__cxa_atexit，
                               //  GI 0x08285529 sub $0x18）⇒ 缺此 include 则本 TU
                               //  不发射 GI/__static_init，missing_in_game 多 2。

#include "CDungeon.h"          // TournamentRewardRate/TournamentRewardItemRate 权威定义
#include "CBattle_Field_shared.h"   // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDataManager.h"
#include "CItemList.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameRand.h"          // get_rand_int

#include "CTournamentDungeonReward/Clear.inc"


#include "CTournamentDungeonReward/CTournamentDungeonReward.inc"


#include "CTournamentDungeonReward/dtor_CTournamentDungeonReward.inc"

#include "CTournamentDungeonReward/ResetReward.inc"


// ORIG 0x828502e：type∈{0,1}、maxItems<3 校验后，按 cardNo 在
// m_rewards[type][i].m_flag08 中匹配卡片号：
//   m_id==0  → items[0] 置 add_info=m_field04（固定物品），items[1] 空；
//   m_id!=-1 → items[1].m_addInfo=m_id 后经 CItemList::create_item 生成，
//              items[0] 空；m_id==-1 → 双空返回 0（未命中）。
#include "CDataManager.h"
#include "CItemList.h"   // create_item（G_CDataManager()->getItemList()）

#include "CTournamentDungeonReward/GetItemFromCard.inc"


#include "CTournamentDungeonReward/DecisionRewardType.inc"


#include "CTournamentDungeonReward/MakeReward.inc"


#include "CTournamentDungeonReward/SetRewardGold.inc"


#include "CTournamentDungeonReward/_MakeRewardPacket.inc"


#include "CTournamentDungeonReward/MakeRewardPacket.inc"

#include "CTournamentDungeonReward/SelectCard.inc"

#include "CTournamentDungeonReward/isMemberSelectCard.inc"


#include "CTournamentDungeonReward/_ForceSelectCard.inc"


#include "CTournamentDungeonReward/ForceSelectCard.inc"


#include "CTournamentDungeonReward/isCardSelectComplete.inc"


#include "CTournamentDungeonReward/MakeSelectPacket.inc"

