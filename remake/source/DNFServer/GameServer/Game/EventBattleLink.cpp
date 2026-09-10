// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventBattleLink/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// EventBattleLink.cpp —— 事件/战斗链接核销批次（10 符号，LinkHelper.EventBattleLink）
//
//   1. _ZN10BingoEvent16reqBingoQuizMarkER5CUseri          T 0x080ca262
//   2. _ZN10BingoEvent22sendMaxMatchLineRewardER5CUser     T 0x080ca622
//   3. _ZN20CraneMinigameManager10getItemCntEi             T 0x080ebd44
//   4. _ZN20CraneMinigameManager13isPickSuccessEsi         T 0x080ebce0
//   5. _ZN20CraneMinigameManager8sendItemEP5CUseri         T 0x080ebd7a
//   6. _ZN13CBattle_Field14SetDamageValueEii               T 0x08308ff8
//   7. _ZN13CBattle_Field19SetDamagedPacketCntEi           T 0x08308f4e
//   8. _ZN21CCommunityServerProxy16SendPvPBuddyListEjj     T 0x0846cb80
//   9. _ZN22CConditionEventManager27RequestConditionEventRewardEP5CUserscRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
//                                                          T 0x08334850
//  10. _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj T 0x080fda56
//      （附：_ZN34Packet_Request_PvP_Buddy_Conn_ListC1Ev W 0x0846cefc、
//           _ZN6CBingo11getQuizValueEv W 0x080cae12、_ZN6CBingo12getQuizIndexEv W 0x080cae1e、
//           _ZN19CraneMinigameScript7getItemEs T 0x088e38d6 —— 上述符号的直接依赖，同批产出）
//
// 报告：docs/class_func_reports/{BingoEvent,CBingo,CraneMinigameManager,
// CraneMinigameScript,CBattle_Field,CCommunityServerProxy,
// CConditionEventManager,CInGameAdvertisementManager}/。
// BingoEvent / CBingo 为 ORIG 全局命名空间类；与 BingoEventLink4.cpp /
// GmCmds_A.cpp 一样采用 TU 局部类（成员互不重叠，无强符号冲突）。
// ============================================================================

#include <map>
#include <string.h>

#include "CUser.h"                      // CUser/cUserHistoryLog（+0x79700）
#include "CUserCharacInfo.h"            // GetCurConditionEventStep 族/getCurCharacNo
#include "CDataManager.h"               // G_CDataManager/m_bingo/find_item/getItemList
#include "CItemList.h"                  // create_item/GetRestrictCode/get_rand_int
#include "Inven_Item.h"
#include "CInventory.h"
#include "CBattle_Field.h"              // SetDamageValue/SetDamagedPacketCnt/GetDungeon
#include "CParty.h"                     // GetParty + +0xb24 CBattle_Field 子对象
#include "CDungeon.h"                   // get_index
#include "WarField.h"                   // MapInfo::m_monsterMap/map_monster
#include "CraneMinigameManager.h"
#include "CCommunityServerProxy.h"
#include "Packet_Request_PvP_Buddy_Conn_List.h"
#include "CConditionEventManager.h"
#include "CInGameAdvertisementManager.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "CGuildServerProxy.h"          // CEventBase/CEventManager
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "EventClassify_Action.h"       // EventClassify::CEventActionMng
#include "InGameEventScripts.h"         // ::Action_SendMail
#include "GlobalData.h"
#include "LogManager.h"
#include "BingoEvent.h"                 // BingoEvent 权威头（m_bingo: shared_ptr<CBingo> @+0x0c）

// ---------------------------------------------------------------------------

// b22 桥清零（P201，Model22）：ORIG 0x0810cf32
// _ZN13EventClassify15CEventActionMng24process_action_send_mailEP5CUserR15Action_SendMailb
// 原 extern"C" mangled 直连桥转真成员调用——EventClassify_Action.h:28 权威声明
// 形参即全局 ::Action_SendMail（InGameEventScripts.h 唯一定义，R15 编码=全局
// 作用域，与 ORIG mangled 逐字符一致），定义 EventClassify_Action.cpp:35。旧
// 「镜像类型分歧」注记已随头修正失效。
// §1 BingoEvent —— reqBingoQuizMark / sendMaxMatchLineReward（ORIG 0x080ca262 /
// 0x080ca622）。类布局（ORIG ctor 0x080c9984）：+0x0c 为 boost::shared_ptr<CBingo>
// 槽（getBingoSystem 0x080c9fea 返回该地址，operator-> 解引用得 CBingo*）。
// ---------------------------------------------------------------------------
// BingoEvent 权威头（BingoEvent.h）：成员 `boost::shared_ptr<CBingo> m_bingo` @+0x0c，
// 方法声明 reqBingoQuizMark/sendMaxMatchLineReward/sendOneMatchLineReward 齐全。
class CBingo
{
public:
    int getQuizValue();                                        // ORIG W 0x080cae12
    int getQuizIndex();                                        // ORIG W 0x080cae1e
    int calNewValue(unsigned int& board, int idx);             // ORIG T 0x080cad4c（定义 BingoEventLink4.cpp）
};

// BingoEventLink4.cpp 定义的全局位工具（强符号，仅声明引用）。
#include "EventBattleLink/isSetBit_Uint.inc"
static const int kMaxMatchLine = 3;

#include "EventBattleLink/getQuizValue.inc"
}

// getQuizIndex @ 0x080cae1e（while 前置 jmp、size() 每次求值 setg 有符号比较、
// operator[] 传 int 下标 ixEj、this 直用不落局部——照 ORIG 汇编）。
#include "EventBattleLink/getQuizIndex.inc"
}

// reqBingoQuizMark @ 0x080ca262（reqBingoQuizMark.md 汇编逐分支转写）。
#include "EventBattleLink/reqBingoQuizMark.inc"
}


// sendMaxMatchLineReward @ 0x080ca622（sendMaxMatchLineReward.md 转写）：
// m_bingo.m_map30[kMaxMatchLine] = <type, value>（CDataManager+0xb568+0x30），
// type 经 find_item 构造 Inven_Item 后以系统邮件发放（sender=292/title=294）。
#include "EventBattleLink/sendMaxMatchLineReward.inc"
}

// ---------------------------------------------------------------------------
// §2 CraneMinigameManager / CraneMinigameScript（ORIG 0x080ebd44/0x080ebce0/
// 0x080ebd7a/0x088e38d6）。ORIG 中 Manager 与 Script 同一对象布局：
// +0x04 vector<CraneItemInfo*>、+0x18 内嵌 Action_SendMail。
// ---------------------------------------------------------------------------
// getItem @ 0x088e38d6。
#include "EventBattleLink/getItem.inc"
}

// getItemCnt @ 0x080ebd44：item->m_cnt（+0x08 unsigned short）。
#include "EventBattleLink/getItemCnt.inc"
}

// isPickSuccess @ 0x080ebce0：item 存在且 m_itemIdx(+4)==pickIdx 且
// get_rand_int(100) <= m_pickRatio(+0x10) 才成功。
#include "EventBattleLink/isPickSuccess.inc"
}

// sendItem @ 0x080ebd7a：经 +0x18 内嵌 Action_SendMail 发送物品，返回
// process_action_send_mail 结果；item 缺失或 cnt==0xffff 返回 0x15。
#include "EventBattleLink/sendItem.inc"
}

// ---------------------------------------------------------------------------
// §3 CBattle_Field::SetDamageValue / SetDamagedPacketCnt（0x08308ff8/0x08308f4e）。
// 均在当前地图 m_monsterMap 中按 uid 查找；SetDamageValue 对 map_monster+0x28
// 累加伤害，SetDamagedPacketCnt 对 +0x24 自增；未找到返回 false。
// ---------------------------------------------------------------------------
#include "EventBattleLink/SetDamageValue.inc"
}

#include "EventBattleLink/SetDamagedPacketCnt.inc"
}

// ---------------------------------------------------------------------------
// §4 CCommunityServerProxy::SendPvPBuddyList（0x0846cb80）+ 包构造（0x0846cefc）。
// ---------------------------------------------------------------------------
#include "EventBattleLink/Packet_Request_PvP_Buddy_Conn_List.inc"
}

#include "EventBattleLink/SendPvPBuddyList.inc"
}

// ---------------------------------------------------------------------------
// §6 CInGameAdvertisementManager::WriteLog（0x080fda56）：
//   state==5 → party+0xb24 战场副本 idx；stream 0x32c + UID；
//   GetInBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG> 填 0x0..0x1f；LOGDB_Q(4) 入队。
// ---------------------------------------------------------------------------
#include "EventBattleLink/WriteLog.inc"
}
