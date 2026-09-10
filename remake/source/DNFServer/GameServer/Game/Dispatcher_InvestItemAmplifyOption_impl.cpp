// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatcher_InvestItemAmplifyOption_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Dispatchers.h"

#include "CDataManager.h"      // G_CDataManager(), WongWork::CItemUpgrade, m_itemUpgrade (+0x20)
#include "CInventory.h"        // getCurCharacInvenR/W, INVEN_TYPE, GetInvenSlot/delete_item/update_item
#include "CItemAmplifier.h"    // CItemAmplifier, stInvestAmplifyOption_t, ENUM_ITEM_AMPLIFY_INVEST_TYPE, stAmplifyOption_t
#include "CItem.h"             // CItem, IsEnableWorld(ENUM_WORLD_TYPE) + IsEnableWorld(const CItem*)
#include "GlobalData.h"        // GlobalData::s_itemAmplifier_
#include "GameWorld.h"         // G_GameWorld(), IsPVPChannel(), IsPvPSkilTreeChannel()
#include "CUserCharacInfo.h"   // get_charac_level(), getCurCharacInvenR/W()（CUser is-a CUserCharacInfo）
#include "Inven_Item.h"        // Inven_Item（0x3d, pack1）
#include "CUser.h"             // CUser::get_state()/CheckItemLock/Send/SendCmdErrorPacket, m_historyLog

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 定义由 CompoundEmblemDeps.cpp 提供（见 CompoundEmblemDeps.h）。
extern int LineFunc(int, const char*, int, unsigned int);

// ORIG 0x082194f4（约 883 指令）：强化投资（增幅强化投点）。
// 恢复链路：解析装备槽/材料槽/强化类型 → 校验 → 消耗材料 → CItemAmplifier 强化 +
// CItemUpgrade 随机强化（investType==2）→ 写回装备槽 → 回复包 header 0xd0。
// 所有校验/解析失败走 LineFunc 断言路径，并返回 LineFunc 的返回值（ORIG 0x08219534 起）。
// 业务校验失败按 ORIG 抛 ENUM_ERROR（整型码）；第 5 项之后随机强化失败走
// SendCmdErrorPacket(0xd0, code) 后返回 0。
//
// b28d-20 判词（ORIG 0x082194f4，883 insn）：§6.2 三件套 —— gap=1（仅 N18a 单段
// regmove）、p167 调用点差集 116/116 清零、槽位集全等（-0x1c..-0x1a8）⇒ 语义完成、
// byte 层 N 封顶。本轮修法：①try/catch(ENUM_ERROR)（ORIG try 082198cf..0821a01b、
// catch 0821a01e：SendCmdErrorPacket(0xd0, code)+return 0）；②get_charac_level 表达式
// 内联（P0，ORIG 08219b89 无槽往返）；③G_GameWorld 两次直调（P0/P183 不缓存）；
// ④|| 直作 if 条件复现 mov$1/mov$0 物化（P214，ORIG 08219e26 段）；⑤checkInvestableItem
// 用 !（P168a）；⑥enableRet 用 while+break 复现 cmpl/setne/test 物化（P225，ORIG 08219ea1）；
// ⑦apply_Item 改读 m_addInfo(+0x02)（ORIG 08219fed 读 -0x9b/-0xd8，真语义修正，原 m_addInfo2 为 +0x07）；
// ⑧补 ORIG -0x30 死写入 strBufSize=0x80（P171/P232 撑帧，使槽位集与 ORIG 全等）；
// ⑨getInvestMaterialInfo 返回类型 int→bool（ORIG 08219cd4 xor $1，已广播）。
// 残余 N：delete_item 第三参 ORIG 08219f7b `mov -0x4c,%eax; mov %eax,%esi` 两段中转
// （N18a/P75；4.4 恒单段 mov mem,%esi。字段侧/callee 侧/调用点 (unsigned) cast/
// P226 语句表达式/P231 register 五形实测均无效）。
#include "Dispatcher_InvestItemAmplifyOption_impl/dispatch_sig.inc"
}

// ORIG 弱符号（约 5 指令，vtable 占位）：返回 0 与 ORIG identical，勿改。
#include "Dispatcher_InvestItemAmplifyOption_impl/process.inc"
}
#include "Dispatcher_InvestItemAmplifyOption_impl/read.inc"
}