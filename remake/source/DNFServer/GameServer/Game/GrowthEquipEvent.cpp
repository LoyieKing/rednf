// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GrowthEquipEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ GrowthEquipEvent 实现。
// 依据 docs/class_func_reports/GrowthEquipEvent/*.md + ORIG elf 反汇编：
//   ctor 0x080f6ef0：CEventBase() + 本类 vptr（无自有成员）
//   D1 0x080f6f0c / D0 0x080f6f3c：恢复 vptr 后调基类析构
//   StartEvent 0x080f6f5a / StartEvent(Word_Param) 0x080f6fac /
//   EndEvent 0x080f6ffe：SetEventFlag + cMyTrace
//   StartAction 0x080f7710：遍历世界用户表，state>2 者补发活动信息
//   IsEventing 0x080f7820（W）：转发 CEventBase::IsEventing
//   SelectGrowthWeapon 0x080f73be / SendGrowthWeaponEventInfo 0x080f7226
//   GiftGrowthWeaponBox 0x080f7050（礼包邮件 + DB_EventSaveGrowthWeapon）
// ============================================================================
// [b28d-55] 本 TU 判词表（verdict = game_func_report --no-compile，fresh .o）：
//   StartAction 0x080f7710           DIFF(51/51) 修法：非 const _Rb_tree_iterator
//     + `it++` 后置（P50/P96/P28）+ 去 world 具名局部、G_GameWorld() 内联进
//     begin/end 表达式（P0/P183）；P167 CLEAR(9/9)。
//   SendGrowthWeaponEventInfo 0x080f7226 DIFF(117/116) P167 CLEAR(22/22)；
//     残差 = P77 short→int 拓宽实参形 + 帧槽差（N82 族）。
//   GiftGrowthWeaponBox 0x080f7050   DIFF(115/116) P167 EXTRA ×1：
//     CEnvironment::get_server_group 多 1 次（ORIG 该路径不调）——待后续核对。
// ============================================================================
#include "GrowthEquipEvent.h"

#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"
#include "DB_EventUpdateGrowthWeaponCharac.h"
#include "DB_EventSaveGrowthWeapon.h"
#include "Inven_Item.h"
#include "RDARScriptStringManager.h"
#include "CUserCharacInfo.h"
#include "CMailBoxHelper.h"

#include <cstring>
#include "cMyTraceNoop.h"

#include "GrowthEquipEvent/GrowthEquipEvent.inc"
}

// ============================================================================
// GrowthEquipEvent::SelectGrowthWeapon @ 0x080f73be
// 使用成长武器道具（Dispatcher_UseBoosterItem::_onBoosterItemUseResult_OnlyOne）：
// ORIG 守卫比较 CDataManager+0x4bc4 == itemId。该字段即 m_growthWeapon.m_giftBox
// （get_GiftGrowthWeaponBox 0x836561c 读同一偏移，CDataManager.h 布局 +0x4bac+0x18），
// 用具名字段替代裸偏移。
// ============================================================================
// [b23判词·N-PTR] P167 调用点差集 CLEAR（13/13）：ORIG 无 CSE，getGrowthWeaponEventdata()
// 调 3 次（已内联，不缓存 data）、serverGroup 走 [G_CEnvironment()+0x378] 直读成员（无
// get_server_group() call）。残余 gap+2 = ORIG 指针成员写用 `mov reg,DISP(base)` 直存，
// gcc 4.4 对 `*(T*)(void*+off)=v` 折成 `lea DISP(base),r; mov reg,(r)`（#2/#3 各 +1），
// 跨版本指针写形不可源码复现，语义等价。
#include "GrowthEquipEvent/SelectGrowthWeapon.inc"
}

// ============================================================================
// GrowthEquipEvent::SendGrowthWeaponEventInfo @ 0x080f7226
// 包头 0x1b9：byte 状态（2=礼包已领 / 1=当前角色已选且组一致 / 0=未选）、
// int 已选道具 idx、short +0x10 字段。
// ============================================================================
#include "GrowthEquipEvent/SendGrowthWeaponEventInfo.inc"
}

// ============================================================================
// GrowthEquipEvent::GiftGrowthWeaponBox @ 0x080f7050（const）
// 活动礼包箱领取（每角色一次，data+0x00 状态字节守卫）：按脚本配置的礼包箱
// idx（m_growthWeapon.m_giftBox，ORIG CDataManager+0x4bc4）构造 Inven_Item，
// 经 RDAR 字符串 game_server_msg_277/278 走系统邮件，再发 DB 存档请求并刷新
// 邮件提醒 tooltip。
// ============================================================================
#include "GrowthEquipEvent/GiftGrowthWeaponBox.inc"
}

// ============================================================================
// 生命周期（vtable 全覆盖，保证本 TU 发出的 _ZTV16GrowthEquipEvent 可解析）
// ============================================================================
#include "GrowthEquipEvent/dtor_GrowthEquipEvent.inc"
}

#include "GrowthEquipEvent/StartEvent.inc"
}

#include "GrowthEquipEvent/StartEvent_1.inc"
}

#include "GrowthEquipEvent/EndEvent.inc"
}

#include "GrowthEquipEvent/StartAction.inc"
}

#include "GrowthEquipEvent/IsEventing.inc"
}
