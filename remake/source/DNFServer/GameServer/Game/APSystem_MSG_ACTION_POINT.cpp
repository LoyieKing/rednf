// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 APSystem_MSG_ACTION_POINT/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ 迁移 TU —— APSystem::_MSG_ACTION_POINT_ACTION_CLEAR（2026-08-21）。
// 从 GameStubs.cpp 拆出。权威命名空间 APSystem（同名定义也在 APSystem.cpp，但该 TU
// 未入 game CMake；本 TU 独立提供该构造符号，保证 0 依赖、可链接）。
// ORIG 0x08122aaa：memset(this, 0, 0xb)。
#include <string.h>
#include "APSystemTypes.h"  // CDBAndExchangeServerProc 声明（CUser/MSG_CHARAC_SELECT 仅前向引用）

namespace APSystem
{
class _MSG_ACTION_POINT_ACTION_CLEAR
{
public:
    _MSG_ACTION_POINT_ACTION_CLEAR();
};
#include "APSystem_MSG_ACTION_POINT/_MSG_ACTION_POINT_ACTION_CLEAR.inc"


#include "APSystem_MSG_ACTION_POINT/RequestLoadActionPointFromDBAndES.inc"

} // namespace APSystem