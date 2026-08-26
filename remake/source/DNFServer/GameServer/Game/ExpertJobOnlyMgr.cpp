// ============================================================================
// ExpertJobOnlyMgr.cpp — expert_job OnlyOrig 清零批（EJMgrSide）
//
// 符号清单（ORIG nm -S 核对地址；全部逐报告/objdump 忠实转写）：
//   CAlchemist::UseDelilah                     T 0x085d18ae sz 0x33
//   CAlchemist::UseRevivalItem                 T 0x085d1694 sz 0x7d
//   CAlchemist::GetAssaultPrevent              T 0x085d137c sz 0xa7
//   CAlchemist::OnUseStackableItem             T 0x085d1712 sz 0x19b
//   CAlchemist::GetQuestItemDropRate           T 0x085d12ee sz 0x71
//   CAlchemist::CheckUsableStackableItem       T 0x085d18e2 sz 0x96
//   CDisjointer::OnEnterDisjointStore          T 0x085d309a sz 0x101
//   CDisjointer::OnRepairDisjointMachine       T 0x085d35ce sz 0x218
//   CDisjointer::OnUpgradeDisjointMachine      T 0x085d37e8 sz 0x2e2
//   CDollController::{C1,C2,D1,D2,D0}          T 0x085d45b4 / D1 0x085d45d0 /
//                                              D0 0x085d4600（C2/D2 同址）
//   CExpertJobMgr::OnLeaveUser                 T 0x0849e214 sz 0x41
//   CExpertJobMgr::OnRequestPeer               T 0x0849e256 sz 0x10b
//   CExpertJobMgr::OnResponsePeer              T 0x0849e362 sz 0x1fb
//   CExpertJobMgr::IsAvailableRequestPeer      T 0x0849e55e sz 0xad
//   CExpertJobMgr::OnEnchantByBead             T 0x0849ed1a sz 0x474
//   CExpertJobMgr::OnGiveup                    T 0x0849e60c sz 0x70d
//   CCharacExpertJob::SendExpertJobInfo        T 0x0832b454 sz 0x201
//   CCharacExpertJob::set_disjoint_machine_grade W 0x85d3e9a sz 0x1b（objdump）
//   CDisjointMachine::include(IObject const&) const W 0x85d3de6 sz 0x20（objdump）
//
// 已存在 owner（SKIP）：CExpertJobMgr::AcquireExpertJob → ExpertJobLink4.cpp。
// ============================================================================

#include "CExpertJob.h"

#include "CBattle_Field.h"     // SetAssignNPCByPotion（UseDelilah）
#include "CDataManager.h"      // G_CDataManager / find_item / EnableWorld 表
#include "CItem.h"             // is_stackable/get_index/virtual getUsableItemType
#include "CPrivateStoreMgr.h"  // private_store::{GetInstancePrivateStoreMgr,
                               // CPrivateStoreMgr::IsBusyPrivateStore}
#include "CParty.h"            // GetParty/get_user（0xb24 CBattle_Field 区）
#include "CStackableItem.h"    // m_int2d0 APC 药水持续秒
#include "CSystemTime.h"       // getCurSec
#include "GameTypes.h"         // cMyTrace（OnEnchantByBead trace）
#include "GlobalData.h"        // s_systemTime_/s_rentalSystem
#include "PacketGuard.h"       // PacketGuard / InterfacePacketBuf 基础
#include "TimerQueue.h"        // G_TimerQueue/InsertTimer/TIMER_MESSAGE
#include "UserQuest" /* 见下 */

// UserQuest 完整类型在 CUser.h（getCurCharacQuestRefR 返回）；下列 include 组合
// 保证 OnGiveup 全部跨域调用可见。
