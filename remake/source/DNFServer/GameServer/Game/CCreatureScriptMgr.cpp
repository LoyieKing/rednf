// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCreatureScriptMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG CCreatureScriptMgr::CCreatureScriptMgr @ 0x833cb60：两个 map 默认构造。
#include "CCreatureScriptMgr/CCreatureScriptMgr.inc"
}

#include "CCreatureScriptMgr/dtor_CCreatureScriptMgr.inc"
}

// ORIG Destroy @ 0x833d3bc：遍历 +0x18 map，删除（D1 + operator delete）每个
// CCreatureScript*，最后 clear()。
// [b28d-30] 复核 DIFF：我方少 8 条（析构/clear 路径形差，N82/P12 族），ORIG 0x0833dxxx 见 report。
#include "CCreatureScriptMgr/Destroy.inc"
}

//   3) sync IsEnable 时按 native.m_attackInfo[0..1] 拼 Attack 路径并
//      ImportAttackInfoScript，序列化 "%d,%d,%f,%d"（元素集追加 ",%d"，
//      空集追加 ",4"；失败则 "%d,%d"）入 buf270/buf370；
//   4) 构造 CCreatureScript，Set(native)，SetItemId(注册表 itemId)，
//      insert_creature_info_to_db，查重后注册进 m_scripts。
// [b28d-30] 复核 DIFF：大件残差（N82/N89 族），ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/LoadCreatureScriptFile.md。
#include "CCreatureScriptMgr/LoadCreatureScriptFile.inc"
}

// ORIG FindCreatureScript @ 0x833d49a：在 +0x18 map 按 creatureId 查脚本指针；
// 未命中 cMyTrace 后返回 0。
// [b28d-30] 复核 DIFF：我方多 10 条（find 迭代器 cv/转换构造形，P96/P270/N82 族），
// ORIG 见 class_func_reports/user_creature__CCreatureScriptMgr/FindCreatureScript.md。
#include "CCreatureScriptMgr/FindCreatureScript.inc"
}

// ORIG RegisterCreatureIdAndItemId @ 0x833d54a。
// [b28d-30] 复核 NEAR：map 迭代器槽/转换临时差（P50/P96/N86 族），ORIG 0x0833d54a。
#include "CCreatureScriptMgr/RegisterCreatureIdAndItemId.inc"
}

}  // namespace user_creature
