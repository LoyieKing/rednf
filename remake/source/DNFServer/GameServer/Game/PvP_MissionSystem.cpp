// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PvP_MissionSystem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CDataManager.h"
// ---- b28d-61 判词汇总（本 TU 4 件非 identical）----
// 已修 AE：Factory（P171 去 default 冗余赋值）、Load_EtcParameter（P292 早返）、
//   sort_list（P175/P225 索引循环+P159 int 返回）、C1（new CMTRand(time(0))+
//   resize(0x24,list)）、D1（P141 delete 直删）、Load_list（P198/P140/P167 全段还原）。
// b28d-70：get_WithinMissionIndex 0x085e2f00 已修（CMission.h MissionRankRange 改为
//   std::pair<int,int>，本函数同步 pair 形，P167 CLEAR）。
// [R26D 本轮修正] 三件 DIFF 全部推进（p167 真缺口清零/收敛）：
//  · importPvpMissionParameterScript 0x08a5b7ec —— 头置条件形（for(;;)+
//    if(!ScanType) break；ORIG +0x4a 循环头、+0x266 出口）⇒ gap −7→−3。
//    残余：①`push_back(idx)` 左值选 ERKj 而 ORIG 用 EOj 右值重载（改右值实测
//    identical −1：本 TU 唯一的 push_backERKj 实例不再发射，代价 > 收益，已回退；
//    p167 仍 MISSING EOj/EXTRA ERKj）；②`m_field0c = ScanInt()` 的 RHS-first+
//    edx 中转需该字段 unsigned 化（P211 档③；探针 h3 实证，cast 形 h6 无效）⇒
//    属共享头 MissionTypes.h，未落；③槽号/帧镜像 N82（ORIG idx 复用 −0x38，
//    4.4 独立分槽 ⇒ 帧 0x50 vs ORIG 0x40）。
//  · importPvpMissionScript 0x08a5ac28 —— 尾置条件形（while(ScanType)…；ORIG
//    +0xb26 尾条件块 `test %al; jne +0x79`、失败 +0xb47 `mov $1,%ebx`）+
//    `!(token == "[condition]")`（P167：ORIG 调 eq 版，旧文 `!=` 生成 ne）⇒
//    gap +5→+2；残余 = N89（ORIG 内联头 inline，4.4 恒 call）+ C1/C2 别名伪差。
//  · get_New_DailyMission 0x085e26e0 —— `make_pair` 去多余 static_cast<unsigned>
//    （ORIG 用 make_pairIiiE；旧文选 IjjE）+ `!(mi==end)` 取反式（eq 版）+
//    `(*mi).second` 与 `it->second` 混用（ptEv/deEv 各 1 = ORIG 同量）⇒ p167
//    差集由 7 项收敛到 3 项（余 _Unwind_Resume×2 = EH 段、vector::clear×1 = N89、
//    C1/C2 别名）。
//  · 全 TU 四数（fresh check_tu_routed）：225/466/0/6/691/missing171 →
//    225/465/0/6/690/missing170（missing −1、仅NEW −1、identical 不变）。
#include "CMission.h"
#include "MissionLessThan.h"
#include "PvPTypes.h"          // CMTRand
#include "DNFLexWrapper.h"     // loadRDARScriptFile / Scan* helper
#include <stdio.h>
#include "SyncScriptDeps.h"   // checkValiedJobName / getCharacterJob
#include "NpcScript.h"

// ============================================================================
// PvP_MissionSystem 装载支撑（G5 批次）。
// ORIG 各函数地址/语义见 docs/class_func_reports/PvP_MissionSystem/*.md。
// ============================================================================

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// [L32 GI 调序] ORIG `_GLOBAL__I_g_PvpMissionScriptBaseDirectory`（CU=RDARScriptPvpMission.cpp）
//   ⇒ 本 TU 首个强符号须为 g_PvpMissionScriptBaseDirectory。仅交换两块定义的先后：
//   零语义/零布局改动（g_PvpMissionScriptFileList 仍是本 TU 唯一动态初始化对象，
//   相对构造序不变，P327）。
// ORIG 全局（BSS 0x09500100，0x100 字节）：PvP 任务脚本根目录（运行期由
// 部署环境填充；BSS 初值即空串）。文件列表全局见下方 g_PvpMissionScriptFileList。
char g_PvpMissionScriptBaseDirectory[0x100];
// ORIG 全局 STScriptFileList（PvP 任务脚本列表，BSS 0x0949…）。由
// CDataManager::init 的 loadRDARScriptListFile 填充；此处提供符号。
STScriptFileList g_PvpMissionScriptFileList;

#include "PvP_MissionSystem/clear.inc"


#include "PvP_MissionSystem/clear_1.inc"


#include "PvP_MissionSystem/_ReadThreeInt.inc"


#include "PvP_MissionSystem/_ReadFourInt.inc"


#include "PvP_MissionSystem/importPvpMissionScript.inc"


#include "PvP_MissionSystem/importPvpMissionParameterScript.inc"


#include "PvP_MissionSystem/PvP_MissionSystem.inc"


#include "PvP_MissionSystem/dtor_PvP_MissionSystem.inc"


#include "PvP_MissionSystem/Factory.inc"


#include "PvP_MissionSystem/Load_list.inc"


#include "PvP_MissionSystem/Load_EtcParameter.inc"


#include "PvP_MissionSystem/copy_missionParameter.inc"


#include "PvP_MissionSystem/find_mission.inc"


#include "PvP_MissionSystem/get_WithinMissionIndex.inc"


#include "PvP_MissionSystem/get_kind_mission_list.inc"


#include "PvP_MissionSystem/get_daily_mission_kind.inc"


#include "PvP_MissionSystem/get_BaseMissionExp_byRank.inc"


#include "PvP_MissionSystem/get_New_DailyMission.inc"


#include "PvP_MissionSystem/sort_list.inc"

