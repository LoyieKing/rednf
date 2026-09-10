// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyTelePort/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ==========================================================================
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

// ORIG W 0x0822d764：memset(this,0,4)
#include "CPartyTelePort/reset.inc"
}

#include "CPartyTelePort/get_teleport_village.inc"
}

#include "CPartyTelePort/get_teleport_area_index.inc"
}

#include "CPartyTelePort/get_teleport_posx.inc"
}

#include "CPartyTelePort/get_teleport_posy.inc"
}

#include "CPartyTelePort/get_teleport_direction.inc"
}

#include "CPartyTelePort/CPartyTelePort.inc"
}

#include "CPartyTelePort/dtor_CPartyTelePort.inc"
}

#include "CPartyTelePort/init.inc"
}

#include "CPartyTelePort/set_teleport_data.inc"
}

#include "CPartyTelePort/set_teleport_state.inc"
}

#include "CPartyTelePort/get_teleport_member_state.inc"
}

#include "CPartyTelePort/set_teleport_member_state.inc"
}

#include "CPartyTelePort/get_teleport_village_1.inc"
}

#include "CPartyTelePort/get_teleport_area_index_1.inc"
}

#include "CPartyTelePort/get_teleport_posx_1.inc"
}

#include "CPartyTelePort/get_teleport_posy_1.inc"
}

#include "CPartyTelePort/get_teleport_direction_1.inc"
}

#include "CPartyTelePort/get_teleport_state.inc"
}

#include "CPartyTelePort/send_teleport_status.inc"

}

#include "CPartyTelePort/reset_teleport_data.inc"
}

#include "CPartyTelePort/check_cur_teleport_state.inc"
}

// [b28d-solo-CPartyTelePort][N] 语义完成，byte 层跨版本封顶：
// ① 返回值归一化：ORIG 0x085bc034 `movzbl -0x35(%ebp),%eax`（4.1 直返 char），
//    4.4 对 `return <char>;`（函数返回 bool）恒发 `cmpb $0,+setne %al`
//    （/tmp/rp.cpp g1..g6 六形探针全归一化）；返回类型由 ORIG
//    __PRETTY_FUNCTION__ 串 "bool CPartyTelePort::process_teleport()"
//    （.rodata 0x08cbb220）锁定，不可改类型。
// ② 槽分配策略（N82附）：ORIG dir/posy 落深槽 -0x78/-0x74（第二循环
//    -0x70/-0x6c），4.4 复用于 -0x38/-0x34（-0x28/-0x24），cMyTrace 反向；
//    声明序/位置调整无效（4.4 聚合优先低槽 + 标量分区）。
// 其余指令序列（归一化地址/槽号后）与 ORIG 逐条一致；p167 调用点差集 CLEAR。
#include "CPartyTelePort/process_teleport.inc"
}

#include "CPartyTelePort/process_leave_user_at_teleport.inc"
}

// [b28d-solo-CPartyTelePort] AE（check_tu_routed 口径）：ORIG 0x085bc2b0。
// 修法：struct 名 TeleportEffect→effect_data（P0/N114 符号名）；内层迭代器
// begin/end 缓存（P13）；effect_data 逐字段赋值（P54，area 先于 village）；
// 第二个守卫改正向包裹（P222）消 4.4 多余 nop+jmp 岛；第二循环 int i（setg）。
// 残余仅重定位地址与 .o 阶段立即数显示（N124/P264）；p167 调用点差集 CLEAR。
#include "CPartyTelePort/send_teleport_effect_at_different_place.inc"
}

// [b28d-solo-CPartyTelePort] AE（check_tu_routed 口径）：ORIG 0x085bc64c。
// 修法：area/vill 去具名局部改实参内联（P0/P52，ORIG 用 %ebx/%esi 无槽）；
// 双 continue 守卫（P223 同层多 continue 岛）+ `!checkValidUser` xor 形；
// p167 调用点差集 CLEAR。
#include "CPartyTelePort/send_teleport_effect_at_equal_place.inc"

}
