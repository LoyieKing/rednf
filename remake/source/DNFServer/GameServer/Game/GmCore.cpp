// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GmCore.cpp — game_master 命令体系核心框架（第1批）。
// 还原：IGameMaster/CJunior/CSenior 责任链、CCommand 基类、CParser::parse、
//       CCommandMap 双索引注册表、CGameMasterMgr 分发入口。
// 全部函数体对照 df_game_r ORIG 汇编（docs/class_func_reports/game_master__* 与 objdump）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "PacketDispatcher.h"

extern void trim(std::string& s);   // ORIG _Z4trimRSs @0x0829f068

namespace game_master
{

// ---- IGameMaster ----


#include "GmCore/HandleRequest.inc"
}



#include "GmCore/HandleRequest_1.inc"
}



#include "GmCore/HandleRequest_2.inc"
}

// ---- CCommand ----


#include "GmCore/SetParser.inc"
}

#include "GmCore/GetUser.inc"
}

#include "GmCore/executeTemplate.inc"
}

// ---- CParser ----
#include "GmCore/CParser.inc"
}

#include "GmCore/dtor_CParser.inc"
}

// [b28d-solo-GmCore] 具名块级局部（head/tail）令 4.4 槽序反转为 -0xc/-0x10；改
// **匿名表达式临时**（substr 结果直接赋给成员）后 4.4 复现 ORIG 的 -0x10/-0xc
// 与块尾析构点（探针 fF/fH 实证；具名→匿名是槽向旋钮，语义不变）。
#include "GmCore/parse.inc"
}

// ---- CCommandMap ----
#include "GmCore/CCommandMap.inc"
}

// [b28d-solo-GmCore] AE | ORIG 0x082a7d88 | 双容器（map+unordered_map）迭代器析构
// 循环逐条全等（含两段 EH landing）；show_func_diff ndiff=0，仅 .o 未解析重定位
// 显示差（call 目标）属 AE 归一范围。
#include "GmCore/dtor_CCommandMap.inc"
}

#include "GmCore/add.inc"
}

#include "GmCore/add_1.inc"
}

#include "GmCore/get.inc"
}

#include "GmCore/get_1.inc"
}

// ---- CGameMasterMgr ----
#include "GmCore/GetDispatcher.inc"
}

#include "GmCore/HandleRequest_3.inc"
}

#include "GmCore/GetCommand.inc"
}

#include "GmCore/GetCommand_1.inc"
}

} // namespace game_master
