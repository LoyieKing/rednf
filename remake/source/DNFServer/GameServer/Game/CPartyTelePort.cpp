// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyTelePort/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <vector>
#include <cstring>
#include "CPartyTelePort.h"
#include "CParty.h"
#include "CUser.h"
#include "GameWorld.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "GlobalData.h"
#include "PacketGuard.h"

// [b28d-solo-CPartyTelePort] 类型名必须与 ORIG nm 一致：ORIG 的
// vector<effect_data> 相关符号为 `_ZNSt6vectorI11effect_dataSaIS0_EE*`
// （P0/N114：符号名不得自造）。
struct effect_data
{
    char village;
    int area;
    std::vector<unsigned short> users;
};
// [b28d-solo-CPartyTelePort][N89/N103] vector<effect_data> 扩容路径实例
// `_ZNSt20__uninitialized_copyILb0EE18uninitialized_copyISt13move_iteratorI
//  P11effect_dataES4_EE...`：ORIG(4.1 libstdc++) 内联 `::new` placement +
// `effect_dataC1ERKS_`（67 条，0x085c382e）；4.4 libstdc++ 改调
// `_ZSt10_ConstructI11effect_dataS0_EvPT_OT0_` helper（47 条）⇒ 库头实现差，
// 源侧不可复现（禁改 libstdc++ 头 / 禁发明替身符号）。语义等价。

// ============================================================================
// CPartyTelePort（sizeof 0x24）
// ============================================================================

// ============================================================================
// 实现
// ============================================================================
// ---- stPartyTelePort（ORIG 0x822d764，memset 0，清 4 字节状态头）----

#include "CPartyTelePort/reset.inc"


#include "CPartyTelePort/get_teleport_village.inc"


#include "CPartyTelePort/get_teleport_area_index.inc"


#include "CPartyTelePort/get_teleport_posx.inc"


#include "CPartyTelePort/get_teleport_posy.inc"


#include "CPartyTelePort/get_teleport_direction.inc"


#include "CPartyTelePort/CPartyTelePort.inc"


#include "CPartyTelePort/dtor_CPartyTelePort.inc"


#include "CPartyTelePort/init.inc"


#include "CPartyTelePort/set_teleport_data.inc"


#include "CPartyTelePort/set_teleport_state.inc"


#include "CPartyTelePort/get_teleport_member_state.inc"


#include "CPartyTelePort/set_teleport_member_state.inc"


#include "CPartyTelePort/get_teleport_village_1.inc"


#include "CPartyTelePort/get_teleport_area_index_1.inc"


#include "CPartyTelePort/get_teleport_posx_1.inc"


#include "CPartyTelePort/get_teleport_posy_1.inc"


#include "CPartyTelePort/get_teleport_direction_1.inc"


#include "CPartyTelePort/get_teleport_state.inc"


#include "CPartyTelePort/send_teleport_status.inc"


#include "CPartyTelePort/reset_teleport_data.inc"


#include "CPartyTelePort/check_cur_teleport_state.inc"


#include "CPartyTelePort/process_teleport.inc"


#include "CPartyTelePort/process_leave_user_at_teleport.inc"


#include "CPartyTelePort/send_teleport_effect_at_different_place.inc"


#include "CPartyTelePort/send_teleport_effect_at_equal_place.inc"

