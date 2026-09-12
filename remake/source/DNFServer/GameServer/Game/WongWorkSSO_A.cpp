// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkSSO_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// WongWork::CSimpleSSO 方法集（G3 批次 A，ORIG df_game_r）。
// 报告：docs/class_func_reports/WongWork__CSimpleSSO/
//   expireLoginTime 0x08602b24 / reset 0x086028b4 / checkValidPassword 0x086028c6 /
//   updateSSOExpireTime(uint,st) 0x08602906 / updateSSOExpireTime(uint,uint) 0x08602a4a /
//   _checkValiedPassword 0x08602bd0 / _getGameStartTime 0x08602da6 /
//   getGameStartTime 0x086031f6 / updateHumanCertify 0x08603226 /
//   checkValidPassword_Jpn 0x081b25ec / stOutputData_t::ctor(W) 0x0818fb28。
// 类权威声明：CSimpleSSO.h（原 GlobalData.cpp 局部类收敛）；ctor/dtor/init 属 GlobalData.cpp。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射。
// [T1-GI · P327] 键名 `_GLOBAL__I__ZN8WongWork10CSimpleSSO22checkValidPassword_Jpn
// EjPKcS2_RNS0_14stOutputData_tE`（ORIG 0x81b26e8）：checkValidPassword_Jpn 定义前移为
// 本 TU 首个「强符号 · 外部链接 · 本 TU 定义」实体（改前首个 = stOutputData_t ctor）。
#include <iostream>

#include "CSimpleSSO.h"
#include "MySQL.h"
#include "CSystemTime.h"
#include "CGM_Manager.h"     // CGM_Manager::IsGm
#include "CRijndael.h"
#include "DNFFunctionLib.h"  // DNFFLib::Binary2Hex
#include "LogManager.h"      // LogManager::logFormat
#include "GameTypes.h"       // cMyTrace

// 轻量 extern 声明（避免拉入 CUser.h/GlobalData.h 重型共享头）：
char* NumberToString(unsigned int value, int radix);          // ORIG 0x0810904b
// [R18gd 2026-09-13] GlobalData 已按 ORIG 形态 class 化（namespace→class 静态成员）；
// 原此处 `namespace GlobalData { extern ... }` 最小声明重开点合并到权威头（唯一声明点）
// ——符号名/mangled 与 `GlobalData::x` 访问语法零变化。
#include "GlobalData.h"


#include <string.h>
#include <stdio.h>

#include "WongWorkSSO_A/checkValidPassword_Jpn.inc"


#include "WongWorkSSO_A/stOutputData_t.inc"


#include "WongWorkSSO_A/reset.inc"


#include "WongWorkSSO_A/expireLoginTime.inc"


#include "WongWorkSSO_A/_getGameStartTime.inc"


#include "WongWorkSSO_A/getGameStartTime.inc"


#include "WongWorkSSO_A/checkValidPassword.inc"


#include "WongWorkSSO_A/_checkValiedPassword.inc"


#include "WongWorkSSO_A/updateHumanCertify.inc"


#include "WongWorkSSO_A/updateSSOExpireTime.inc"


#include "WongWorkSSO_A/updateSSOExpireTime_1.inc"

