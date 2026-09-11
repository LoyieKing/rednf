// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Secu_GoldControl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ Secu_GoldControl 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Secu_GoldControl.md
#include "CUser.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include <string.h>

class DB_CheckCharacName
{
public:
    static void makeRequest(int uid, unsigned int accId, const char* charName, bool flag);
};
// ORIG 金币统计区组件视图（P1：成员视图折叠 lea/add 与 ORIG 同形）
struct SecuGoldView
{
    char m_pad0[0x04];          // m_pUser @ +0x00
    user_gold_info m_total;     // +0x04
    user_gold_info m_cur;       // +0x40
    user_gold_info m_backup;    // +0x7c
};
// 协议视图（W8LT_DBTask.cpp 同型）：+0x00=accId、+0x04=char_name 串、
// +0x22=flag 字节（§4.3 具名字段直写，ORIG `mov %dl,0x22(%eax)` 直接寻址）
#pragma pack(push, 1)
struct DB_CheckCharacNameReqView
{
    unsigned int m_accId;       // +0x00
    char m_charName[0x1e];      // +0x04
    unsigned char m_flag;       // +0x22
    unsigned char m_pad23;      // +0x23
};
#pragma pack(pop)

#include "Secu_GoldControl/Init.inc"


#include "Secu_GoldControl/makeRequest.inc"

#include "Secu_GoldControl/SetInfo.inc"


#include "Secu_GoldControl/SetMailCharName.inc"


#include "Secu_GoldControl/resetRangeMoney.inc"

#include "Secu_GoldControl/AddGold.inc"

#include "Secu_GoldControl/SubGold.inc"


#include "Secu_GoldControl/AddMoney.inc"


#include "Secu_GoldControl/SubMoney.inc"


#include "Secu_GoldControl/AddImportMoney.inc"


#include "Secu_GoldControl/SubImportMoney.inc"

#include "Secu_GoldControl/AddAuctionMoney.inc"


#include "Secu_GoldControl/SubAuctionMoney.inc"


#include "Secu_GoldControl/MailComplete.inc"


#include "Secu_GoldControl/TradeComplete.inc"


#include "Secu_GoldControl/MailGold.inc"


#include "Secu_GoldControl/MailSameMid.inc"




#include "Secu_GoldControl/SavetoDB.inc"


#include "Secu_GoldControl/CheckMoneyRange.inc"

#include "Secu_GoldControl/GetGoldInfo.inc"


#include "Secu_GoldControl/CheckDate.inc"



#include "Secu_GoldControl/UseFatigue.inc"

