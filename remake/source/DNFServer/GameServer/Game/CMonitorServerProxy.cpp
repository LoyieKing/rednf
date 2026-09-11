// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMonitorServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN19CMonitorServerProxyC2ESsii`（ORIG 0x84715a6）；ctor 已是首强符号。
#include <iostream>

#include "LogManager.h"
// df_game_r 还原 —— CMonitorServerProxy（监视服务器客户端代理）实现。
// 参照 docs/class_func_reports/CMonitorServerProxy.md；单 TU 编译后经
// check_tu_game_orig.sh 与 df_game_r ORIG 对比（AE 口径）。

#include "CMonitorServerProxy.h"

#include <errno.h>
#include <string.h>

#include "Packet_Change_Char_Name.h"
#include "Packet_Exchange_Server_Info.h"
#include "Packet_Game_Server_Regist.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_GTM_OntimeEvent_RequestReward.h"
#include "Packet_Monitor_Call_Guild_Info.h"
#include "Packet_Monitor_Call_Guild_Level_Up.h"
#include "Packet_Monitor_Call_Guild_Members.h"
#include "Packet_Monitor_Char_Changable_Info.h"
#include "Packet_Monitor_Member_Pay_Tax.h"
#include "Packet_Monitor_Notice_Charac_Live_On_Ten_Min.h"
#include "Packet_Monitor_Notify_New_Mail.h"
#include "Packet_Monitor_UDP_HeartBeat.h"
#include "Packet_Monitor_UDP_Login.h"
#include "Packet_Monitor_UDP_Logout.h"
#include "Packet_Monitor_User_Repel_ByCharName.h"
#include "Packet_Notice_Guild_War_End.h"
#include "Packet_Notice_Guild_War_Start.h"
#include "Packet_Request_Charac_Tower_Update_Ranking.h"

#include "DNFFunctionLib.h"

#include "CMonitorServerProxy/CMonitorServerProxy.inc"


#include "CMonitorServerProxy/Init.inc"


#include "CMonitorServerProxy/InitUdp.inc"


#include "CMonitorServerProxy/InitTcp.inc"


#include "CMonitorServerProxy/Disconnect.inc"


// ---- 收发 ----

#include "CMonitorServerProxy/SendPacket.inc"


#include "CMonitorServerProxy/SendTcpPacket.inc"


#include "CMonitorServerProxy/OnTcpRecv.inc"


#include "CMonitorServerProxy/OnTcpSend.inc"


#include "CMonitorServerProxy/OnTcpError.inc"


#include "CMonitorServerProxy/Parsing.inc"


// ---- Send*：登录/心跳/信息上报 ----

#include "CMonitorServerProxy/SendCharLevelGrowType.inc"


#include "CMonitorServerProxy/SendHeartBeat.inc"


#include "CMonitorServerProxy/SendLogin.inc"


#include "CMonitorServerProxy/SendLogout.inc"


#include "CMonitorServerProxy/SendPacketCallGuildMembers.inc"


#include "CMonitorServerProxy/SendGuildLevelup.inc"


#include "CMonitorServerProxy/SendCallGuildInfo.inc"


#include "CMonitorServerProxy/SendMemberPayTax.inc"


#include "CMonitorServerProxy/SendUserRepel.inc"


#include "CMonitorServerProxy/SendNotifyNewMail.inc"


#include "CMonitorServerProxy/SendGuildWarEnd.inc"


#include "CMonitorServerProxy/SendGuildWarStart.inc"


#include "CMonitorServerProxy/SendExchangeServerInfo.inc"


#include "CMonitorServerProxy/SendNoticeCharLive.inc"


#include "CMonitorServerProxy/SendRequestUserMid.inc"


#include "CMonitorServerProxy/SendRequestOnTimeEventReward.inc"


#include "CMonitorServerProxy/updateTowerRank.inc"


#include "CMonitorServerProxy/SendChangeCharName.inc"


#include "CMonitorServerProxy/SendGameServerInfo.inc"

