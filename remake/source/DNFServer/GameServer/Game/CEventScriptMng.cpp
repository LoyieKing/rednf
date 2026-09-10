// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventScriptMng/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventClassify::CEventScriptMng 实现。
#include "CEventScriptMng.h"

#include "CUser.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "PacketGuard.h"
#include "DB_SelectLevelUpReward.h"   // process_level_up_reward 的 DB 确认请求
#include "GameWorld.h"
#include "CDataManager.h"
#include "DB_UpdateInGameEventHistory.h"
#include "InGameEventScripts.h"
#include <ctime>
#include <cstring>

#include "SyncScriptDeps.h"          // toTString

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN13EventClassify12CEventEntityC2Ev`（P327）。全限定定义可写在
// 命名空间块之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ORIG 0x810af40：设 vptr，+0x04 字节=0，构造 m_dbTableName(std::string)，
// +0x0c/0x10/0x14/0x18 均清 0。
#include "CEventScriptMng/CEventEntity.inc"
}

// InGameEventInfo / InGameEventInfoList 为全局作用域类型（见 CEventScriptMng.h），
// 故其方法定义位于任何命名空间之外。

// ============ InGameEventInfo ============

// ORIG 0x08986044：ctor 仅构造 m_name（+0x08），m_eventId/m_field4 及后续
// 字段保持未初始化，由后续 insertEventInfo 赋值（ORIG 汇编无任何 int 清零）。
#include "CEventScriptMng/InGameEventInfo.inc"
}

#include "CEventScriptMng/dtor_InGameEventInfo.inc"
}

#include "CEventScriptMng/InGameEventInfo_1.inc"
}

// ORIG _ZN15InGameEventInfoaSERKS_：无自赋值检查，逐字段复制。
InGameEventInfo& InGameEventInfo::operator=(const InGameEventInfo& o)
{
    m_eventId = o.m_eventId;
    m_field4 = o.m_field4;
    m_name = o.m_name;
    m_startTime = o.m_startTime;
    m_endTime = o.m_endTime;
    m_isEventing = o.m_isEventing;
    return *this;
}

// ============ InGameEventInfoList ============

// ORIG _ZN19InGameEventInfoListD1Ev：仅 map 析构（隐式），无 clear（P12）。
#include "CEventScriptMng/InGameEventInfoList.inc"
#include "CEventScriptMng/insertEventInfo.inc"
}

// ============ InGameEventScriptLoader ============


// ORIG _ZN23InGameEventScriptLoaderD1Ev：仅 map 析构（隐式），无 clear（P12）。
#include "CEventScriptMng/InGameEventScriptLoader.inc"
#include "CEventScriptMng/insertEventScript.inc"
}

// [b28d-solo-CEventScriptMng] getEventScript：NEAR — N93 封顶。
// ORIG 0x08982f38：find 目标槽 -0x10 / end 目标槽 -0xc（4.1 先声明占深槽且调用序
// find→end）；4.4.7 按首次使用序分配，恒反向（find→-0xc / end→-0x10）。
// 实测三形：①本形（it 先声明）find 槽 -0xc；②交换声明序（endIt 先）调用序变
// end→find，指令序整体错位；③先声明后赋值引入 ORIG 不存在的
// _Rb_tree_iterator::C1Ev（nm 实证）⇒ 槽方向与调用序不可同时满足，封顶。
// p167 CLEAR（6/6），语义闭合。
#include "CEventScriptMng/getEventScript.inc"
}

namespace EventClassify
{

// ============ CEventEntity ============

#include "CEventScriptMng/dtor_CEventEntity.inc"
}

#include "CEventScriptMng/is_eventing.inc"
}

#include "CEventScriptMng/set_event_flag.inc"
}

#include "CEventScriptMng/set_event_data.inc"
}

#include "CEventScriptMng/get_event_record_type.inc"
}

#include "CEventScriptMng/set_event_record_type.inc"
}

// ORIG 0x810b0ac（32 字节）：基类实现为纯 return 0 —— 参数仅存栈不使用。
#include "CEventScriptMng/send_init_data.inc"
}

#include "CEventScriptMng/get_db_table_name.inc"
}

#include "CEventScriptMng/get_event_start_time.inc"
}

#include "CEventScriptMng/set_event_start_time.inc"
}

#include "CEventScriptMng/get_event_end_time.inc"
}

#include "CEventScriptMng/set_event_end_time.inc"
}

// ============ CEventScriptMng ============

#include "CEventScriptMng/CEventScriptMng.inc"
}

// [b28d-solo-CEventScriptMng] ~CEventScriptMng：AE。
// ORIG 0x0810b218：两个 map 循环各自 begin/end 缓存（P13），修复前每轮重调 end()
// 多占 sret 临时槽并顶偏全帧 ⇒ 缓存后槽位逐条对齐、compare AE。
// p167 余 2×D1Ev↔2×D2Ev 为同址别名（ORIG 0x08110f24/0x08110f60 与我们的 .o 均
// 同址 W 双名，P93 伪差集），compare 判 AE，语义闭合。
#include "CEventScriptMng/dtor_CEventScriptMng.inc"
}

// ORIG 0x810ba36：key ∈ [0,0x19c] 时取 map[key] 并调用其虚 is_eventing。
#include "CEventScriptMng/is_eventing_1.inc"
}

// ORIG T 0x0810bea8（报告 EventClassify__CEventScriptMng/
// process_add_item_count_db.md）：user 有效且事件 0x193 实体存在、
// CAddItemCountDB::is_event_item(itemId) 命中时，给当前角色累加
// 事件物品计数（count 按 unsigned short 零扩展）。
#include "CEventScriptMng/process_add_item_count_db.inc"
}

// ORIG 0x810b110：按事件类型取实体，并转发到实体的表名访问器。
#include "CEventScriptMng/get_db_table_name_1.inc"
}

// [b28d-solo-CEventScriptMng] process_account_first_login：NEAR — N7 封顶。
// ORIG 0x0810bde0：两条独立守卫（P133）+ 匿名 bitset reference 临时（槽 -0x14）；
// 修复前合并 `||` 触发 bool 汇聚（mov$1/jmp/mov$0/test）⇒ 拆两条 if 后守卫岛
// （nop;jmp + nop）与全帧槽位逐条对齐。
// 残差唯一：尾部虚调用目标寄存器 ORIG %edx vs 4.4.7 恒 %eax（N7；本 TU 未走
// 4.4.6 路由，P230 分诊=后端敏感族）。p167 CLEAR（6/6），语义闭合。
#include "CEventScriptMng/process_account_first_login.inc"
}

// ORIG 0x810c110：活动 0x56 开启时更新用户服务器疲劳值。
#include "CEventScriptMng/process_add_fatigue_count.inc"
}

// ORIG 0x810bf00：活动 0x193 的任务计数命中时累加一次物品计数。
// [B24D判词 N] 语义正确：user!=0 → get_event_entity(0x193)→is_event_quest→
//   IncCurCharacEventAddItemCount(1)。diff=ORIG 句尾 `jmp;nop`（4.1 合并跳转+对齐）
//   我方直接 leave;ret，27/25 纯 lowering/对齐表示差异，语义等价。
#include "CEventScriptMng/process_add_quest_count_db.inc"
}

// ORIG 0x810c0a6：升级奖励按指定等级/成长类型发放。
#include "CEventScriptMng/process_level_up_reward_recv_level.inc"
}

// ORIG 0x810c152：按疲劳事件用户类型累加已使用疲劳量。
#include "CEventScriptMng/inc_user_fatigue.inc"
}

// ORIG 0x810c1ea：礼物疲劳事件验证通过后发送奖励。
#include "CEventScriptMng/send_gift_fatigue_item.inc"
}

// ORIG 0x810c270：商城奖励活动开启时更新目标商品计数。
// [B24D判词 N] 语义正确：is_eventing(0x19a)→get_event_entity(0x19a)→
//   updateTargetGoodsCount(user,goodsId,request)。diff=ORIG 句尾 `jmp;nop;jmp`（4.1
//   合并跳转+对齐）我方单 jmp，32/30 纯 lowering/对齐表示差异，语义等价。
#include "CEventScriptMng/update_purchase_target_item_count.inc"
}

// ORIG 0x810c2d4：透传商城消费奖励邮件结果。
#include "CEventScriptMng/send_purchase_bonus_mail.inc"
}

// ORIG 0x810c33a：养成事件开启时发送疲劳奖励。
#include "CEventScriptMng/send_bring_up_item_fatigue.inc"
}

// ORIG 0x810c3a4：网吧事件初始化通知。
#include "CEventScriptMng/send_pc_room_play_init_time.inc"
}

// [b28d-solo-CEventScriptMng] send_event_init_data：DIFF — N82附 封顶。
// ORIG 0x0810c40e：包 0x176；end() 循环头缓存（P13）后全帧槽位逐条对齐
// （PacketGuard -0x28 / countIdx -0x2c / it -0x30 / endIt -0x34 / vfn -0x40 /
// count -0x19 与 ORIG 全等）。
// 残差唯一：多实参虚调用三活值（vfn/flag/m_todayTimeDay）的 callee-saved 角色
// 对调——ORIG vfn→槽 -0x40、flag→%edi、day→%esi；4.4.7 vfn→%esi、flag→槽
// -0x40、day→%edi（多 1 条 `mov -0x40(%ebp),%edx` 回读，135 vs 136 条）。
// 实测两形：①原实参形（本形）；②P226 语句表达式 `({register bool f=flag;f;})`
// 反劣（多 movzbl 中转，且循环旋转）。flag 的裸 register 三条件中「4.4 不自行
// 升格」不成立 ⇒ 无源码解，封 N82附（多实参 call 首值 callee-saved 对调）。
// p167 CLEAR（19/19），语义闭合。
#include "CEventScriptMng/send_event_init_data.inc"
}

// ORIG 0x810b0cc：0x191→0 / 0x192→1 / 0x19c→2，其它 -1。
#include "CEventScriptMng/get_convert_to_user_confirmflag.inc"
}

// ORIG 0x810b96a：map find，命中返回实体指针，未命中返回 0。
#include "CEventScriptMng/get_event_entity.inc"
}

// ORIG 0x810b9d4（const）：map find != end。
#include "CEventScriptMng/is_event_entity.inc"
}

// ORIG 0x810c7b8：实体存在则调用虚 get_event_record_type（vtable+0x14）。
#include "CEventScriptMng/get_record_type.inc"
}

#include "CEventScriptMng/get_today_time.inc"
}

#include "CEventScriptMng/get_today_time_day.inc"
}

#include "CEventScriptMng/is_current_ingame_event_record_type.inc"
}

#include "CEventScriptMng/get_ingame_event_record_type_list.inc"
}

#include "CEventScriptMng/get_pass_day.inc"
}

// [b28d-solo-CEventScriptMng] update_time_values：AE。
// ORIG 0x0810c5d2：两处修复——① 0x0810c628 `movl $0x0,-0xc(%ebp)` 死局部
// （P232 死槽占帧，令 now/prev/localT/prevT/指针组整体下移 4，全帧槽位与 ORIG
// 全等）；② 0x0810c62f 的 `||` 条件两条守卫（js/jge）同落**出线**的 localTp 块、
// prevTp 块 fall-through（4.1 布局）⇒ 用德摩根正向包裹（P222/P272）条件取反、
// 两块互换复现。p167 CLEAR（4/4），语义闭合。
#include "CEventScriptMng/update_time_values.inc"
}

#include "CEventScriptMng/init_event_nums.inc"
}

#include "CEventScriptMng/init_evnet_entity.inc"
}

#include "CEventScriptMng/initScriptlist.inc"
}


// [b28d-solo-CEventScriptMng] process_level_up_reward：AE。
// ORIG 0x0810bf56：三条独立守卫（user==0 / !is_eventing / reward!=0 正向包裹，
// P133 + P214①；尾部两岛 nop;jmp + nop 与 ORIG 全等）；check_recv_level 结果
// 无具名局部（P0：call 结果寄存器直测）；flag 形参入口 spill 到 -0x2c（bool
// 形参一次性截断，P244）。
// 三个 callee 返回类型链：ORIG 0x0810bfb8/0x0810c005/0x0810c000/0x0810c07c 均
// `call; test %al,%al; je`；4.4.7 探针对 char/int 返回恒产 4 条（调用点
// cast/!=0/!! 四形全败），仅 bool 返回产 2 条 ⇒ 由 Main 代落
// CEventScriptMng.h is_recv_check_item/check_recv_level/set_reward_item → bool
// （定义端 EventLevelupA2/CEventScriptMng_entities 同步；P248 档①/P315）后
// 6 条残差归零。p167 CLEAR（16/16），语义闭合。
#include "CEventScriptMng/process_level_up_reward.inc"
}

}  // namespace EventClassify
