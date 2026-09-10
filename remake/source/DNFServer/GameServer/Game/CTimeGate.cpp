// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CTimeGate/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN9CTimeGate15put_object_infoER11PacketGuard`（ORIG 0x8284584）。
#include <iostream>

// ============================================================================
// CTimeGate（G2 时间之门批次）
// ----------------------------------------------------------------------------
// ORIG（弱/强符号）：
//   ctor                            _ZN9CTimeGateC1Ev   0x8284512
//     → IObject::IObject() 基类 -> 置 CTimeGate vptr(0x8c0beb8) -> movb $0,+0x04
//   dtor                            _ZN9CTimeGateD1Ev   0x8284536
//     → 仅重设 vptr(0x8c0beb8)；基类 IObject 析构为空（不调用）
//   put_object_info                 _ZN9CTimeGate15put_object_infoER11PacketGuard 0x82844d8
//     → InterfacePacketBuf::put_byte(2);
//       put_byte((int)(char)+0x04 /*m_openState*/); return 1
//   include（vtable slot0；ORIG 符号名 include）         W 0x82845a0 → return 0
//   isOpen                          _ZNK9CTimeGate6isOpenEv   0x8234e1c
//     → movzbl +0x04; cmp $0x64; 即 return (m_openState == 0x64)；bool 返回
//       （P22：调用处 !isOpen() 以 xor $1 取反）
//   setBlindTimeGateStateQuest      _ZN9CTimeGate26setBlindTimeGateStateQuestEi 0x8234e46
//     → 两段各自物化为 bool 再判定：
//       bool t1 = (getQuestIndex()==questIdx && isOpen());
//       if (t1) return true;
//       bool t2 = (getOpenedQuestIndex()==questIdx && !isOpen());
//       if (t2) return true;
//       return false;
//   SetTimeGate                     _ZN9CTimeGate11SetTimeGateEiiiii 0x86d2788
//     → +0x08=a(village) +0x0c=b(area) +0x10=c(npc) +0x14=d(quest) +0x18=e(opened)
//   SetLevel / 5 个 getter          W（独立符号，mov 直读字段）
// 布局见 CTimeGate.h（继承 village_object::IObject，vptr@+0，m_openState@+4）。
// ============================================================================

#include "CTimeGate.h"

#include "PacketGuard.h"   // put_byte（InterfacePacketBuf）

// ORIG 0x8284512：IObject 基类 ctor -> 置 CTimeGate vptr -> m_openState=0。
// C++ 中基类构造、vptr 覆写由编译器自动完成，函数体只需置 m_openState=0。
// [A31-GI · P327] put_object_info 定义前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN9CTimeGate15put_object_infoER11PacketGuard`（ORIG 0x8284584）。
// ORIG 0x82844d8：put_byte(2); put_byte(m_openState 零扩展); return 1。
#include "CTimeGate/put_object_info.inc"
}

#include "CTimeGate/CTimeGate.inc"
}

// ORIG 0x8284536：仅重设 vptr（基类 IObject 析构为空，不调用）。
#include "CTimeGate/dtor_CTimeGate.inc"
}

// ORIG W 0x82845a0：vtable slot0 的 include 覆写（IObject 纯虚）；函数体
// return 0（恒不碰撞）。ORIG 无 CTimeGate::reset 符号。
#include "CTimeGate/include.inc"
}

#include "CTimeGate/SetTimeGate.inc"
}

// ORIG W 0x84ed4a4：movb %?,+0x04（level 截断为 unsigned char）。
#include "CTimeGate/SetLevel.inc"
}

// ORIG 0x8234e46：两段条件各自物化为 bool 再判定（见文件头注释）。
#include "CTimeGate/setBlindTimeGateStateQuest.inc"
}

#include "CTimeGate/isOpen.inc"
}

#include "CTimeGate/getVillageIndex.inc"
}

#include "CTimeGate/getAreaIndex.inc"
}

#include "CTimeGate/getNpcIndex.inc"
}

#include "CTimeGate/getQuestIndex.inc"
}

#include "CTimeGate/getOpenedQuestIndex.inc"
}
