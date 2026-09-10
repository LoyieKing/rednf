// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CItem.md + ORIG weak 访问器反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// NeedMaterialDataManager 走权威头（完整定义 + 实现落 NeedMaterialDataManager.cpp），
// 本 TU 不再重复声明（§4.4 单一声明点；两处调用 mangled 一致，零行为变化）。

#include "CItem.h"
#include "NeedMaterialDataManager.h"

// [A31-GI · P338/P327] 补 <iostream> 恢复 __ioinit；GetNeedMaterial() 前移为本 TU
// 首个强外部符号 ⇒ GI 键名 `_GLOBAL__I__ZNK5CItem15GetNeedMaterialEv`（ORIG 0x85139cb）。
#include <iostream>

// ===================== 报告函数（CItem.md） =====================

// ORIG 0x085139cb
#include "CItem/GetNeedMaterial.inc"
}

#include "CItem/CItem.inc"

#include "CItem/dtor_CItem.inc"
#include "CItem/_isPackageItem.inc"
}

#include "CItem/checkItemUpgradePossible.inc"
}

#include "CItem/check_item_routing.inc"
}

#include "CItem/check_job_type.inc"
}

#include "CItem/isAmplifyPollutionItem.inc"
}

// ===================== 虚函数（ORIG vtable 0x8c96da8 槽 12..23） =====================

#include "CItem/IsImpossibleUpgrade.inc"
}

#include "CItem/IsImpossibleAmplify.inc"
}

#include "CItem/IsImpossibleDisjoint.inc"
}

#include "CItem/IsImpossibleGift.inc"
}

#include "CItem/IsImpossibleCharacCargo.inc"
}

#include "CItem/IsPossibleKiriProtection.inc"
}

#include "CItem/PeriodAfterUnsealing.inc"
}

#include "CItem/IsRandomOption.inc"
}

#include "CItem/GetRandomOptionGrade.inc"
}

#include "CItem/IsRandomOptionUnseal.inc"
}

// ===================== 非虚访问器（ORIG weak） =====================

#include "CItem/GetItemName.inc"
}

#include "CItem/GetAttachType.inc"
}

#include "CItem/is_stackable.inc"
}

#include "CItem/get_index.inc"
}

#include "CItem/get_grade.inc"
}
#include "CItem/GetSellPrice.inc"
}

#include "CItem/get_price.inc"
}

#include "CItem/getItemGroupName.inc"
}

#include "CItem/getUsableLevel.inc"
}

#include "CItem/get_rarity.inc"
}

#include "CItem/getUsablePeriod.inc"
}

#include "CItem/getExpirationDate.inc"
}

#include "CItem/GetQuestItemDropBouns.inc"
}

#include "CItem/GetItemIconName.inc"
}

#include "CItem/IsEnableWorld.inc"
}

// ORIG 0x822c8ea：subset 检查。遍历 other->m_setd4，若任一世界类型本对象
// 未启用则 false；全部启用（含空集）则 true。
#include "CItem/IsEnableWorld_1.inc"
}

// ===================== 缺失弱访问器（ORIG weak） =====================

#include "CItem/isEpicRoutingItem.inc"
}

// ORIG 0x8150f18（weak）：movzbl 0xbd(%eax) — 返回 +0xbd 路由标志
#include "CItem/IsRoutingItem.inc"
}

#include "CItem/check_low_level.inc"
}

#include "CItem/IsHiddenOption.inc"
}

// ORIG 0x828b5b4：（movzbl 0x60(%eax); sete %al）即 this+0x60 字节 == 1
#include "CItem/isPackagable.inc"
}

#include "CItem/isExpertJobUsable.inc"
}

#include "CItem/GetExpertJobCompoundMaterialVariation.inc"
}

#include "CItem/GetExpertJobCompoundRateVariation.inc"
}

#include "CItem/GetExpertJobCompoundResultVariation.inc"
}

#include "CItem/GetExpertJobSelfDisjointBigWinRate.inc"
}

#include "CItem/GetExpertJobSelfDisjointResultVariation.inc"
}

#include "CItem/GetExpertJobAdditionalExp.inc"
}

#include "CItem/GetFinishPointPrice.inc"
}

#include "CItem/get_need_skill.inc"
}

#include "CItem/getFootControlRateLimit.inc"
}

#include "CItem/GetUsablePvPRank.inc"
}

#include "CItem/GetIncreaseStatusType.inc"
}

#include "CItem/GetIncreaseStatusIntData.inc"
}

// ORIG W 0x08545fda（0x0e）：mov 0xf4(%eax),%eax — 返回 +0xf4 所需等级
#include "CItem/GetNeedLevel.inc"
}

// ORIG W 0x08545fcc：返回升级技能类型字段 +0xf0。
#include "CItem/GetUpSkillType.inc"
}

// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
// ORIG W 0x822c86e（0x0d）：mov 0x8(%ebp),%eax; add $0x9c,%eax; ret
#include "CItem/GetDyeInfo.inc"
}

// ORIG W 0x822c97a（0x57）：m_set128.find(category) != m_set128.end()
#include "CItem/hasCategory.inc"
}

// ORIG W 0x85f7be0（0x0f）：mov 0x8(%ebp),%eax; movzbl 0x144(%eax),%eax; ret
// [2026-09-10 E01 落 · Main 批准] 返回类型 uchar→bool（与 CItem.h 声明同批）：
// ORIG 消费端 0x085f77f4 `call; test %al,%al; je` 无 setne 归一（P248 档①）。
// 返回类型不入 mangled；体仍为 movzbl 直返（bool 返回不插 setne，P292 勘误）。
#include "CItem/IsBanRedeemItem.inc"
}
