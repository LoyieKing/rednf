// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmRegister/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// GmRegister.cpp — game_master::CGameMasterMgr 构造注册链完整还原。
// 对照 ORIG CGameMasterMgr::CGameMasterMgr() @0x082a8a28（size 0x18fb）：
//   senior = new CSeniorGameMaster(0)；junior = new CJuniorGameMaster(senior)；
//   随后按 ORIG 顺序注册：数字段 0x01..0x80（79 条 add(int,CCommand*)）结束后，
//   交错区实证为 "bongfail"(CSetMultiboxFailCnt) → 0x82(CBingoGmTest) → "lv"…
//   即 0x82 的 add 夹在名字注册中间，共 80 条 int + 7 条 string。
#include "GameMasterCmd.h"
#include <string>

namespace game_master
{

// b28d-solo-GmRegister 判词（verdict=AE，check_tu_routed.sh clean 口径；旧 N77 封顶作废）：
//   ① ORIG 的 119 条 value-init 前置清零（47 个 new 站点，逐 dword/word；P299/P318）：
//      删除 GameMasterCmd.h 中 47 个命令类的用户默认 ctor（改由编译器隐式合成，R2 已广播）
//      + 调用点写 `new T()`——GCC 4.4 仅在类**无用户提供 ctor** 时发射前置清零
//      （探针 /tmp/b28d_gmreg_probe/p1/p3/p4 实测）；另删 CPowerWarStraightVictory::m_pad[2]
//      （P26/P312：ORIG 该站点只发一条 movw，显式 pad 会被多清 2B）。
//   ② senior/junior 字段改基类指针 IGameMaster*（P211 异型化）：复现 ORIG
//      `mov %ebx,%eax; mov %eax,%edx`（派生→基赋值中转）与 `mov (%eax),%edx`（实参单段）。
//   验收：gap 0（1609=1609）；p167 调用点差集 CLEAR（318/318，0 missing/0 extra）；
//   our_insns 1489→1609；87 注册块 (new尺寸↔类↔id/串↔顺序) 与 ORIG 逐块全等。
#include "GmRegister/CGameMasterMgr.inc"
}

} // namespace game_master
