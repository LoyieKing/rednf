// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExpertJobOnlyMaterial/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r Game/ExpertJobOnlyMaterial.cpp —— expert_job 配方材料域独占 TU。
// 承载 expert_job::ExpertJobMaterial 全部 10 个方法定义 + expert_job 自由
// 函数 5 个（配方材料查询/删除链）。逐函数转写自 docs/class_func_reports/
// expert_job__ExpertJobMaterial/*.md 与 docs/class_func_reports/expert_job/
// {DelMaterialOfRecipe,FindMonsterCard,GetRecipeInputItems,IsExistRecipeItem,
// UseMaterialOfRecipe}.md；Empty 无独立报告（README 仅列 9 函数），由
// objdump 直转并标注【objdump】。
//
// 关键布局依据：
//   ExpertJobMaterial: +0x00 m_user(CUser*) +0x04 m_item(CItem*)
//     +0x08 m_itemIndex +0x0c m_slot +0x0e m_field0e（CExpertJob.h 实证；
//     ORIG 存储口径均为整数槽，CUser*/CItem* 以 reinterpret_cast 往返）
//   CItem vtable 槽序（CItem.h）：slot3@+0x0c GetItemType、slot7@+0x1c
//     IsEnchantalbe —— 与 CheckType/IsRecipe 报告虚调用偏移一致
//   配方材料清单 = CStackableItem +0x174 RecipeInfo::m_vec0（ORIG 各函数
//     经 CItem* 裸偏移 +0x174 解释为 vector<pair<int,int>> 访问）
// 依赖外部符号（若链接缺失属其它 TU 生产者，本 TU 不造假定义）：
//   CUser::GetCurCharacInventoryRef(u16,u16)  T 0x08680f2e
//   CUser::CalculateExpertJobMaterials(vector<pair<int,int>>&) T 0x0868ba58
//   CStackableItem::get_sub_type() const      W 0x0822c9d2
// ============================================================================
#include <utility>
#include <vector>

#include "CDataManager.h"
#include "CExpertJob.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CUser.h"
#include "GameTypes.h"     // cMyTrace
#include "PacketGuard.h"   // PacketGuard : InterfacePacketBuf

namespace expert_job
{

// ---------------------------------------------------------------------------
// ExpertJobMaterial 成员
// ---------------------------------------------------------------------------

// 【objdump】ORIG _ZN10expert_job17ExpertJobMaterial5EmptyEv W @0x0849f286：
// m_item==0 || m_itemIndex==0 || m_user==0 → 1，否则 0。（无独立报告）
#include "ExpertJobOnlyMaterial/Empty.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial7SetDataEP5CUsertt @0x0849bf08。
// 第一个 ushort 实参恒被校验为 0 后原样回传（作为 inventory 子区码），
// slot 为背包槽位；从当前角色背包槽取出物品索引并解析 CItem*。
#include "ExpertJobOnlyMaterial/SetData.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial8IsRecipeEv @0x0849c114：
// 材料必须是 stackable 且 GetItemType()==3（配方类，vtable slot3 @+0x0c）。
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
#include "ExpertJobOnlyMaterial/IsRecipe.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial9CheckTypeEbi @0x0849bfd0：
// 非 stackable 材料（equipment 附魔卷轴类）只要求 IsEnchantalbe()
// （vtable slot7 @+0x1c）；stackable 材料要求 GetItemType()==itemType。
#include "ExpertJobOnlyMaterial/CheckType.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial7IsRightEv @0x0849c06e：
// 复核当前背包对应槽仍持有登记时的同一物品索引。
#include "ExpertJobOnlyMaterial/IsRight.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial15FindMonsterCardERKSt6vectorISt4p
// airIiiESaIS5_EE @0x0849c612：清单内第一枚 sub_type==1 的 stackable 材料
// （怪物卡片）。
// b28d-solo-ExpertJobOnlyMaterial: AE — 匿名 pair 值临时取 first 复现
// `mov 0x4(%eax),%edx; mov (%eax),%eax` 双段；P296 嵌套正向包裹消 continue 岛。
#include "ExpertJobOnlyMaterial/FindMonsterCard.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial14GetEnchantItemEv @0x0849c6ca。
#include "ExpertJobOnlyMaterial/GetEnchantItem.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial23IsExistMaterialOfRecipeEv
// @0x0849c166：配方成立且背包满足整份材料需求才通过。
// b28d-solo-ExpertJobOnlyMaterial: AE — P225 while+break 复现槽比零
// `cmpl;setne;test;je` 物化；P292 档① CheckNeedItemList bool 化消 test/sete。
#include "ExpertJobOnlyMaterial/IsExistMaterialOfRecipe.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial19DelMaterialOfRecipeEP5CUserRKSt
// 6vectorISt4pairIiiESaIS5_EEiP11PacketGuardRb @0x0849c204：
// 先 delete_event_items 清单材料（moneyReason 0xc / delReason 8 / 不改
// changed），失败记 trace 返 false；成功 put_byte(delCount+1)、删除配方
// 物品本体槽（count 固定 1）并按 byte/short/int 三连写本体条目，随后对
// 每个事件删除槽位同样三连写入。
// b28d-solo-ExpertJobOnlyMaterial: AE — P248 档② removedBody 局部 bool 化
// 复现 ORIG `movzbl -0x9; xor $1; test %al,%al; je`（char 局部 4.4 恒折 cmpb）。
#include "ExpertJobOnlyMaterial/DelMaterialOfRecipe.inc"
}

// ORIG _ZN10expert_job17ExpertJobMaterial19UseMaterialOfRecipeEP11PacketGu
// ard @0x0849c536：套用配方，删全部材料 + 本体；changed 时 send_equip(0x16)。
// b28d-solo-ExpertJobOnlyMaterial: AE — P0 删 m_user 具名缓存（逐点重读）；
// P171 删 changed 初始化（ORIG 只读 -0xd）；P225 while+break 槽比零物化。
#include "ExpertJobOnlyMaterial/UseMaterialOfRecipe.inc"
}

// ---------------------------------------------------------------------------
// 自由函数
// ---------------------------------------------------------------------------

// ORIG _ZN10expert_job15FindMonsterCardEP5CItem @0x0849be32。
// b28d-solo-ExpertJobOnlyMaterial: AE — 匿名 pair 值临时取 first（双段装载）
// + P296 嵌套正向包裹消 continue 岛；P49 逐点内联 m_recipe174.m_vec0。
CItem* FindMonsterCard(CItem* recipe)
{
    // b28d-36（P49）：ORIG 0x0849be32 内联 `lea 0x174(%eax),%edx`（helper 被
    // 4.1 前端内联）；4.4 -O0 发真实 call（DIFF+missing）⇒ 逐点内联成员访问。
    for (std::vector<std::pair<int, int> >::const_iterator it =
             reinterpret_cast<CStackableItem*>(recipe)
                 ->m_recipe174.m_vec0.begin();
         it != reinterpret_cast<CStackableItem*>(recipe)
                   ->m_recipe174.m_vec0.end();
         ++it)
    {
        CStackableItem* found = static_cast<CStackableItem*>(
            G_CDataManager()->find_item(std::pair<int, int>(*it).first));
        // b28d-solo-ExpertJobOnlyMaterial（P296）：嵌套正向包裹消 continue 岛
        // （ORIG 0x0849be92/0x0849bea1/0x0849beb6 三处 je 直落 0x0849bebd）。
        if (found != 0)
        {
            if (found->is_stackable())
            {
                if (found->get_sub_type() == 1)
                    return found;
            }
        }
    }
    return 0;
}

// ORIG _ZN10expert_job19GetRecipeInputItemsEP5CItemRSt6vectorISt4pairIiiES
// aIS4_EE @0x0849b734：从配方材料中筛出非怪卡（sub_type!=1）且可堆叠的
// 输入项追加到出参。
// b28d-solo-ExpertJobOnlyMaterial: AE — P171/P232 补函数头死初始化局部
// `int unused=0`（复现 ORIG `movl $0x0,-0x10(%ebp)` 并撑帧 0x24→0x34）；
// 匿名 pair 值临时取 first；P296 嵌套正向包裹消 continue 岛。
#include "ExpertJobOnlyMaterial/GetRecipeInputItems.inc"
}

// ORIG _ZN10expert_job17IsExistRecipeItemEP5CUserP5CItemb @0x0849b820：
// onlyInputs=true 时只校验输入材料，否则校验完整材料清单。
// b28d-solo-ExpertJobOnlyMaterial: AE — clear 移入两分支各自开头（ORIG
// 0x0849b845/0x0849b8b8 各一次）；else 分支 recipe 具名局部（-0xc 槽，
// P0）；两分支各自 CheckNeedItemList（ORIG 两个调用点）；P292 档① bool 化。
// 返回类型 int→bool（P292 档① callee 侧）：ORIG 调用点 _DoEnchant
// 0x0849cc58 `call; xor $1,%eax; test %al,%al; je` 无 setne ⇒ callee bool；
// 返回类型不入 mangled，函数体 return 0/1 不变（S370 同步改 CExpertJob.h:450）。
#include "ExpertJobOnlyMaterial/IsExistRecipeItem.inc"
}

// ORIG _ZN10expert_job19DelMaterialOfRecipeEP5CUserRSt6vectorISt4pairIiiES
// aIS4_EEiP11PacketGuardRbi @0x0849b957：与成员版差别——清单先经
// CalculateExpertJobMaterials 归一化；尾参 bodyCount 取 1 时附带删除并包
// 上报配方本体条目，其它值只上报材料。
// b28d-solo-ExpertJobOnlyMaterial: AE — P248 档②（同成员版 bool 局部）。
#include "ExpertJobOnlyMaterial/DelMaterialOfRecipe_1.inc"
}

// ORIG _ZN10expert_job19UseMaterialOfRecipeEP5CUserP5CItemP11PacketGuardb
// @0x0849bcfb。
// b28d-solo-ExpertJobOnlyMaterial: AE — 删 onlyInputs 分支多余的
// materials.clear()（ORIG C1Ev 后直接 GetRecipeInputItems）。
#include "ExpertJobOnlyMaterial/UseMaterialOfRecipe_1.inc"
}

}  // namespace expert_job
