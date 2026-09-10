// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NPCDynamicInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// NPCDynamicInfo.cpp —— CNPCDynamicInfo 6 个 ORIG 方法（权威实现归本 TU）。
// 布局/头见 NPCDynamicInfo.h；ctor 归属 CharacterAIA2.cpp（W 0x0858191c）。
//
// ORIG 参照（nm -C df_game_r）：
//   changeMood 0x085804b2 T / forceChangeMood 0x0858053e T /
//   isKeyItem 0x08580584 T / giveGiftItem 0x085805a8 T /
//   makeNotiPacketCurMood 0x08580624 T / getMood 0x08581910 W（头内联）
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CNPCDynamicInfo10changeMoodEb`（ORIG 0x858186e）。
#include <iostream>


#include "NPCDynamicInfo.h"

#include "GlobalData.h"   // s_systemTime_（+0x941f714；changeMood 的 getCurSec this）
#include "PacketGuard.h"  // put_short / put_byte

// ORIG 0x085804b2 T：
//   flag 假时校验 m_fieldc <= getCurSec() 才更新；更新 = getNextMood 轮转 +
//   m_fieldc 累加当前秒。返回当前 mood（enum，直传 getMood eax）。
#include "NPCDynamicInfo/changeMood.inc"
}

// ORIG 0x0858053e T：
//   getMood()==mood 直接 false；否则 while(changeMood(true)!=mood) 空循环，返回 true。
#include "NPCDynamicInfo/forceChangeMood.inc"
}

// ORIG 0x08580584 T：转发 CNPCScript::isKeyItem，bool 直传 eax（无规范化）。
#include "NPCDynamicInfo/isKeyItem.inc"
}

// ORIG 0x085805a8 T：
//   参数布局 (uint mood, ulong unk[保留], uint count, CItem const*, result&)；
//   局部 short r：r<=0 直返；否则 r *= getFavorRatePerMood(getMood())（filds/fmuls/fistps）。
#include "NPCDynamicInfo/giveGiftItem.inc"
}

// ORIG 0x08580624 T：put_short(getIndex()) + put_byte((char)getMood())。
#include "NPCDynamicInfo/makeNotiPacketCurMood.inc"
}
