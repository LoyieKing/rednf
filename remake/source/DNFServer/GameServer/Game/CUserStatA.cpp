// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserStatA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CUserStatA.cpp —— CUser 等级/属性域三符号独立 TU（Prod_CUserA 批次）。
// 仅承载三个真实 ORIG 符号（已 grep 全工程确认无既有定义，2026-08-24 广播）：
//   CUser::_update_charac_stat(bool)   ORIG T 0x086646c8
//                                      _ZN5CUser19_update_charac_statEb
//   CUser::_onLevelUp()                ORIG T 0x0866311a
//                                      _ZN5CUser10_onLevelUpEv
//   CUser::UpdateTutorialSkipable()    ORIG W 0x08697318
//                                      _ZN5CUser22UpdateTutorialSkipableEv
// 依据 docs/class_func_reports/CUser/{_update_charac_stat,_onLevelUp}.md 全文；
// UpdateTutorialSkipable 无报告，按 objdump 逐指令转写（见函数注释）。
// 被调 _processLevelReward/_postQuestTriggerItem/processLevelUpEventReward/
// processLevelUpEvent、DB_Insert{UnlimitSupport,ArchieveEvent}Log::makeRequest、
// EventClassify::CEventScriptMng::process_level_up_reward、CFatigueBatteryHandle::
// ActiveFatigueBattery_EndLevel 为"声明先行"依赖，定义由其它批次还原。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"          // G_CDataManager/m_characList/find_quest/find_item/get_event_script_mng
#include "CCharacList.h"           // CCharacList::m_characs[11]（+0x000，步长 0x7dc）
#include "CCharacter.h"            // CCharacter 布局（0x7dc 步长）/get_growtype_stat/_Additioal_info
#include "Quest.h"                 // Quest::m_mailFlag/m_levelMin、stSelectQuestParam(CUser*)
#include "CInventory.h"            // GetInvenRef/GetInvenData/insertItemIntoInventory/update_item
#include "Inven_Item.h"            // m_addInfo/m_field1/set_add_info/ResetItemAttr
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"     // CEventBase/CEventManager::GetRepeatEvent
#include "CFatigueBatteryHandle.h"
#include "CSystemTime.h"
#include "CEventScriptMng.h"
#include <string.h>
#include <list>

class DB_InsertUnlimitSupportLog
{
public:
    // ORIG 0x0842ebd0：_ZN26DB_InsertUnlimitSupportLog11makeRequestEjjPKc（返回 void，
    // 见 DB_InsertUnlimitSupportLog/makeRequest.md 反编译）。类声明与 DB_U3.cpp/
    // CMailBoxHelper.cpp 的 TU 局部前置声明模式一致。
    static void makeRequest(unsigned int uid, unsigned int characNo,
                            const char* name);
};

class DB_InsertArchieveEventLog
{
public:
    // ORIG 0x0842e856：
    // _ZN25DB_InsertArchieveEventLog11makeRequestEjjN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE
    // （返回 void，见 DB_InsertArchieveEventLog/makeRequest.md 反编译）。
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type);
};

// ============================================================================
// CUser::_update_charac_stat(bool) @ 0x086646c8
// 报告：docs/class_func_reports/CUser/_update_charac_stat.md（汇编+C 全文）。
// 语义：取当前角色 (firstGrow,secondGrow) 对应职业成长属性块（CCharacList 以
// 0x7dc 步长的 CCharacter 数组，基址 = CDataManager::m_characList），将其
// add/subtract 到当前角色 AddInfo（写侧），add=false 时回退。
// ============================================================================
#include "CUserStatA/_update_charac_stat.inc"
}

// ============================================================================
// CUser::_onLevelUp() @ 0x0866311a
// 报告：docs/class_func_reports/CUser/_onLevelUp.md（汇编+C 全文）。
// 升级尾处理：限时日志 → 等级奖励 → 达成事件日志 → 公会服邮件金币 → 邮件任务
// 触发 → 会员限时时邮件道具 → NPC 礼物/升级事件 → 85 级疲劳电池 → 20 级补发
// 4183 号物品 → 事件脚本升级奖励。
// ============================================================================
// [SweepCUserStatA] verdict=NEAR(routed) [N7] ORIG 0x0866311a orig/ours=684/681
//   本波修真缺口：
//   ① ④ 分支 B 原写裸 else，ORIG +0x2e1 有第二次 get_charac_level() 调用 ⇒ 改
//      else if (get_charac_level() == 0x28)（P48 O0 无 CSE，双臂各自独立调用）；
//   ② ④/⑥ 时间上界 ORIG 为无符号 ja（0x197/0x4fd），原写有符号 < ⇒ 补
//      (unsigned int)getCurSec()（P196：同函数 ① 处 jg 为有符号，两处必须分型）；
//   ③ ⑤ 循环体：questId 补具名槽（ORIG +0x438 mov %eax,-0x30 ⇒ P0 有槽往返）；
//      三守卫改 P223 goto 岛（ORIG +0x456/0x464/0x47e 三 nop 岛 + 尾 jmp）；
//   ④ ⑥ GetInvenRef 链内联 + 连续 case switch 包块 ⇒ 复现 ORIG 无槽 `sub/cmp/ja`
//      （P320/P197④ 镜像族；内联 if 形 4.4 恒产 setbe/test/je）；
//   ⑤ ⑧ cMyTrace 改全表达式临时链（P227，ORIG ctor 前右起求 varargs 跨 ctor 保活）；
//   ⑥ ⑨ ItemAdd 第 2 实参直读 m_addInfo2、第 3 实参内联 get_add_info（P0 无槽），
//      并按其后 store 序先写 m_addInfo2 再写 m_addInfo（ORIG +0x912/+0x91c）；
//   ⑦ ⑤ stSelectQuestParam 移出内层块（作用域延至函数尾）⇒ 消除 4.4 与 ④
//      Inven_Item 的栈槽复用（P332 塌槽）；槽偏移 109/109 与 ORIG 全等、帧 0x2ac 全等。
//   残差：3 处虚调用寄存器角色互换（N7：ORIG 目标 %edx/this %eax ↔ 4.4 目标 %eax），
//         长度/助记符/槽号全等；p167 仅 Inven_Item/list/cMyTrace/stSelectQuestParam
//         的 C1/C2·D1/D2 同址别名（P93 结构性噪音）。
#include "CUserStatA/_onLevelUp.inc"
}

// ============================================================================
// CUser::UpdateTutorialSkipable() @ 0x08697318 —— 无报告，objdump 逐指令转写：
//   8697324: call 819a870 <CUserCharacInfo::enableSaveCharacStat()>  ; this+0x1 = 1
//   8697337: call 84ecae4 <CUser::SetTutorialSkipable(unsigned char)>; 实参 1
//   （SetTutorialSkipable 写 CUser+0x8e08a 字节；GetTutorialSkipable 同址读取。）
// ============================================================================
#include "CUserStatA/UpdateTutorialSkipable.inc"
}
