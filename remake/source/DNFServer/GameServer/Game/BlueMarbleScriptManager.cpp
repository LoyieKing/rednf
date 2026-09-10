// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BlueMarbleScriptManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ BlueMarbleScriptManager 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 BlueMarbleScriptManager.h（→ PvPTypes.h）。
// 布局依据 PvPTypes.h（m_tiles +0xbc / m_maps +0xd4，ORIG 偏移已断言）。
// getMap/getTile 为 O(1) map 查找（ORIG 0x088d66b6/0x088d6646）。

#include <map>
#include <string>

#include "BlueMarbleScriptManager.h"

// ORIG _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii... 0x088bbd77 T
//（定义 CDataManagerScripts.cpp/STScriptFileList.cpp；DNFLexWrapper.h 未含声明）。
// b28d-22/168a①：ORIG initTileList/initMapList 调用点 `call; xor $1,%eax; test %al,%al`
// ⇒ 本 TU 按 bool 视图声明（返回类型不入 mangled；定义处恒 0/1，ABI 兼容）。
#include "BlueMarbleScriptManager/loadRDARScriptListFile.inc"
#include "BlueMarbleScriptManager/BlueMarbleScriptManager.inc"
}

// ORIG 0x88d622c T：清两个文件列表 map 与 m_tiles/m_maps（不删指针、不触
// m_info——与 ORIG clear 逐段一致）。
#include "BlueMarbleScriptManager/clear.inc"
}

// ORIG 0x88d60f2：懒加载单例（new + 静态指针保存；异常路径 delete）。
#include "BlueMarbleScriptManager/getInsance.inc"
}

// ORIG 0x088d66b6 T：m_maps（this+0xd4）find，未命中返回 0。
#include "BlueMarbleScriptManager/getMap.inc"
}

// ORIG 0x088d6726 T：m_maps（this+0xd4）std::map::size。
#include "BlueMarbleScriptManager/getMapTotalCount.inc"
}

// ORIG 0x088d6646 T：m_tiles（this+0xbc）find，未命中返回 0。
#include "BlueMarbleScriptManager/getTile.inc"
}

// ============================================================================
// init* 三连（CDataManager::init 调用链；报告 BlueMarbleScriptManager/*.md）
// ============================================================================

// ORIG T 0x088d6622：直接以 m_info（this+0x30）调 importScript(dir, file)，
// 返回值透传。
// [b24F] DONE：返回类型 bool→int（ORIG call 后直接 leave;ret 透传，无 test;setne）。
#include "BlueMarbleScriptManager/initBlueMarbleInfo.inc"
}

// ORIG T 0x088d626e：loadRDARScriptListFile(&m_tileFileList, dir, list, 0)
// 失败返回 false；清空 m_tiles 后遍历文件列表，new BlueMarbleTileScript 并
// importScript(dir + 文件名)；成功入表，失败 delete。
#include "BlueMarbleScriptManager/initTileList.inc"
}

// ORIG T 0x088d6438：同 initTileList，作用于 m_mapFileList/m_maps 与
// BlueMarbleMapScript（import 失败时 ORIG 先析构再 delete）。
#include "BlueMarbleScriptManager/initMapList.inc"
}
