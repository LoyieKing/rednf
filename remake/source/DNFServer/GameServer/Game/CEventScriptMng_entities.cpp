// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventScriptMng_entities/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventClassify 事件实体第一组实现。
// 建模并实现以下 CEventEntity 派生类（ORIG 均为 0x20 字节：
//   基类 CEventEntity 0x1c + InGameEventScript* m_script1c(+0x1c)）。
//   - CLevelUpReward      ← 事件记录类型 0x194
//   - CUsedFatigueGiveItem← 事件记录类型 0x65
//   - CAttendance         ← 事件记录类型 0x56
//   - CPcRoomPlayTime     ← 事件记录类型 0x5c
//   - CAccountFirstLogin  ← 事件记录类型 0x191/0x192
//   - CAddItemCountDB     ← 事件记录类型 0x193
//
// 脚本绑定：m_script1c 指向的具体脚本类型由记录类型唯一确定（ORIG nm W 符号：
// 六个派生类 ctor 均为隐式生成，仅调基类 C1 + 设 vptr；对象由 create_event_entity
// value-init：operator new(0x20) → 整块清零 → C1 thunk）。除 CAccountFirstLogin
// （entitiesB 的派生类需显式调用基类 ctor，故保留声明并补 m_script1c=0 对齐工厂
// 清零终态）外均不声明 ctor，由编译器同样产生清零循环 + 弱 C1。
//
// 脚本访问一律经 InGameEventScripts.h 权威布局的具名字段（禁止裸偏移）：
//   0x194→LevelUpRewardScript        0x65→UsedFatigueGiveItemScript
//   0x56→AttendanceScript            0x5c→PcBangPlayTimeScript
//   0x191/0x192→AccountFirstLoginScript    0x193→AddItemCountDBScript

#include "CEventScriptMng.h"

#include "PacketGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CEnvironment.h"
#include "LogManager.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager（GetRepeatEvent）

#include <cstring>
#include <vector>

#include "InGameEventScripts.h"         // 具体事件脚本类型（权威布局）
// 依赖类由事件系统提供（FixEventScriptDeps）：
#include "EventClassify_Action.h"       // EventClassify::Action_SendMail / CEventActionMng
#include "DB_InsertAccountFirstLoginFlag.h"
#include "GameTypes.h"                  // cMyTrace

// ORIG T 0x0889232e（定义 BingoEventLink4.cpp）：
#include "CEventScriptMng_entities/isSetBit_Uint.inc"

namespace EventClassify
{

// process_action_send_mail 形参用的是 EventClassify_Action.h 的命名空间内
// Action_SendMail；脚本子对象 m_mail 为全局 ::Action_SendMail（ORIG 符号口径，
// 见 InGameEventScripts.h）。两者逐字段同布局（sizeof 均 0x44）。用宏桥接：
// -O0 下 inline 函数会产生多余调用帧（ORIG 无）。
typedef char dnf_mail_layout_assert_[
    (sizeof(::Action_SendMail) == sizeof(Action_SendMail)) ? 1 : -1];
#define EVENT_MAIL_REF(mail) (*reinterpret_cast<Action_SendMail*>(&(mail)))

// ============ CLevelUpReward（0x194 → LevelUpRewardScript）============
// ORIG T 0x810e4ea：以绑定脚本 m_mail（+0x28）为 Action_SendMail&，
// flag=true 走 process_action_send_mail 发奖。
#include "CEventScriptMng_entities/do_action.inc"
}
 
// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：P211 档③——m_script1c
//   具型化（LevelUpRewardScript*）+ 赋值点 static_cast 异型化 ⇒ store 复现 ORIG
//   RHS-first；4.4.6 后端同时消 N7（vfn %ecx / this %eax 与 ORIG 同形）。4.4.7 下
//   该件仍 NEAR（纯 N7 后端差；Main 已路由 4.4.6，dnf_game_cxx.sh b28d-S157 条目）
//   （ORIG 0x0810e168 sz 0x0000002a）
#include "CEventScriptMng_entities/set_event_data.inc"
}

#include "CEventScriptMng_entities/get_db_table_name.inc"
}

#include "CEventScriptMng_entities/is_recv_check_item.inc"
}

#include "CEventScriptMng_entities/get_mail_data.inc"
}

// ORIG 0x810e51a：不缓存脚本/items/item/mail 局部——条件与循环每次内联
// 解引用 it 与 m_script1c；循环计数为 signed int（setg 比较）。
// [b28d-solo-CEventScriptMng_entities] AE（169/169，compare_tu 口径；报告工具
//   对 .o 未解析重定位不归一化，故其 verdict 显示 DIFF，属 P264 伪差）（ORIG 0x0810e51a sz 0x00000241）
#include "CEventScriptMng_entities/send_reward_from_recv_level.inc"
}
// ORIG T 0x811056e（vtable slot 2）：基类 is_eventing 且
// GetRepeatEvent(0x65) 非空且其 IsEventing(0) 为真才返回 true。
#include "CEventScriptMng_entities/is_eventing.inc"
}

// ORIG T 0x08110756：脚本 job 过滤（-1 通配），随后按 [user type] 分派：
//   0=全适用；1=仅 premium（ENUM_PREMIUM_TYPE 0xc）；其它=不适用。
#include "CEventScriptMng_entities/verifyApplyUser.inc"
}

#include "CEventScriptMng_entities/get_db_table_name_1.inc"
}

// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：同 CLevelUpReward
//   ::set_event_data——P211 档③具型化（UsedFatigueGiveItemScript*）+ 4.4.6 消 N7；
//   4.4.7 下仍 NEAR（纯 N7 后端差，Main 已路由 4.4.6）（ORIG 0x081105be sz 0x0000002a）
#include "CEventScriptMng_entities/set_event_data_1.inc"
}

// [b28d-solo-CEventScriptMng_entities] AE：字段 bool 化 + 直存（ORIG 0x081107ea sz 0x0000002a）
#include "CEventScriptMng_entities/getIsCharacEvent.inc"
}

// [B24D判词 N] 语义正确：m_usedFatigue==0/GetCurCharacUsedGiftFatigueQuantity==0 早返；
//   step=used-fatigue；入口 m_fatigueLimit>=0 && limit<=step 早返；do-while 循环判
//   used>step + limit 界，++step 后 step%used!=0 跳过，否则 process_action_send_mail，
//   成功后 logFormat(m_id/charac_no/reward)。全程不缓存脚本指针（与 ORIG 同）。
//   diff=ORIG 多处句尾 `jmp;nop;jmp;nop` 合并跳转链 vs 我方 `jmp;nop;jmp`，105/108
//   循环/跳转 lowering 表示差异，语义等价。
// [b28d-solo-CEventScriptMng_entities] AE（compare_tu 口径；报告工具 105/106 的差
//   为 .o 未解析重定位（P264）+ 1 条尾部对齐 nop（§6.1 剥离））（ORIG 0x081105f6 sz 0x00000160）
#include "CEventScriptMng_entities/sendEventItemFromFatigue.inc"
}

// ============ CAttendance（0x56 → AttendanceScript）============

// [b28d-solo-CEventScriptMng_entities] N 封顶（P75/N7 复合；残 2 行，35/35）：
//   P226 已翻内层物化岛（见下），4.4.6 后端已对齐 vfn→%edx。唯一残差 =
//   ORIG `mov -0xc(%ebp),%eax; movl $0x0,0x4(%esp)` vs 4.4 `movl $0x0,0x4(%esp);
//   mov -0xc(%ebp),%eax`——receiver 装载先于常量实参压栈（4.1 this-first /
//   4.4 arg-first，P75 实参装载时刻跨版本差）。18 形探针全败（/tmp/probe_isevt{,2}.cpp：
//   ->、(*e)、int()、0u、*&*e、两种 cast、三目、嵌套 if、register 局部、具名 int
//   实参、语句表达式实参等），源形不可择。P0 禁 register asm 绑定
//   （ORIG 0x0810e75c sz 0x0000006f）
#include "CEventScriptMng_entities/is_eventing_1.inc"
}

// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：P211 档③——m_script1c
//   具型化（AttendanceScript*，S12 先例）+ 赋值点 static_cast ⇒ store RHS-first；
//   4.4.6 后端消 N7（vfn %ecx / this %eax 与 ORIG 同形）。4.4.7 下仍 NEAR（纯 N7
//   后端差，Main 已路由 4.4.6）（ORIG 0x0810eadc sz 0x0000002a）
#include "CEventScriptMng_entities/set_event_data_2.inc"
}

#include "CEventScriptMng_entities/send_init_data.inc"
}

#include "CEventScriptMng_entities/get_db_table_name_2.inc"
}

#include "CEventScriptMng_entities/get_mail_data_1.inc"
}

#include "CEventScriptMng_entities/get_fatigue_condition.inc"
}

#include "CEventScriptMng_entities/get_final_day_condition.inc"
}

// ORIG 0x810e878：day 越界（<0 或 ≥ m_attends.size()）直接返回；命中则把
// m_attends[day] 写入 mail 的 itemIdx/addInfo，正文 key 取 m_str70 后发奖邮件。
// [b28d-solo-CEventScriptMng_entities] AE：CAttendance::m_script1c 具型化
//   (AttendanceScript*) ⇒ 复现 ORIG 单段 `mov 0x1c(%eax),%ebx` 直落 callee-saved
//   （旧 InGameEventScript*+static_cast 恒两段 mov mem,%eax; mov %eax,%ebx ⇒ 95/97）（ORIG 0x0810e878 sz 0x00000147）
#include "CEventScriptMng_entities/send_attend_reward_item.inc"
}

// ORIG 0x810e9c0：统计 attendFlags 置位数（bit 0..31），等于 final 记录数才发：
// mail 写入 m_pair1c（全勤终奖），正文 key 取 m_str74。
#include "CEventScriptMng_entities/send_attend_final_rward_item.inc"
}

// ============ CPcRoomPlayTime（0x5c → PcBangPlayTimeScript）============

#include "CEventScriptMng_entities/is_eventing_2.inc"
}

// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：同 CLevelUpReward
//   ::set_event_data——P211 档③具型化（PcBangPlayTimeScript*）+ 4.4.6 消 N7；
//   4.4.7 下仍 NEAR（纯 N7 后端差，Main 已路由 4.4.6）（ORIG 0x08110872 sz 0x0000002a）
#include "CEventScriptMng_entities/set_event_data_3.inc"
}

#include "CEventScriptMng_entities/get_db_table_name_3.inc"
}

// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，40/40）：N22 由 P248 档①
//   翻正（CUser::getPcRoomPlayTimeRewardCnt int→unsigned int，Main 代落）；
//   原 N7 残余（is_eventing 虚调用 vfn 目标 %edx vs 4.4.7 恒 %eax）随 4.4.6 路由
//   消尽 ⇒ 逐条对齐（ORIG 0x08110a38 sz 0x00000076）
#include "CEventScriptMng_entities/verifyUserPcRoomPlayEvent.inc"
}

// ============ CAccountFirstLogin（0x191/0x192 → AccountFirstLoginScript）============

// [b28d 单文件泳道] ctor 定义删除：ORIG 的 C1 为隐式 W（0x08110f88，仅
// call CEventEntity::C1 + 写 vptr），用户提供 ctor 会令 4.4 对 `new T()` 不做
// value-init 前置清零（CWG434/N77）——删后由编译器隐式合成，工厂 12 个 case
// 的 value-init 清零循环与 ORIG 逐条对齐。

// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：同 CLevelUpReward
//   ::set_event_data——P211 档③具型化（AccountFirstLoginScript*）+ 4.4.6 消 N7；
//   4.4.7 下仍 NEAR（纯 N7 后端差，Main 已路由 4.4.6）（ORIG 0x0810deda sz 0x0000002a）
#include "CEventScriptMng_entities/set_event_data_4.inc"
}

#include "CEventScriptMng_entities/get_db_table_name_4.inc"
}

#include "CEventScriptMng_entities/get_mail_data_2.inc"
}

#include "CEventScriptMng_entities/do_action_1.inc"
}

// ============ CAddItemCountDB（0x193 → AddItemCountDBScript）============

// [b28d-solo-CEventScriptMng_entities] AE（4.4.6 路由，17/17）：同 CLevelUpReward
//   ::set_event_data——P211 档③具型化（AddItemCountDBScript*）+ 4.4.6 消 N7；
//   4.4.7 下仍 NEAR（纯 N7 后端差，Main 已路由 4.4.6）（ORIG 0x0810e064 sz 0x0000002a）
#include "CEventScriptMng_entities/set_event_data_5.inc"
}

#include "CEventScriptMng_entities/get_db_table_name_5.inc"
}

#include "CEventScriptMng_entities/is_event_item.inc"
}

#include "CEventScriptMng_entities/is_event_quest.inc"
}

// ============================================================================
// create_event_entity @ 0x0810b402（CEventScriptMng 实体工厂）。
// flag=true → 仅基类实体（operator new 0x1c + CEventEntity ctor，无清零）。
// 否则按记录类型 value-init 对应派生类：operator new(0x20) → 整块清零循环 →
// C1 thunk（全部 12 个类的 ctor 均为编译器隐式合成 W，与 ORIG C1 同形）。
//   0x56→CAttendance        0x5c→CPcRoomPlayTime       0x65→CUsedFatigueGiveItem
//   0x191/0x192→CAccountFirstLogin    0x193→CAddItemCountDB   0x194→CLevelUpReward
//   0x196→CExchangeRandomItemReward   0x197→CAvatarDisjointRandomReward
//   0x199→CFatigueQuantity  0x19a→CPurchaseCashItemBonus
//   0x19b→CObjectBringUp    0x19c→CAccountFirstLoginDaily   其它→0
// ============================================================================
// [b28d-solo-CEventScriptMng_entities] AE（418/418）：7 个实体类 ctor 隐式化
//   （删用户 ctor 声明/定义）⇒ 4.4 对 new T() 前置 value-init 清零，12 个 case 的
//   清零循环与 ORIG 全等（旧显式 ctor 仅 5 个清零 ⇒ 341，gap 77 = 7×11 条；
//   ORIG 全 12 个 case 均含 and $0xfffffffc 清零循环）（ORIG 0x0810b402 sz 0x00000568）
#include "CEventScriptMng_entities/create_event_entity.inc"
}
}  // namespace EventClassify
