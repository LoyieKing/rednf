// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserDepsA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CUserDepsA.cpp —— CUser 第一组恢复（独占 TU；不触碰 CUser.cpp）。
//
// 六个方法均逐指令对照 docs/class_func_reports/CUser/<方法>.md 的
// ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   1. get_character_job        ORIG T 0x0864e024 _ZNK5CUser17get_character_jobEj
//   2. CheckQuestMap            ORIG T 0x0866cac6 _ZN5CUser13CheckQuestMapEi
//   3. IsCompleteGuildFund      ORIG T 0x0866c1ca _ZN5CUser19IsCompleteGuildFundEv
//   4. LoadCharacTypeHotKey     ORIG T 0x08692d2a _ZN5CUser20LoadCharacTypeHotKeyEii
//   5. SendNotiPacketMessage    ORIG T 0x086886ce _ZN5CUser21SendNotiPacketMessageEPKc17ENUM_MESSAGE_TYPE
//   6. change_equip             ORIG T 0x0865eed2 _ZN5CUser12change_equipEisi
// ============================================================================

#include <stddef.h>
#include <string.h>

#include "CUser.h"                    // CUser/UserQuest/_Charac_info/STGuildDBInfoOnly/
                                      // ENUM_MESSAGE_TYPE/INVEN_TYPE/CCharacterView +
                                      // online_preliminary::COnlinePreliminary（经 CUserCharacInfo.h）
#include "CInventory.h"               // GetInvenRef(int,int) const / ChangeEquip(INVEN_TYPE,int,int)
#include "Inven_Item.h"               // Inven_Item 完整类型（m_addInfo +0x02）
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "CGameOption.h"              // get_load_hotkey_flag/get_hotkey_option（CHotkeyOption 0xbc）
#include "GameWorld.h"                // G_GameWorld/IsCheckUnusableEquipmentChannel
#include "CDataManager.h"             // G_CDataManager/CDataManager::find_item(int) const
#include "CItem.h"                    // ENUM_WORLD_TYPE/CItem::IsEnableWorld(ENUM_WORLD_TYPE)
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString(int,const char*,bool*)
#include "AvatarRechargeServer.h"     // ARAD::Singleton<AvatarRechargeServer>::Get +
                                      // CalcDurabilityChangeEquip(CUser*,int)
#include "DB_LoadCharacHotKeyOption.h"

// ----------------------------------------------------------------------------
// 1. ORIG T 0x0864e024 sz 0x91。
// 遍历 m_characList（+0x796e8 vector<_Charac_info>），首个 m_characNo（+0x00，
// 按 uint 比较）等于 characNo 的元素返回其 +0x26 字节（job，movsbl 符号扩展）；
// 未命中返回 -1。
// ----------------------------------------------------------------------------
#include "CUserDepsA/get_character_job.inc"
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x0866cac6 sz 0x3d。
// getCurCharacR() 为空返回 0；否则委托当前角色 UserQuest 的
// get_rescue_questmap(dungeonIdx)（ORIG 直调 _ZNK9UserQuest19get_rescue_questmapEi）。
// ----------------------------------------------------------------------------
#include "CUserDepsA/CheckQuestMap.inc"
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0866c1ca sz 0x53。
// m_guildDB.m_fundLevel（+0x8d03b，无符号字节）>0xf 且 m_guildDB.m_fundMoney
// （+0x8d0c0，uint）>=10000000 即完整；否则 fundMoney >=20000000 判完整
// （汇编 jbe $0xf / jbe $0x98967f / jbe $0x1312cff，全部无符号比较）。
// ----------------------------------------------------------------------------
#include "CUserDepsA/IsCompleteGuildFund.inc"
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x08692d2a sz 0x171。
// hotkeyType = GetCharacHotKeyType(characJob)（==10）。装了热键加载开关时：
//   CheckLoadHotKeyCashedCharacType(cashed, job) 通过才发包——
//   PacketGuard: put_header(0,0x1c4)/put_byte(type)/put_int(0xbc)/
//   put_binary(get_hotkey_option(type),0xbc)/finalize(true)，Send 后析构；
// 未开开关：DB_LoadCharacHotKeyOption::makeRequest(GetUID, acc_id, type)。
// ----------------------------------------------------------------------------
#include "CUserDepsA/LoadCharacTypeHotKey.inc"
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x086886ce sz 0xf5。
// 通知消息包：put_header(0,0xc)/put_byte(type)/put_short(0)/put_byte(0)/
// put_int(strlen(msg))/put_str(msg,strlen(msg))/finalize(true)，Send 后析构
// （strlen 与 put_str 均按 ORIG 各调一次）。
// ----------------------------------------------------------------------------
#include "CUserDepsA/SendNotiPacketMessage.inc"
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x0865eed2 sz 0x30e。space 分派：
//   1 头像域（INVEN_TYPE_AVATAR=2）：频道不可用装备校验失败→msg_146 通知返 1；
//     isCompetitionMercenary→返 0xb3；ChangeEquip(AVATAR,slot,dst)==0 时回写
//     setAvatarTypeBonus(CalcMercenaryAvatarBonus)；>0 直接返；否则
//     AvatarRechargeServer::CalcDurabilityChangeEquip(this,dstSlot)。
//   2 交易域：CheckInTrade()→返 0x13（否则落尾部）。
//   0 装备域（INVEN_TYPE_EQUIP=1）：同款不可用装备校验；
//     getCurChannelEquipslotSwitch()==1 且 dstSlot!=0xb 时 find_item(m_addInfo)
//     非空且 !IsEnableWorld(WORLD_1) → 返 1；ChangeEquip(EQUIP,...)>0 直接返。
//   其它 space 返 1。
// 尾部：0<=dstSlot<0xc 时 m_characterView.enableSaveCharacView()；
// send_equip(dstSlot)；返 0。
// ----------------------------------------------------------------------------
// [2026-09-09 AE 归零] check_tu_routed fresh ⇒ AE（orig=203 our=203，
// fn_diff_tu 0 差异行，帧 sub $0x34 与槽 -0x1c/-0x18/-0x14/-0x10/-0xc 全等），
// p167 调用点差集 CLEAR（32/32），jcc 逐位交叉验证一致（30 条 jcc 的
// 助记符+目标指令索引全等）。四处源形修正（均有 ORIG 直证）：
//   ① if/else 链 → switch(space)：ORIG `mov 0xc(%ebp),%eax; cmp $1,%eax;
//      je; cmp $2,%eax; je; test %eax,%eax; jne` 是 switch 决策链
//      （4.4 if 链恒折内存直比 cmpl $imm,mem，P251/P286）；case 叶块
//      物理序=书写序（P324）⇒ case 0（fall-through 体）→ 2 → 1 → default；
//   ② `int ret = 0;`（ORIG 头部 movl $0,-0x18）；
//   ③ equipslot 守卫合并短路 `a==1 && b!=0xb` → 嵌套 if：ORIG 为
//      `cmp $1,%al; sete %al; test %al,%al; je X; cmpl $0xb; je X`
//      （两守卫同落 X + 首条件 sete 物化 = 嵌套 if 形，P133/P225；
//      合并 && 产短路直跳 jne/je 反形）；
//   ④ find_item 实参内联：ORIG GetInvenRef 结果无槽往返
//      （`mov 0x2(%eax),%eax; mov %eax,%ebx; call G_CDataManager;
//      mov %ebx,0x4(%esp); call find_item`，P0/P183/P52），
//      声明中间 `const Inven_Item* item` 会多占一槽并把帧撑到 0x44。
// 旧判词「N82/N95 帧 0x34 vs 0x44 封顶」已作废。
#include "CUserDepsA/change_equip.inc"
}
