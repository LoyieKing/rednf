// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPacketRespondent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— user_creature::CPacketRespondent。
// 逐方法依据 ORIG objdump 反汇编转写（地址见 CPacketRespondent.h 注释；
// ORIG 全部为 W 内联符号，本 TU 提供强定义）。基类 PacketGuard 由
// ctor/dtor 调 _ZN11PacketGuardC1/D1Ev 实证（0x0834032a/0x0834036a），
// 非直接 InterfacePacketBuf（InterfacePacketBuf 子对象经 PacketGuard 仍在 +0）。
// ============================================================================
#include <string.h>

#include "CPacketRespondent.h"

#include "CUser.h"
#include "CUserCharacInfo.h"   // getCurCharacVill
#include "GameWorld.h"         // G_GameWorld/send_all/send_to_area/getUserVillage
#include "Village.h"           // Village::get_gate_area
#include "LogManager.h"        // cMyTrace
#include "CGameManager.h"      // user_creature::CCreature 访问器
#include "CInventory.h"        // user_creature::CCreatureItem

namespace user_creature
{

// ORIG C1 @ 0x0834032a：PacketGuard 基类构造后依次写
// +0x0c=user、+0x10=header、+0x14=subHeader、+0x18=0、+0x1c(byte)=0。
#include "CPacketRespondent/CPacketRespondent.inc"
}

// ORIG D1 @ 0x0834036a：m_autoSend 非零则补发一次 Send，随后 ~PacketGuard()。
#include "CPacketRespondent/dtor_CPacketRespondent.inc"
}

// ORIG finalize @ 0x083403b8：+0x1c=1 后转发 InterfacePacketBuf::finalize(bool)。
#include "CPacketRespondent/finalize.inc"
}

// ORIG Send @ 0x083403e0：+0x1c=0；CUser::Send(*this) 结果透传。
// [b28d-solo-CPacketRespondent P248档①] ORIG 尾 `call CUser::Send; leave; ret` 无 movzbl
// ⇒ 与 callee 同型 bool（CUser::Send 返回 bool，__PRETTY_FUNCTION__ 串 "bool CUser::Send"）。
// 声明 int 时 4.4 插 `movzbl %al,%eax`（4.1 不插）——改 bool 即 identical。
#include "CPacketRespondent/Send.inc"
}

// ORIG Broadcast @ 0x08340404：
//   +0x1c=0；vill==0 && area==0 → 返回 0；
//   IsGateArea(m_user, area) → m_user->Send(*this)；
//   否则 G_GameWorld()->send_to_area(vill, area, *this)；返回 1。
// [b28d-solo-CPacketRespondent P248档①] 同 Send：gate 路径 `call Send; jmp 尾` 直通，
// 与 CUser::Send(bool) 同型；ORIG 0x08340404 尾 `mov $0x1,%eax` / `mov $0x0,%eax` 两臂。
#include "CPacketRespondent/Broadcast.inc"
}

// ORIG SendAll @ 0x0834048a：+0x1c=0；G_GameWorld()->send_all(*this)；返回 1。
#include "CPacketRespondent/SendAll.inc"
}

// ORIG IsGateArea @ 0x08340256：GameWorld/村空时按 __PRETTY_FUNCTION__ 打点并返 0；
// 否则比较 Village::get_gate_area(village) == area。
// b28d-40: P225 while 形复现 sete/test/je 物化 + P221 全表达式临时链翻 sibling
// trace 槽序（-0x30 深 / -0x20 浅）；ORIG 0x08340256，AE。
#include "CPacketRespondent/IsGateArea.inc"
}

// ORIG SendAccordingToPlace @ 0x083404b8：
// state==5 直发本人；否则 area=get_area(false)、vill=(signed char)getCurCharacVill()
// 后 Broadcast(vill, area)。返回值未使用（void）。
#include "CPacketRespondent/SendAccordingToPlace.inc"
}

// ORIG SetError @ 0x08340538：clear → put_header(+0x10,+0x14) → put_byte(0)
// → put_byte(errCode) → finalize(true)。
#include "CPacketRespondent/SetError.inc"
}

// ORIG SetOk @ 0x083405a2：clear → put_header(+0x10,+0x14) → put_byte(1)
// → finalize(true)。
#include "CPacketRespondent/SetOk.inc"
}

// ORIG MakeCmdpacketRenameCreature @ 0x083405fa：clear → put_header(1,0x67)
// → put_byte(1) → put_short(param1) → put_byte(param2) → finalize(true)。
#include "CPacketRespondent/MakeCmdpacketRenameCreature.inc"
}

// ORIG MakeNotipacketRenameCreature @ 0x08340672：clear → put_header(0,0x65)
// → put_short(uid 低 16 位) → put_int(len) → put_str(name,len) → finalize(true)。
#include "CPacketRespondent/MakeNotipacketRenameCreature.inc"
}

// ORIG MakeNotipacketDiedCreature @ 0x083406fe：clear → put_header(0,0x64)
// → put_short(uid16) → finalize(true)。
#include "CPacketRespondent/MakeNotipacketDiedCreature.inc"
}

// ORIG MakeNotipacketGainExpCreature @ 0x08340760：clear → put_header(0,0x66)
// → put_byte(level)；
//   growFlag：put_byte(1)、put_int((int16)(exp>>16))、put_byte(10)、
//             put_byte(exp 低字节)；
//   else    ：put_byte(0)、put_int(exp)；
// 最后 finalize(true)。
#include "CPacketRespondent/MakeNotipacketGainExpCreature.inc"
}

// ORIG MakeNotipacketEvoluteCreature @ 0x08340842：clear → put_header(0,0x6a)
// → put_byte(slot) → put_short(uid16) → finalize(true)。
#include "CPacketRespondent/MakeNotipacketEvoluteCreature.inc"
}

// ORIG MakeNotipacketRevivalCreature @ 0x083408b6：clear → put_header(0,0x6b)
// → put_short(uid16) → finalize(true)。
#include "CPacketRespondent/MakeNotipacketRevivalCreature.inc"
}

// ORIG MakeNotipacketCreatureState @ 0x08340918：clear → put_header(0,0x67)
// → put_int(uid) → put_int(state) → finalize(true)。
#include "CPacketRespondent/MakeNotipacketCreatureState.inc"
}

// ORIG MakeNotipacketResponseCreature @ 0x0834097e：clear → put_header(0,0x68)
// → put_short(uid16) → finalize(true)。
#include "CPacketRespondent/MakeNotipacketResponseCreature.inc"
}

// ORIG MakeNotipacketCreatureItemList @ 0x083409e0：
//   clear → put_header(0,0x69)；index=get_index() 后 put_byte(count) 占位计数；
//   遍历 map 值（post-increment 迭代）：仅当 item 非空且 GetCreatureItemType()==1
//   且 (slot==-1 || GetSlotNo()==slot) 时依次
//     put_int(GetUid)、put_byte(CCreature::GetStomach)、
//     IsGrowCreature ? [put_byte(1), put_int((int16)(exp>>16)), put_byte(10),
//                       put_byte(exp&0xff)]
//                   : [put_byte(0), put_int(GetExp)]、
//     put_byte(GetLevel)、strlen(name)+put_int+put_str、put_byte(IsNoCharge)，
//     计数 ++；
//   循环结束 put_byte(index, count) 回填占位字节，finalize(true)。
//
// [b28d-solo-CPacketRespondent P183/P0/P296] ORIG 0x083409e0 实证：
// ① 无 creature 具名局部——每处访问均 `mov -0xc(%ebp),%eax` 重读 item 后直呼
//    CCreature 访问器（单继承布局同址；镜像类型 reinterpret_cast 保持同一调用符号）；
// ② nameLen 在函数头预清（`movl $0x0,-0x14(%ebp)`）+ 循环内赋值（P198 两句式）；
// ③ 占位 put_byte 读 count 变量（`mov -0x10(%ebp),%edx`），非立即数 0；
// ④ 三跳过条件为正向包裹：4.1 产 `sete;test;je 尾` / 短路物化 `mov $1/$0;test;je 尾`；
//    写 `continue` 使 4.4 产 setne/jne + nop;jmp 中继岛（P296）。
#include "CPacketRespondent/MakeNotipacketCreatureItemList.inc"
}

}  // namespace user_creature
