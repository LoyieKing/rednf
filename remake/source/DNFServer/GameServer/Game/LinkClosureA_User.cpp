// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinkClosureA_User/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkClosureA_User.cpp — undef_bizA 收口批 A：CUser 用户域符号
//
// 完整阅读报告（docs/class_func_reports/CUser/）：
//   GetCurCharacInventoryRef_08680f2e.md（含 Ghidra C 全文）
//   quest_basic_reward.md / compute_quest_basic_reward_exp.md /
//   compute_quest_basic_reward_gold.md / compute_level_penalty.md
// objdump 转写（报告缺失，nm -S 定址）：
//   UpdateRating(float)            W 0x08568e9c
//   GetUserGainItem 域外自由核对：CLuckPoint 三方法落 CClearReward.cpp
// const 重载版本（_ZNK5CUser24GetCurCharacInventoryRefEtt T 0x08680c8a，
//   OnlyOrig_CreatureJob 批声明）亦按 objdump 同构转写。
// ============================================================================
// [G5 2026-09-10 · L12 round2 组5 复核] 5 件：p167（getUserQuestRewardItem CLEAN、
//   gold MISSING=EXTRA=1 别名）⇒ 无语义缺口。逐件：
//   · CalculateExpertJobMaterials（NEAR）：交换 ci/mi 声明序试验四数不变
//     （31/40/2/5）⇒ 后置 ++ 的 sret 临时 -0x10/-0x14 相对序属 4.4 分配产物（N86），已回滚。
//   · compute_quest_basic_reward_exp：帧 0x44↔0x54 + 槽整体浅 4（N95/N115/P232）。
//   · compute_quest_basic_reward_gold：`xor $1; test; jne` ↔ ORIG `test; je` 直判
//     （P292/P133 族，否定早返物化方向）+ itemIdx 槽 -0xc↔-0x18。
//   · getUserQuestRewardItem / GetCurCharacInventoryRef：仅尾 nop 岛（P223 已试）与
//     .rodata addend 显示差 ⇒ AE 等价。

#include "CUser.h"
#include "Quest.h"
#include "CDataManager.h"      // G_CDataManager / m_questParameter(+0x8cc8)
#include "CInventory.h"
#include "CExpandEquipslot.h"
#include "CTitleBook.h"
#include "CUserCharacInfo.h"

// ---------------------------------------------------------------------------
// CUser::GetCurCharacInventoryRef（ORIG T 0x08680f2e 报告全文已读）
// switch 跳表区域编码：case0→GetInvenRef(1)、case1→GetInvenRef(2)、
// case2→GetCargoRef、case3→GetInvenRef(0)、case7→GetInvenRef(3)、
// case0x12→GetCharacExpandData(kServerItemExpansion=9).GetInvenRefW(5)、
// case0x13..0x16→GetCharacExpandData(kTitleBook=0xe).getTitleSlot(area-0x13)。
// ORIG 共享出口 +0x172；default/guard 均 mov $0 进同一出口。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/GetCurCharacInventoryRef.inc"
}

// ORIG T 0x08680c8a（const 重载）：guard/getter 全部走 const R 版
// （getCurCharacInvenR/getCurCharacCargoR/GetInvenRefR/GetCargoRef const/
// GetCharacExpandDataR/GetInvenRefR）——ORIG 无 enableSaveInven 副作用，
// 仅 CTitleBook::getTitleSlot 直调非 const 版（_ZN…E 实证）。
#include "LinkClosureA_User/GetCurCharacInventoryRef_1.inc"
}

// ---------------------------------------------------------------------------
// CUser::UpdateRating（ORIG W 0x08568e9c 报告缺失；objdump 六条 call 序忠实）:
// enableSaveCharacStat(); SetRating(rating);
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/UpdateRating.inc"
}

// ---------------------------------------------------------------------------
// CUser::getUserQuestRewardItem（ORIG T 0x0866e914 报告全文已读）
// 字段全部位于 Quest：m_fieldDC(+0xdc vector<int>)/m_fieldE8(+0xe8)/
// m_rewardItems(+0xec)/m_titleReward(+0xc4)；其余探查经由第二参 const CUser*
// 的 getCurCharacQuestR()->isClearQuest(idx)。
//   mode==1：命中任一已清除 idx 即返回 &m_rewardItems；
//   否则：全部 idx 已清除才返回 &m_rewardItems；
//   空 idx 表或不满足一律返回 &m_titleReward。
// ---------------------------------------------------------------------------
// b28d-66 [P281]：CUser.h:3055 已 static（Main 代落）⇒ 实参 0x8/0xc(%ebp) 与 ORIG 一致。
//   本轮修正：ORIG 每个分支各自 `const_iterator it = quest->m_fieldDC.begin()/end()`
//   （const vector ⇒ 一次 call 直构，槽 else=-0xc/-0x10、then=-0x14/-0x18），无 const_cast
//   非 const begin 引起的 iterator→const_iterator 转换构造；else 分支为
//   `for(; it!=end; ++it) if(!isClearQuest(*it)) break;`（ORIG xor $1 证 break 形）。
#include "LinkClosureA_User/getUserQuestRewardItem.inc"
}

// ---------------------------------------------------------------------------
// CUser::getAddExpRateFromPremium（ORIG W 0x085bfd76 _ZN5CUser24getAddExpRateFromPremiumE17ENUM_PREMIUM_TYPE）
// this+0x7121c（m_premium）→ WongWork::CUserPremium::GetAddExpBonus(type)
// （ORIG W 0x085bf940）。转发链逐指令转写。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/getAddExpRateFromPremium.inc"
}

// ---------------------------------------------------------------------------
// CUser::compute_level_penalty（报告全文已读）
// m_questParameter(+0x8cc8).m_field18(+0x18 → +0x8ce0) /
// m_field1c(+0x1c → +0x8ce4)。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/compute_level_penalty.inc"
}

// ---------------------------------------------------------------------------
// CUser::compute_quest_basic_reward_exp（报告全文已读）
// ---------------------------------------------------------------------------
// b28d-65：m_field130 已按 P248档② bool 化（Quest.h）并改裸 if(!field) ⇒ xor$1 对齐；
//   残余槽整体平移 4（ORIG -0x24..-0x10 vs 4.4 -0x28..-0x14，N95/P232 族）⇒ 封顶。
#include "LinkClosureA_User/compute_quest_basic_reward_exp.inc"
}

// ---------------------------------------------------------------------------
// CUser::compute_quest_basic_reward_gold（报告全文已读）
// m_b8 gate: quest+0xb8==0 或 ==0x19 才发放；遍历 m_titleReward(+0xc4) 首个
// m_itemIdx==0 元素计算金币；quest+0x104=m_field104 为金币倍率。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/compute_quest_basic_reward_gold.inc"
}

// ---------------------------------------------------------------------------
// CUser::quest_basic_reward（报告全文已读）
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/quest_basic_reward.inc"
}

// ---------------------------------------------------------------------------
// CUser::CalculateExpertJobMaterials（报告全文已读）
// copy 向量上按 GetExpertJobCompoundMaterialVariation/100 缩减后再并回原表。
// P48 双迭代器形态照录（ci 在 copy 上、mi 在实参向量上同步推进）。
// ---------------------------------------------------------------------------
// b28d-65：残余 P0（jobLevel/jobType ORIG 直落 %ebx/%esi 无槽）+ N86（迭代器两段）+
//   槽向族；orig 165 vs ours 153，四形实测不可翻 ⇒ 封顶。
#include "LinkClosureA_User/CalculateExpertJobMaterials.inc"
}

// [B01 铁律1 / Main 批准] 原 `_forceLinkClosureAWeakSymbols`（static + used）为**发明符号**
// （ORIG `_ZL29_forceLinkClosureA…` 不存在），其目的（P147 强制发射 7 个弱模板实例）已由
// 其它 TU 自然发射覆盖（本机 nm 普查 841 个 .o）：`_ZNK…normal_iteratorIPi…4baseEv` 116 个、
// `…IPKi…C1ERKS2_` 82 个、`…C1IPiE`(enable_if 转换构造) 16 个、`pair 迭代器 ppEv` 16 个、
// `vector<int>::begin/end` 116/116、`vector<pair<int,int>>::begin` 98 个 ⇒ 删定义不丢符号。
