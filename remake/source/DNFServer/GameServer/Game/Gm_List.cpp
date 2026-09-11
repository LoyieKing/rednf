// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Gm_List/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r：Gm_List 独立编译单元。
// ---- b28d-61 判词汇总（本 TU 2 件非 identical，全 N 封顶）----
// SendGMList_To_UpperServer 0x08299d54（P167 MISSING+EXTRA：Packet_Sync_GM_List
//   构造/发送块差异，N80 变参求值序）、LoadGmListFromDb 0x08299a10
//   （P167 CLEAR 38/38；N78/N89 迭代器+局部初始化 lowering）。
// ORIG SendGMList_To_UpperServer @0x8299d54：
//   serverGroup 0=监视服务器、1=公会服务器；每 20 个 GM 一包（Packet_Sync_GM_List，
//   header 0x1f4d/0x70），m_pageNo 递增；末批不足 20 也发送。
#include "CGuildServerProxy.h"   // Gm_List / WongWork::CGMAccounts 权威声明

#include <list>

#include "CGuildServerProxy.h"      // WongWork::CGMAccounts / stGMInfo_t / proxies
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "Packet_Sync_GM_List.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CInventory.h"   // NumberToString

#include "Gm_List/SendGMList_To_UpperServer.inc"



// ---- 生命周期 / DB 装载 ----

#include "Gm_List/Gm_List.inc"


#include "Gm_List/dtor_Gm_List.inc"

#include "Gm_List/LoadGmList.inc"


#include "Gm_List/clearGmList.inc"


#include "Gm_List/LoadGmListFromDb.inc"

