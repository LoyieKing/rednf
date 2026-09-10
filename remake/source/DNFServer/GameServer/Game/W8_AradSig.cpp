// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W8_AradSig/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W8_Mid.AradSigs —— arad 域 38 个仅-ORIG 符号（/tmp/only_orig/arad.txt）
// 对应类布局声明在 MoreTypes.h（namespace arad），实现转写自 df_game_r ORIG
// 反汇编；每个函数的 ORIG 地址见行内注释。全部具名字段访问。
// 需要登记 CMake 的文件：Game/W8_AradSig.cpp
// ============================================================================
#include "MoreTypes.h"

#include <cstring>

namespace arad {

// ---------------------------------------------------------------------------
// SigServerState::tagData C1/C2 —— ORIG W 0x0818670c：
//   mov 0x8(%ebp),%eax; add $0x8,%eax; call AradServerStateMessage::Ctor(0x8186652)
// 即对成员 m_msg（tag 内 +0x08）执行 AradServerStateMessage 默认构造；
// C++ 中空体用户构造函数自动完成同样的成员对象构造。
// ---------------------------------------------------------------------------
#include "W8_AradSig/tagData.inc"
#include "W8_AradSig/SigAvatarRoulette.inc"
}

#include "W8_AradSig/init.inc"
}

// ---------------------------------------------------------------------------
// SigAradEventItemLog —— ORIG W C1/C2 0x8694f14 -> init() 0x8694f28：
//   movl $0x0,(%eax)[m_itemId]; movl $0x0,0x4(%eax)[m_itemCount]
// 仅清前两 dword（m_id/m_characNo 不初始化，与 ORIG 一致）。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradEventItemLog.inc"
}

#include "W8_AradSig/init_1.inc"
}

// ---------------------------------------------------------------------------
// SigAradAvatarConvert —— ORIG W C1/C2 0x081a0a6a -> init() 0x081a0a7e：
//   movw $0x0,(%eax)[m_selectNo]; movw $0x0,0x2(%eax)[m_slot];
//   movl $0x0,0x4(%eax)[m_itemId]; movl $0x0,0x8(%eax)[m_resultItemId];
//   movw $0x0,0xc(%eax)[m_resultCount]
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradAvatarConvert.inc"
}

#include "W8_AradSig/init_2.inc"
}

// ---------------------------------------------------------------------------
// SigAradEmblemCompound::materialInfo::init —— ORIG W 0x081a0a0c：
//   movw $0x0,(%eax)[slot]; movw $0x0,0x2(%eax)[grade]; movl $0x0,0x4(%eax)[itemIdx]
// ---------------------------------------------------------------------------
// P210：SigAradEmblemCompound::init 定义须先于本 callee，否则 4.4 按 materialInfo::init
// 帧精确继承 outgoing（caller 帧 0x18→0x4 与 ORIG 0x18 反）。
// ---------------------------------------------------------------------------
// SigAradEmblemCompound —— ORIG W C1/C2 0x081a0a2c -> init() 0x081a0a40：
//   sub $0x18（ORIG 保守 outgoing 帧）
//   movw $0x0,0x14(%eax)                        // 清 m_selectIdx
//   call materialInfo::init(this)               // 清 {m_slot0,m_grade0,m_itemId0}
//   eax += 0x8; call materialInfo::init(this+8) // 清 {m_slot1,m_grade1,m_itemId1}
// 注意 ORIG 不清 m_resultIdx(+0x10)。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradEmblemCompound.inc"
}

#include "W8_AradSig/init_3.inc"
}

#include "W8_AradSig/init_4.inc"
}

// ---------------------------------------------------------------------------
// SigLoadRewardEventItem —— ORIG W 全套：
//   C1/C2 0x08186686 -> init() 0x0818669a -> set(0) 0x081866b6；
//   set(v)：m_field0=v；m_count=0；memset(m_characNos,0,0x28)；
//           memset(m_eventCodes,0,0x28)
// （memset 先 characNos 区后 eventCodes 区，次序照 asm）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigLoadRewardEventItem.inc"
}

#include "W8_AradSig/set.inc"
}

#include "W8_AradSig/init_5.inc"
}

// ---------------------------------------------------------------------------
// SigSaveRewardEventItem —— ORIG W C1/C2 0x08195cd4：调 init()（定义位于
// Arad_InternalDispatcher.cpp）；本 TU 仅提供 ctor。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigSaveRewardEventItem.inc"
}

// ---------------------------------------------------------------------------
// SigSetCharacCreateTime —— ORIG W：
//   C1/C2 0x0822ee3c -> init() 0x0822ee50 -> set(0,0) 0x0822ee74；
//   set(a,b)：+0=m_characNo=a、+4=m_createTime=b（两个 dword 直写）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigSetCharacCreateTime.inc"
}

// P210：init 定义先于 set（ORIG init 帧 0x18，4.4 若先见 set 定义则继承其小帧）。
#include "W8_AradSig/init_6.inc"
}

#include "W8_AradSig/set_1.inc"
}

// ---------------------------------------------------------------------------
// SigAradJumpingCharacter —— ORIG W C1/C2 0x081a0990 -> init() 0x081a09a4：
//   movb $0x0,(%eax)          [m_head[0]]
//   movl $0x0,0x1(%eax)       [m_head[1..4]]
//   movl $0x0,0x5(%eax)       [m_head[5..8]]
// 仅清 m_head 九字节（memcpy 常量定长在 -O0 展开为同形态写入）；
// eventKind/growType/itemIndex/tail 不初始化。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
// b28：fY 探针（/tmp/bl14_p3.o）实证 pack(1) 具名字段视图直赋复现 ORIG `movl $0x0,0x1(%eax)`
// disp 直存形；`*(unsigned*)(m_head+K)=0` 指针算术形 4.4 恒拆 add+movl（+2 条）。
struct SigAradJumpingInitView
{
    char m_h0;
    unsigned int m_head1;
    unsigned int m_head5;
};
#pragma pack(pop)

#include "W8_AradSig/SigAradJumpingCharacter.inc"
}

#include "W8_AradSig/init_7.inc"
}

// ---------------------------------------------------------------------------
// SigAradUseAvatarRecharge —— ORIG W init() 0x081a09c4 / C1 0x081a09f8：
//   清 avatarSlot/stoneSlot/addEndurance/maxEndurance/enduranceAfter 五字段。
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradUseAvatarRecharge.inc"
}

#include "W8_AradSig/init_8.inc"
}

// ---------------------------------------------------------------------------
// SigAradAvatarEnduranceSave —— ORIG W init() 0x081933de / C1 0x081933f6：
//   movl $0x0,(%eax)[uiId]; movl $0x0,0x4(%eax)[endurance]
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradAvatarEnduranceSave.inc"
}

#include "W8_AradSig/init_9.inc"
}

// ---------------------------------------------------------------------------
// SigAvatarHiddenOptionChange —— ORIG W C1/C2 0x081a095c -> init() 0x081a0970：
//   清三个 word（m_materialSlot/m_avatarSlot/m_hiddenOption）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAvatarHiddenOptionChange.inc"
}

#include "W8_AradSig/init_10.inc"
}

// ---------------------------------------------------------------------------
// SigAradAvatarHiddenOptionSave —— ORIG W C1/C2 0x0817edfc -> init() 0x0817ee10：
//   清 m_field0/m_characNo/m_itId（dword×3）与 m_slot/m_hiddenOption（word×2）
// ---------------------------------------------------------------------------
#include "W8_AradSig/SigAradAvatarHiddenOptionSave.inc"
}

#include "W8_AradSig/init_11.inc"
}

}  // namespace arad
