// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCreatureScriptMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ user_creature::CCreatureScriptMgr 实现。
#include "CCreatureScriptMgr.h"
#include "GameTypes.h"
#include "CreatureScript.h"
#include "LogManager.h"
#include "sync_script.h"   // CSyncScript::truncate/insert/IsEnable
#include <stdio.h>
#include <string.h>

// [T2b-GI · P338/P327] 补 <iostream> 引入 file-scope 动态初始化 __ioinit，恢复本 TU
// `__static_initialization_and_destruction_0`/`_GLOBAL__I_` 发射；键名目标
// `_ZN13user_creature28GetInstanceCreatureScriptMgrEv`（ORIG 0x833df33）。零语义改动。
#include <iostream>

namespace user_creature
{

// [T2b-GI · P327] 定义前移：本函数是 ORIG 该 TU 首个强外部符号 ⇒ GI 键名来源
// （`_GLOBAL__I__ZN13user_creature28GetInstanceCreatureScriptMgrEv`，ORIG 0x833df33）。
// 只移函数定义、不动带动态初始化的对象 ⇒ 构造序零变化。ORIG 原址 0x8336bd0。
CCreatureScriptMgr* GetInstanceCreatureScriptMgr()
{
    static CCreatureScriptMgr instance;
    return &instance;
}

#include "CCreatureScriptMgr/CCreatureScriptMgr.inc"


#include "CCreatureScriptMgr/dtor_CCreatureScriptMgr.inc"


#include "CCreatureScriptMgr/Destroy.inc"


#include "CCreatureScriptMgr/LoadCreatureScriptFile.inc"


#include "CCreatureScriptMgr/FindCreatureScript.inc"


#include "CCreatureScriptMgr/RegisterCreatureIdAndItemId.inc"


}  // namespace user_creature
