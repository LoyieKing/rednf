// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_Function_Set/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// Arad_Function_Set.cpp —— 原始工程 localjapan/Arad_Function_Set.cpp 的复刻 TU。
// 收录 ARAD::PRINT_ERROR 三个静态错误上报函数与 ARAD::notifySystemMessage。
//   isEmpty       ORIG T 0x08197ba4 sz 0x59 _ZN4ARAD11PRINT_ERROR7isEmptyEPcP5CUsert
//   CheckItemLock ORIG T 0x08197bfd sz 0x59 _ZN4ARAD11PRINT_ERROR13CheckItemLockEPcP5CUsert
//   delete_item   ORIG T 0x08197c56 sz 0x59 _ZN4ARAD11PRINT_ERROR11delete_itemEPcP5CUsert
//   notifySystemMessage ORIG T 0x08197d00 sz 0xb5 _ZN4ARAD19notifySystemMessageEP5CUserPc
//
// PRINT_ERROR 三函数体完全同构：取 user 当前角色编号后经 LogManager::logFormat
// 上报（level=1，file="localjapan/Arad_Function_Set.cpp"，__PRETTY_FUNCTION__
// 静态串由编译器自动生成，对应 ORIG .rodata @0x8b9d8e0 等）。行号常量
// 0x29d/0x2a2/0x2a7 为 ORIG 硬编码源行。
// notifySystemMessage 无独立报告，按 objdump 0x8197d00 转写【推断】：
//   user/msg 任一为空直接返回；否则 PacketGuard 组包
//   GameWorld::make_packet_chat_msg(guard,user,msg,len,type=0,NOTIPACKET=0xc)
//   后 CUser::Send(guard) 发给本人（异常路径 PacketGuardD1+Unwind_Resume 由
//   编译器生成）。
// 报告：docs/class_func_reports/ARAD__PRINT_ERROR/{isEmpty,CheckItemLock,delete_item}.md
// N96 清绑（2026-09-09）：三处 `register unsigned int characNo __asm__("eax")` 已删。
// P0 依据：ORIG 无 characNo 槽（call 返 eax 直入 logFormat 出参槽 0x1c(%esp)）⇒ 还原为
// 实参内联右值形。清绑前后 our_insns 21/21/21 不变，与 ORIG 逐条全等（仅 .o 重定位地址差）
// ⇒ 残差 0，无需 N96+N18a 封顶。

#include "Arad_DataManager.h"

#include "CUser.h"
#include "CUserCharacInfo.h"    // getCurCharacNo（ORIG W 0x080cbc4e）
#include "GameWorld.h"          // G_GameWorld / make_packet_chat_msg
#include "LogManager.h"         // logFormat（ORIG T 0x08ad3c0a）
#include "PacketGuard.h"

#include <cstring>

namespace ARAD
{

// ---- PRINT_ERROR::isEmpty @ 0x08197ba4 ------------------------------------
// N96 清绑：characNo 无槽 ⇒ 实参内联右值（21=21，与 ORIG 全等）。
#include "Arad_Function_Set/isEmpty.inc"
}

// ---- PRINT_ERROR::CheckItemLock @ 0x08197bfd -------------------------------
// N96 清绑：同 isEmpty（21=21，与 ORIG 全等）。
#include "Arad_Function_Set/CheckItemLock.inc"
}

// ---- PRINT_ERROR::delete_item @ 0x08197c56 ---------------------------------
// N96 清绑：同 isEmpty（21=21，与 ORIG 全等）。
#include "Arad_Function_Set/delete_item.inc"
}

// ---- notifySystemMessage @ 0x08197d00（objdump 转写【推断】）-----------------
#include "Arad_Function_Set/notifySystemMessage.inc"
}

}  // namespace ARAD
