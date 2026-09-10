// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 UserPartyLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// UserPartyLink4.cpp —— LinkMisc4 批次 CUser/CParty/CDataManager 域 5 符号。
// 全部逐报告忠实转写（docs/class_func_reports/{CUser,CDataManager,CParty}/），
// ORIG mangled 由本 TU 的真实 C++ 定义产生：
//   _ZN5CUser14RewardTutorialEj                T 0x086475d8
//   _ZN5CUser18adjust_charac_statEv            T 0x08664766
//   _ZN5CUser18deleteGuildExpBookEv            T 0x0867e898
//   _ZN12CDataManager19getDungeonIndexListEv   T 0x08365434（非 const）
//   _ZN6CParty12drop_item_gmEP5CUserj          T 0x085a73a6
// ============================================================================
// b28d-45 判词汇总（本 TU 3 件 + 1 NEAR）：
//   deleteGuildExpBook 0x0867e898 NEAR2：N98 pair<ENUM,int>→pair<int,int> 转换
//     临时槽序（ORIG first=-0x10/second=-0xc）；右值引用具名化反劣 117→121 已回退。
//   adjust_charac_stat 0x08664766 DIFF127 N82/N89：_Additioal_info 栈对象 +
//     234:199 大 gap（std 模板/槽复用）。
//   RewardTutorial 0x086475d8 DIFF139 N82/N93：帧 0x60/0x50 + vector<int> 槽向。
//   drop_item_gm 0x085a73a6 DIFF245 N82/N89：257:255 CheckInTrade 早退岛 + 槽序。

#include <string.h>

#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h   // CBattle_Field::drop_item(map_item) + WarField.h map_item
#include "CDataManager.h"    // G_CDataManager + m_serverParameter(+0x68) + find_item
#include "CInventory.h"      // insert_event_items / NumberToString
#include "Inven_Item.h"      // Inven_Item 布局与访问器
#include "LogManager.h"
#include "PacketGuard.h"
#include "ServerParameterScript.h"  // GetEscaladeTutorialReward

// ORIG 文件级枚举（同 CGameManager.cpp 模式；值 0 = 公会经验书 hardcode item）。
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

// ---------------------------------------------------------------------------
// CUser::RewardTutorial(unsigned int)
// ORIG T 0x086475d8 sz 0x245，报告 docs/class_func_reports/CUser/RewardTutorial.md。
// 流程：取 ServerParameterScript::GetEscaladeTutorialReward(flag) 奖励清单；
// 非空时经 CInventory::insert_event_items 入包（moneyReason=3、itemReason=8、
// bLog=true、两个空串日志标签），随后 put_header(1,0x92) 包回执：byte(1)、
// byte(入包数)；>0 时对返回的槽位三元组逐个 put_byte/put_int/put_int，
// finalize(true) 后 Send。
// ---------------------------------------------------------------------------
#include "UserPartyLink4/RewardTutorial.inc"
}

// ---------------------------------------------------------------------------
// CUser::adjust_charac_stat()
// ORIG T 0x08664766 sz 0x381，报告 docs/class_func_reports/CUser/adjust_charac_stat.md。
// 流程：_Additioal_info 局部对象 ctor+clear → calc_lev_stat(calc) → 打印 before
// 快照（当前角色 addInfo 各字段）→ memcpy(calc → getCurCharacAddInfoRefW, 0x52)
// → 打印 after 快照。__PRETTY_FUNCTION__ 处按报告字面量转写。
// ---------------------------------------------------------------------------
#include "UserPartyLink4/adjust_charac_stat.inc"
}

// ---------------------------------------------------------------------------
// CUser::deleteGuildExpBook()
// ORIG T 0x0867e898 sz 0x1c4，报告 docs/class_func_reports/CUser/deleteGuildExpBook.md。
// 流程：deleteSpecificItem({make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0)}, deleted)
// → send_itemspace(0) 失败则 logFormat(0x7c57) 返回 → send_itemspace(2) 失败则
// logFormat(0x7c5c) 返回 → send_RedeemInfo()。ORIG 尾部不设置返回寄存器（void）。
// ---------------------------------------------------------------------------
#include "UserPartyLink4/deleteGuildExpBook.inc"
}

// ---------------------------------------------------------------------------
// CParty::drop_item_gm(CUser*, unsigned int)
// ORIG T 0x085a73a6 sz 0x3df，报告 docs/class_func_reports/CParty/drop_item_gm.md。
// GM 掉落指定物品到队伍战场。ORIG eax 返回错误码：0 成功 / -1 物品不存在 /
// 0x13 目标交易中 / 0x16 战场掉落失败 / 0x17 绑定装备禁止 GM 丢弃；
// 本 TU 按 CParty.h:630 既有 void 声明定义（调用方 GmCmds_D 忽略返回值）。
// ---------------------------------------------------------------------------
#include "UserPartyLink4/drop_item_gm.inc"
}
