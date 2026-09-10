// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinkClosureA_Data/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkClosureA_Data.cpp — CDataManager PvP 组 getter（undef_bizA 收口批 A）
//
// 覆盖符号（唯一 owner = 本 TU；报告缺失 → nm -S 定址 objdump 忠实转写）：
//   CDataManager::GetPvPMode        W 0x0844d4ae（0xe 字节）
//   CDataManager::GetPvPTeamCount   W 0x08568ce0（0xe 字节）
//
// ORIG 汇编均为 push %ebp / mov %esp,%ebp / mov 0x8(%ebp),%eax /
// mov disp(%eax),%eax / leave / ret 六指令形态：直接返回成员字段。
// ORIG disp：GetPvPMode=+0xb44c、GetPvPTeamCount=+0xb450；
// CDataManager.h 布局 m_pvpChannel(+0xb430, pvp_channel_info_t) 起始,
// 两偏移分别对应该结构 +0x1c(m_field1c)/+0x20(m_field20)
// （importPVPChannelList 0x08901f10 扫描序 v8/v9，头注释实证）。
// ============================================================================

#include "CDataManager.h"

#include "LinkClosureA_Data/GetPvPMode.inc"
}

#include "LinkClosureA_Data/GetPvPTeamCount.inc"
}

// ==== QuestParameterScript 三 getter（ORIG T 0x08a6bd1c/0x08a6bd86/0x08a6bda6）====
// getRewardWeight：map<char,int> find，未命中返 100；exp/gold：vector::at 抛越界。
#include "LinkClosureA_Data/getRewardWeight.inc"
}

#include "LinkClosureA_Data/getRewardExp.inc"
}

#include "LinkClosureA_Data/getRewardGold.inc"
}
