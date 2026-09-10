// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPrePayInfoMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CPrePayInfoMgr（LinkE1AB 批，E1-remaining B 组）。
// 符号：
//   CPrePayInfoMgr::CPrePayInfoMgr()        W 0x08231ede（map 构造 + clear）
//   CPrePayInfoMgr::InsertPrePayInfo        W 0x084ed0bc（m_map.insert(make_pair(...))）
//   G_CPrePayInfoMgr()                      W 0x08231f20（GlobalInstance<CPrePayInfoMgr>::inst_ptr）

#include "CPrePayInfoMgr.h"

#include "CEnvironment.h"   // GlobalInstance<T>（全局命名空间单例模板，CPrePayInfoMgr.h 同 TU 集）

#include "CPrePayInfoMgr/CPrePayInfoMgr.inc"
}

#include "CPrePayInfoMgr/InsertPrePayInfo.inc"
}

#include "CPrePayInfoMgr/DeletePrePayInfo.inc"
}

#include "CPrePayInfoMgr/FindPrePayInfo.inc"
}

CPrePayInfoMgr* G_CPrePayInfoMgr()
{
    return GlobalInstance<CPrePayInfoMgr>::inst_ptr();
}
