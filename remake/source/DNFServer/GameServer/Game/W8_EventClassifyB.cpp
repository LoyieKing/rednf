// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_EventClassifyB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— EventClassify 域仅 ORIG 符号批次（W8_Mid.EventBatch）。
// 覆盖：
//   CObjectBringUp::{verifyGiveBringUpReward 0x0810fe1a, verifyMaterialItemByRule
//     0x0810fcea, sendBringUpReward 0x0810fe90}
//   CEventScriptMng::{send_event_init_record_data 0x0810cb2c,
//     process_update_ingame_history 0x0810c82e}
//   CPcRoomPlayTime::processEventPcRoomPlayTime 0x0811089c
//   CUsedFatigueGiveItem::get_user_type 0x08110eec（objdump·报告缺失）
//   CExchangeRandomItemReward::{verifyMaterialItemByRule 0x0810eb30,
//     deleteUsedMaterialItem 0x0810eede, getRewardItem 0x0810ee40,
//     sendRandomItemReward 0x0810f26e}
//   CAvatarDisjointRandomReward::{checkAvatarDisjointError 0x0810f3a4,
//     deleteAvatarItem 0x0810f4e2, SendDisjointAvatarStatistic 0x0810f5f4,
//     getRewardItem 0x0810f694, SendCreateEmblemStatistic 0x0810f742,
//     sendRandomItemReward 0x0810f88c}
// 每函数依据 docs/class_func_reports/EventClassify__*/ 对应 md 的
// ## 汇编 + ## 反编译 C 完整转写；类布局见 EventClassify_Action.h /
// InGameEventScripts.h / CEventScriptMng.h。
// [b28d-W8EventB 回边专项核对（Main 高优先级排查）] 全 17 个游戏函数逐位核对
// jmp/jcc **目标下标**（非仅助记符）：回边（BACK）数量与助记符两侧全等
// （sendBringUpReward 1/1、verifyMaterialItemByRule 1/1、process_update_ingame_history
// 1/1、CExchange::verifyMaterialItemByRule 4/4、deleteUsedMaterialItem 2/2、
// SendCreateEmblemStatistic 1/1，其余 0/0），且回边目标处指令均为**循环体头**
// （+0x72 / +0xc4 / +0x35 / …），非函数出口 ⇒ **不存在**「goto 退出标签 ⇒ 循环只
// 解析/迭代一轮」模式。本 TU 无 goto；2 处 continue（sendBringUpReward）与 2 处
// break（CExchange::verifyMaterialItemByRule、SendCreateEmblemStatistic）均编译为
// 正常前向跳转/回边，与 ORIG 一致。
// ----------------------------------------------------------------------------
// [b28d-D03 判词 · L2Free4to15.S6 本波更新]（fresh `check_tu_routed.sh`：identical=87 ae=148
//   （合计 235）near=1 diff=3 = _Z41__static_initialization…（GI 假缺口：ORIG 该符号有 493 个
//   实例，工具按名匹配取首个 0x0807ee76（属 PacketTypes）⇒ 逐 TU 读数不可比，零改动）
//   + sendBringUpReward + process_update_ingame_history；shift 探针对 5 件全跑）
//   · verifyMaterialItemByRule 0x0810fe1a —— 95/94：shift **PURE SHIFT c=0**（帧 0x24、
//     槽直方图全等）；唯一残差 = ORIG 比我们多 1 条孤立 `nop`（中段对齐产物，N112：
//     4.4 循环出口 nop 填充旗标无效、compare 口径已剥离尾部对齐 nop）⇒ 封 N112。
//   · deleteUsedMaterialItem 0x0810eede —— 223/223：shift PURE SHIFT c=0；残差 = 同内容
//     指令块相对序（difflib 重排），属 N104/EH 尾块相对序族 ⇒ 封顶。
//   · SendCreateEmblemStatistic 0x0810f742 —— 99/99（**NEAR**）：shift PURE SHIFT c=0；
//     残差 = 索引折形 `lea 0x8(%ebx),%edx; mov %eax,-0x4c(%ebp,%edx,4)` vs
//     `lea 0x4(%ebx),%edx; mov %eax,-0x3c(%ebp,%edx,4)`（算术等价：-0x4c+4*(ebx+8) ≡
//     -0x3c+4*(ebx+4)）⇒ P30/N4「add-vs-lea/scaled 索引」封顶；另 2 处数据符号立即数
//     （N132 AE 口径）。
//   · sendBringUpReward 0x0810fe90 —— **[S6 本波] 335→331（329/331，gap −4→−2）**，两类已消
//     （均非 N 类）：①**语义修复**——ORIG 变参顺序 = (get_acc_id, getCurCharacNo,
//     it->second.m_itemIdx)：0x0810ffcc/0x0810ffd9 先 characNo 后 accId，压栈位 0x1c/0x18/0x14
//     依次为 esi(m_itemIdx)/ebx(characNo)/eax(accId) ⇒ 原码 (m_itemIdx, characNo, accId) 写反
//     （日志输出错位），已按 ORIG 改正；②actionMng 由分支内声明提至 **for 之外** ⇒
//     it(-0x40)/actionMng(-0x39) 与全部 10 处 `lea` 槽位及帧 0x60 与 ORIG 全对齐
//     （P-L11d 正解；置于循环体内/if 前均只到 -0x3d/-0x3c）。残差 2 条：a) `sig->m_rewardFlag
//     = f()` 装载 ORIG `mov -0xc(%ebp),%edx; mov %al,0xd(%edx)` vs 4.4 先搬 eax（N7/P211
//     寄存器角色）；b) 正常路径 CStreamGuard 析构块 4.4 压至 landing pad 之后（多 1 jmp，
//     N104 块排布）。p167 58/58 CLEAR（仅 C1/C2·D1/D2 同址别名）。
//   · process_update_ingame_history 0x0810c82e —— 98/102：shift **PURE SHIFT c=0**；
//     残差 = 4 处 sret 调用点的寄存器角色（ORIG `lea -0xNN(%ebp),%edx; mov %eax,0x4(%esp)`
//     vs 4.4 `mov %eax,%edx; lea …,%eax`）⇒ N7（sret 地址/返回值寄存器角色互换，
//     4.4.7 后端固有，P230 判据）封顶。

#include <cstring>
#include <utility>

#include "CEventScriptMng.h"         // 实体类 + CEventScriptMng
#include "InGameEventScripts.h"      // ObjectBringUpScript / ExchangeRandom... 等
#include "EventClassify_Action.h"    // CEventActionMng
#include "CUser.h"                   // CUserCharacInfo getter / Send / get_acc_id
#include "CInventory.h"
#include "CItem.h"                   // GetItemType/get_grade/get_index
#include "CAvatarItemMgr.h"          // UnRegistItem/GetExpireDate
#include "CStatisticServerProxy.h"
#include "Packet_PcRoomPlayTimeReward.h"      // 全局 Packet_PcRoomPlayTimeReward（ORIG W C1 0x08110c0a）
#include "Packet_Emblem_Create_Statistic.h"   // 全局 Packet_Emblem_Create_Statistic（ORIG W C1 0x08110bdc）
#include "Packet_Avater_Disjoint_Statistic.h" // 全局 Packet_Avater_Disjoint_Statistic（ORIG W C1 0x08110bae）

#include "CDataManager.h"            // G_CDataManager/find_item/m_eventAvatarList
#include "GlobalData.h"              // s_stream_pool/s_msgq_mgr/s_*_proxy_mgr
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "PacketGuard.h"             // PacketGuard + PacketHeader（传递可见）
#include "SigTypes.h"                // SIG_OBJECT_BRING_UP
#include "CEnvironment.h"            // G_CEnvironment / m_serverEnvir.m_serverGroup
#include "GameWorld.h"               // IsIntegratedPvPBaseChannel / G_GameWorld
#include "GameTypes.h"               // cMyTrace
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy
#include "CMonitorServerProxy.h"
#include "DB_UpdateAvatarHistory.h"
#include "DB_UpdateInGameEventHistory.h"

// ORIG 全局作用域真实类 DB_SavePcRoomPlayTimeEvent
// （_ZN26DB_SavePcRoomPlayTimeEvent11makeRequestEjjh，T 0x08447bd0；
//   权威类定义与实例注册在 W8LT_DBTask.cpp，makeRequest 定义同 TU）。
struct DB_SavePcRoomPlayTimeEvent
{
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

namespace EventClassify
{

// ============================================================================
// CObjectBringUp
// ============================================================================

#include "W8_EventClassifyB/verifyGiveBringUpReward.inc"


#include "W8_EventClassifyB/verifyMaterialItemByRule.inc"


#include "W8_EventClassifyB/sendBringUpReward.inc"


#include "W8_EventClassifyB/get_user_type.inc"


#include "W8_EventClassifyB/processEventPcRoomPlayTime.inc"


// ============================================================================
// CEventScriptMng
// ============================================================================

#include "W8_EventClassifyB/send_event_init_record_data.inc"


#include "W8_EventClassifyB/process_update_ingame_history.inc"


// ============================================================================
// CExchangeRandomItemReward
// ============================================================================

#include "W8_EventClassifyB/getRewardItem.inc"


#include "W8_EventClassifyB/verifyMaterialItemByRule_1.inc"


#include "W8_EventClassifyB/deleteUsedMaterialItem.inc"


#include "W8_EventClassifyB/sendRandomItemReward.inc"


// ============================================================================
// CAvatarDisjointRandomReward
// ============================================================================

#include "W8_EventClassifyB/checkAvatarDisjointError.inc"


#include "W8_EventClassifyB/deleteAvatarItem.inc"


#include "W8_EventClassifyB/SendDisjointAvatarStatistic.inc"


#include "W8_EventClassifyB/getRewardItem_1.inc"


#include "W8_EventClassifyB/SendCreateEmblemStatistic.inc"


#include "W8_EventClassifyB/sendRandomItemReward_1.inc"


}  // namespace EventClassify
