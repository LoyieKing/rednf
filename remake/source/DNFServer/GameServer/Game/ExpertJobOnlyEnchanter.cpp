// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExpertJobOnlyEnchanter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ExpertJobOnlyEnchanter.cpp —— expert_job::CEnchanter 附魔师 15 符号独占 TU。
#include "InstanceRentalSystem.h"   // [EJEnchanter] 先行引入带 CheckRentableItem(int,uint) 的版本
//（该成员声明为 decl 追加项；ORIG T 0x082702dc）。
// 来源：docs/class_func_reports/expert_job__CEnchanter/ 全部报告逐份转写；
// IsUseOtherUserMonsterCard 无独立报告，按 objdump 反汇编转写并标注【objdump】。
// 布局依据 CExpertJob.h 现有 CEnchanter 字段：
//   +0x08/+0x18 ExpertJobMaterial m_materials[2]（步长 0x10）
//   +0x28 m_user +0x2c m_peer +0x30 uid +0x34 peerUid
//   +0x38 m_field38（附魔费用）+0x3c m_field3c（已登记怪物卡物品，
//   = m_materials[1].m_item）+0x40 m_field40（附魔配方/目标物品）+0x44 m_peerRequestType
// 头部声明追加需求见 /tmp/df_batches/CreatureJob_decl_EJEnchanter.txt。
// ============================================================================
#include "CUser.h"                 // ENUM_PEER_REQUEST_TYPE（先于本头）
#include "CExpertJob.h"
#include "PacketGuard.h"        // PacketGuard : InterfacePacketBuf（put_header/put_byte…）
#include "CItem.h"              // get_index / is_stackable / GetItemType / getUsableItemType
#include "CUser.h"              // Send/SendCmdErrorPacket/get_unique_id/get_acc_name/m_historyLog
#include "CInventory.h"         // GetInvenRef/GetInvenData/MakeItemPacket/gain_money/use_money/delete_item/get_money + GetInvenTypeFromItemSpace
#include "Inven_Item.h"         // Inven_Item（ctor/m_addInfo/m_fieldd）
#include "CDataManager.h"       // G_CDataManager() / find_item(int)
#include "GlobalData.h"         // GlobalData::s_rentalSystem
#include "InstanceRentalSystem.h"
#include "CUserProc.h"          // APSystem::CUserProc::ClearActionAndSendtoUser

namespace expert_job
{

// ---------------------------------------------------------------------------
// IsUseOtherUseMonsterCard（ORIG W 0x849f324 sz 0x27）【objdump】：
// 读 m_peerRequestType(+0x44)，==7 或 ==8 时返真。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/IsUseOtherUserMonsterCard.inc"
}

// ---------------------------------------------------------------------------
// IsRightMaterial（报告 IsRightMaterial.md，ORIG T 0x849c9b2 sz 0x53）：
// 两槽依次 IsRight()，首个非 0 值即返回；全 0 返 0。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/IsRightMaterial.inc"
}

// ---------------------------------------------------------------------------
// IsRecipeRight（报告 IsRecipeRight.md，ORIG T 0x849ca06 sz 0x63）：
// m_field40 非空、非 stackable、虚槽 vtable+0xc（GetItemType）== 3 才为真。
// ---------------------------------------------------------------------------
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobOnlyEnchanter/IsRecipeRight.inc"
}

// ---------------------------------------------------------------------------
// IsEqualMonsterCad（报告 IsEqualMonsterCad.md，ORIG T 0x849ca6a sz 0x5b）：
// FindMonsterCard(m_field40) 结果的 get_index 与 m_field3c 的 get_index 相等。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/IsEqualMonsterCad.inc"
}

// ---------------------------------------------------------------------------
// _DrawEnchant（报告 _DrawEnchant.md，ORIG T 0x849cac6 sz 0x65）：
// IsSuccess 判定，成功则把结果经验并入当前职业经验。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/_DrawEnchant.inc"
}

// ---------------------------------------------------------------------------
// _TryEnchantMine（报告 _TryEnchantMine.md，ORIG T 0x849d7b6 sz 0x287）
// ---------------------------------------------------------------------------
// [b28d-solo-ExpertJobOnlyEnchanter] AE（189/189 逐条全等，p167 14/14 CLEAR）。
// 修法：①补 ORIG 0x0849d83d 的死局部 `movl $0x0,-0xc(%ebp)`（P171 死写入照抄
// + P232 帧律，同时把 4.4 标量槽区补到 ORIG 同深：ret -0x18 / card -0x14 /
// slot -0x10）；②`slot = GetInvenData(...)` 与 `if (slot == -1)` 分两句（P15，
// 赋值即条件形多 sete;test 物化）。
#include "ExpertJobOnlyEnchanter/_TryEnchantMine.inc"
}

// ---------------------------------------------------------------------------
// _TryEnchantOther（报告 _TryEnchantOther.md，ORIG T 0x849da3e sz 0x285）
// ---------------------------------------------------------------------------
// b28c-1 N7: _TryEnchantOther 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobOnlyEnchanter/_TryEnchantOther.inc"
}

// ---------------------------------------------------------------------------
// _PutItemForEnchant（报告 _PutItemForEnchant.md，ORIG T 0x849d344 sz 0x423）
// kind：0=登记费用、1=主槽材料、2=对家槽怪物卡。money 仅 kind==0 使用。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/_PutItemForEnchant.inc"
}

// ---------------------------------------------------------------------------
// OnPutItemForEnchant（报告 OnPutItemForEnchant.md，ORIG T 0x849df5e sz 0x6d）
// msg+0xd byte = kind；msg+0xe 同时被解释为 stItemPos_t 内联结构与首 dword。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/OnPutItemForEnchant.inc"
}

// ---------------------------------------------------------------------------
// OnTryEnchant（报告 OnTryEnchant.md，ORIG T 0x849dfcc sz 0xb1）
// ---------------------------------------------------------------------------
// [b28d-solo-ExpertJobOnlyEnchanter] P254 档③ + P235 匿名 union 双视角：
// ORIG `mov 0xe(%eax),%eax; mov %eax,%ebx` 两段升格由「字段与 find_item(int) 实参
// 异型」触发 ⇒ 取 MSG_STATIC_COMMAND 的 unsigned 视角 m_value0eU（探针
// /tmp/probe_p254d.cpp f2 逐条实证；int 视角 f1 仍单段，OnPutItemForEnchant 不受影响）。
#include "ExpertJobOnlyEnchanter/OnTryEnchant.inc"
}

// ---------------------------------------------------------------------------
// _AgreeEnchant（报告 _AgreeEnchant.md，ORIG T 0x849dd94 sz 0x1c9）
// ---------------------------------------------------------------------------
// b28c-1 N7: 两处虚调用 callee %eax↔%edx 互换。
#include "ExpertJobOnlyEnchanter/_AgreeEnchant.inc"
}

// ---------------------------------------------------------------------------
// OnAgreeEnchant（报告 OnAgreeEnchant.md，ORIG T 0x849e07e sz 0x83）
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/OnAgreeEnchant.inc"
}

// ---------------------------------------------------------------------------
// SetPeerRequestType（ORIG W 0x849f316：写 +0x44）
// 头文件现签名为 (int)；ORIG mangled 为
// _ZN10expert_job10CEnchanter18SetPeerRequestTypeE22ENUM_PEER_REQUEST_TYPE，
// 以枚举重载为准（decl 要求删除旧 (int) 定义于 ExpertJobLink4.cpp:703）。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/SetPeerRequestType.inc"
}

// ---------------------------------------------------------------------------
// _DoEnchant（报告 _DoEnchant.md，ORIG T 0x849cb2c sz 0x817）：
// 前置费用门禁 → 材料/配方校验 → _DrawEnchant 结算 → 目标物品 addInfo 写
// 怪物卡索引 → 双向结果包 → 费用转移/材料消耗 → 结算包与历史日志。
// 返回 0=成功，其余为 ORIG 错误码（0x11/0x15/0x14 等）。
// [b28d-solo-ExpertJobOnlyEnchanter] 判词：553/553 条、gap 0、帧 sub $0x60 与
// 槽位集全等、p167 CLEAR（§6.2 三件套满足），仅剩 1 处 3 条求值序差 ⇒ 语义完成、
// byte 层 N 封顶（4.1/4.4 短路物化位置差，N-88 族）。
// 本轮真语义修正：①门禁漏 `m_field38 > 0` 且 bVar4b=false 时 get_money 检查仍须
// 执行（原 else-if 链整段跳过）；②MakeItemPacket 的 receiver 应为 m_peer（0x2a0
// `mov 0x2c(%eax),%eax`），原误用 m_materials[0].m_user。
// 本轮形态修正：ret 预清+赋值即条件（P198①/P15）；UseMaterialOfRecipe 赋值即条件；
// useOther/slot0/slot0b/ok/cardIdx/ejType/slot1/characName/accName/fee 全部内联
// （P0/P134，具名局部各占槽）；局部声明序按 ORIG 槽位（successSent(-0x21) →
// ref=0(-0x20) → PacketGuard(-0x44) → invenType(-0x1c) → ref(-0x20) →
// itemIndex0(-0x18) → oldCardRef(-0x14) → addedCard(-0x10)）；trace 改全表达式
// 临时链（P227，ctor 前求值 varargs，槽 -0x38）；IsExistRecipeItem 返回类型
// int→bool（P292/P248 档①，CExpertJob.h 声明 + ExpertJobOnlyMaterial.cpp 定义
// 已由 S351 同步）。
// 残余 N：0x056-0x0a1 段 ORIG 先物化 `(m_user != 0 && m_peer->getCurCharacR() != 0)`
// 成 %al 岛再与 CheckMoney 短路；4.4 恒把 CheckMoney 提到物化之前。平铺三条件链、
// `!!`、`!= false`、德摩根四形探针（/tmp/probe_cond.cpp、probe_cond2.cpp）全产
// 同一 4.4 形，P226 语句表达式实测 553→558 劣化 ⇒ 判 N（跨版本短路物化位置差）。
// ---------------------------------------------------------------------------
#include "ExpertJobOnlyEnchanter/_DoEnchant.inc"
}

}  // namespace expert_job
