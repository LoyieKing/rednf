// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventBattleLink/<Func>.inc（不参与编译、内部不含 #include）。
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

#include "EventBattleLink/isSetBit_Uint.inc"

// ORIG sendMaxMatchLineReward 以 static int kMaxMatchLine（.rodata 0x08ae5c40 实例，
// 值 3）为 map key 查询满线奖励。
static const int kMaxMatchLine = 3;

#include "EventBattleLink/getQuizValue.inc"
{
    struct QuizValueView
    {
        char m_pad[0xc];
        int m_value;
    };
    // P1：组件视图折叠单指令 disp（mov 0xc(%eax),%eax）。
    return reinterpret_cast<QuizValueView*>(this)->m_value;
}

#include "EventBattleLink/getQuizIndex.inc"


#include "EventBattleLink/reqBingoQuizMark.inc"



#include "EventBattleLink/sendMaxMatchLineReward.inc"


#include "EventBattleLink/getItem.inc"


#include "EventBattleLink/getItemCnt.inc"


#include "EventBattleLink/isPickSuccess.inc"


#include "EventBattleLink/sendItem.inc"


#include "EventBattleLink/SetDamageValue.inc"


#include "EventBattleLink/SetDamagedPacketCnt.inc"


#include "EventBattleLink/Packet_Request_PvP_Buddy_Conn_List.inc"


#include "EventBattleLink/SendPvPBuddyList.inc"


#include "EventBattleLink/WriteLog.inc"

