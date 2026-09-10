// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PvP_MissionSystem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ---- ORIG _ZN13MissionScript5clearEv @0x08a5bbb2 ----
#include "PvP_MissionSystem/clear.inc"
}

// ---- ORIG _ZN22MissionParameterScript5clearEv @0x08a5bca2 ----
#include "PvP_MissionSystem/clear_1.inc"
}

// ---- ORIG _Z13_ReadThreeIntP13MissionScript @0x08a5ab91 ----
// 条件参数三连读：+0x78 / +0x7c / +0x80。
#include "PvP_MissionSystem/_ReadThreeInt.inc"
}

// ---- ORIG _Z12_ReadFourIntP13MissionScript @0x08a5abd2 ----
// [within] 条件四连读：+0x78 / +0x7c / +0x80 / +0x84。
#include "PvP_MissionSystem/_ReadFourInt.inc"
}

// ============================================================================
// importPvpMissionScript（ORIG _Z22importPvpMissionScriptP13MissionScriptPKc，
// 0x8a5ac28）。反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionScript.md。返回 false=装载/解析失败；true=成功或 EOF 正常收尾。
// ============================================================================
#include "PvP_MissionSystem/importPvpMissionScript.inc"
}

// ============================================================================
// importPvpMissionParameterScript（ORIG
// _Z31importPvpMissionParameterScriptR22MissionParameterScript，0x8a5b7ec）。
// 反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionParameterScript.md。
// ============================================================================
#include "PvP_MissionSystem/importPvpMissionParameterScript.inc"
}
    return true;
}

// ============================================================================
// 构造（ORIG 0x85e1abc）
// 顺序：map@+0x00、vector<list<CMission*>>@+0x18、vector<uint>@+0x24、
//       list<pair<uint,uint>>@+0x34、bitset@+0x40、vector<uint>@+0x48；
//       m_trand = new CMTRand(time(0))；m_field3c=0；reset bitset；
//       m_kindMissionList.resize(0x24)。
// ============================================================================
// ORIG 0x85e1abc：+0x30(m_field30)/+0x3c(m_field3c)/+0x44(m_trand) 仅
// 构造函数体内赋值一次，初始化列表不产生写入（m_field30 保持垃圾，与 ORIG 一致）。
#include "PvP_MissionSystem/PvP_MissionSystem.inc"
}

// ============================================================================
// 析构（ORIG 0x85e1c48）
// 顺序：delete m_trand；m_trand=0；遍历 m_missionMap delete 各 CMission；
//       m_missionMap.clear(); 其余成员默认析构。
// ============================================================================
#include "PvP_MissionSystem/dtor_PvP_MissionSystem.inc"
}

// ============================================================================
// Factory（ORIG 0x85e2132）
// 依 script.m_field34（m_kind 的派生分派）switch 构造对应 CMission_* 子类。
// 各子类 ctor 实参布局（script 偏移）：
//   +0x7c count / +0x80 第二参数 / +0x84 第三参数 / +0x60 comboMap。
// ============================================================================
#include "PvP_MissionSystem/Factory.inc"
}

// ============================================================================
// Load_list（ORIG 0x85e1dc6）
// 遍历 g_PvpMissionScriptFileList.m_fileMap，逐条 import，成功则 Factory、
// 按 GetMissionIndex 插入 m_missionMap、按 GetMissionKind 挂入 m_kindMissionList、
// kind==2 时置 m_dailyMissionKind 位；最后 sort_list。
// ============================================================================
#include "PvP_MissionSystem/Load_list.inc"
}

// ============================================================================
// Load_EtcParameter（ORIG 0x85e1fee）
// importPvpMissionParameterScript 成功则 copy_missionParameter。
// ============================================================================
#include "PvP_MissionSystem/Load_EtcParameter.inc"
}

// ============================================================================
// copy_missionParameter（ORIG 0x85e206e）
//   m_dailyMissionIndices(+0x24) = param.m_dailyIndices(+0x00)
//   m_field30(+0x30)             = param.m_field0c
//   m_dailyMissionList(+0x34)    = param.m_dailyList(+0x10)
//   m_field3c(+0x3c)             = param.m_field18
//   m_baseMissionExp(+0x48)      = param.m_baseExp(+0x1c)
// ============================================================================
#include "PvP_MissionSystem/copy_missionParameter.inc"
}

#include "PvP_MissionSystem/find_mission.inc"
}

// ORIG 0x85e2f00（b28d-70 P167 CLEAR）：循环外构造 `std::pair<int,int> grade(0, 0)`
// （0x85e2f06/0x85e2f0d 双 movl $0 + C1IiiEEOT_OT0_ 0x081627d4），循环内
// `grade = (*it)->GetRankRange()`（aSEOS0_ 0x080ddf1c move 赋值），`grade.first == idx`
// 命中即返回 GetMissionIndex()；无匹配返回 0。ORIG 循环体无判空（P140：0x85e2f57 直接
// operator*→GetRankRange），故不写 `if (!*it)`。init/cond 两处**各调一次**
// `m_kindMissionList[0x1b]` 的 const operator[]（0x85e2f3e/0x85e2fc3）⇒ 不缓存 list
// 引用（P48/P183 重物化）。
// [b28d-70] 判词：DIFF→AE 70/71；P167 CLEAR 12=12（C1IiiE/aSEOS0_ 双符号命中）。
#include "PvP_MissionSystem/get_WithinMissionIndex.inc"
}

// ORIG 0x85e26a0：else 分支 operator[](0)（m_kindMissionList[0]，非静态空表）。
#include "PvP_MissionSystem/get_kind_mission_list.inc"
}

#include "PvP_MissionSystem/get_daily_mission_kind.inc"
}

// ORIG 0x85e2e74：越界时 cMyTrace 记录 InvalidRank 并返回 0。
#include "PvP_MissionSystem/get_BaseMissionExp_byRank.inc"
}

#include "PvP_MissionSystem/get_New_DailyMission.inc"
}

#include "PvP_MissionSystem/sort_list.inc"
}
