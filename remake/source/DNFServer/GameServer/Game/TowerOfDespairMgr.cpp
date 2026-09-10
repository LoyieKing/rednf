// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TowerOfDespairMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// TowerOfDespairMgr.cpp —— TOD 域首批叶子实现（PartyDepsB2 批次）。
// 逐指令依据：
//   TOD_Layer::TOD_Layer            docs/class_func_reports/TOD_Layer/TOD_Layer.md
//   TowerOfDespairMgr::SendAPCInfo  docs/class_func_reports/TowerOfDespairMgr/SendAPCInfo.md
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN9TOD_LayerC2Et`（ORIG 0x85ff0dd）；TOD_Layer ctor 已是首强符号。
#include <iostream>

#include "TowerOfDespairMgr.h"
#include <string.h>

#include "GameTypes.h"   // cMyTrace(char const*, int, int) / operator()(char const*, ...)

#include "TowerOfDespairMgr/TOD_Layer.inc"
}

#include "TowerOfDespairMgr/SendAPCInfo.inc"
}

// TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex —— ORIG T 0x08644272
// [0x08644272,0x086442c9]：m_layerSet(+0x328, set<int>).find(idx) != end()。
// const 成员：find/end 返回 _Rb_tree_const_iterator（与报告一致），
// operator!= 结果直返（无规范化，返回类型 bool）。
// 注意：find 必须独立语句先行——单语句 `find(idx) != end()` 被 gcc
// 从右向左求值（先 end 后 find），与 ORIG 指令序相反；
// 且 ORIG operator!= 的 this=end()、实参=it（写 `end() != it`）。
#include "TowerOfDespairMgr/IsTowerOfDespairDungeon_ByIndex.inc"
}

// SendingAPC_Info::set_data —— ORIG T 0x085fe8e0 [0x085fe8e0,0x085fe905]
// rep movsl 0x2f 次（0xbc 字节）到 this+0x17338（m_apcInfo）。
#include "TowerOfDespairMgr/set_data.inc"
}

// TowerOfDespair_APC_Mgr::Set_APC_Info —— ORIG T 0x085fed2e [0x085fed2e,0x085feecf]
// map find；命中→复用条目 set_data；未命中→构造局部 SendingAPC_Info + set_data
// 后 make_pair/insert，随后析构局部（try 块内 set_data/insert 链）。
#include "TowerOfDespairMgr/Set_APC_Info.inc"
}

#include "TowerOfDespairMgr/Reset_APC_Info.inc"
}

// TowerOfDespairMgr::ResetAPCInfo —— ORIG T 0x0864431e [0x0864431e,0x08644337]
// 转发 +0x354 APC 管理器 Reset_APC_Info（mov 0x354(%eax),%eax; call）。
#include "TowerOfDespairMgr/ResetAPCInfo.inc"
}

// TowerOfDespairMgr::SetAPCInfo —— ORIG T 0x086442ca [0x086442ca,0x0864431d]
// vector<unsigned int>::operator[](m_npcLayers, idx) 构造 TOD_Layer，
// 转发 +0x354 APC 管理器 Set_APC_Info。
#include "TowerOfDespairMgr/SetAPCInfo.inc"
}

// TowerOfDespairMgr::Reload_APC_Info —— ORIG T 0x08644360 [0x08644360,0x08644365]
// 空函数：push/mov/pop/ret（无成员访问），语义为空体。
#include "TowerOfDespairMgr/Reload_APC_Info.inc"
}

// TowerOfDespairMgr::_layerInfo —— ORIG T 0x08644470 [0x08644470,0x08644474]
// 空函数：push/mov/pop/ret（无成员访问），语义为空体。
#include "TowerOfDespairMgr/_layerInfo.inc"
}
