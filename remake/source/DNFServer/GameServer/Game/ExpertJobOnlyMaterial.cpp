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
bool ExpertJobMaterial::Empty()
{
    return m_item == 0 || m_itemIndex == 0 || m_user == 0;
}

// ORIG _ZN10expert_job17ExpertJobMaterial7SetDataEP5CUsertt @0x0849bf08。
// 第一个 ushort 实参恒被校验为 0 后原样回传（作为 inventory 子区码），
// slot 为背包槽位；从当前角色背包槽取出物品索引并解析 CItem*。
int ExpertJobMaterial::SetData(CUser* user, unsigned short area,
                               unsigned short slot)
{
    if (area != 0)
        return 0x15;

    Inven_Item* invenRef = user->GetCurCharacInventoryRef(area, slot);
    if (invenRef == 0)
        return 0x15;
    if (invenRef->m_addInfo == 0)
        return 0x15;

    m_itemIndex = static_cast<unsigned int>(invenRef->m_addInfo);
    m_item = G_CDataManager()->find_item(invenRef->m_addInfo);
    if (reinterpret_cast<CItem*>(m_item) == 0)
        return 0x15;

    m_user = user;
    m_slot = slot;
    m_field0e = area; // ORIG 0x0849bfbc 写 area（-0x1c(%ebp)）到 +0xe
    return 0;
}

// ORIG _ZN10expert_job17ExpertJobMaterial8IsRecipeEv @0x0849c114：
// 材料必须是 stackable 且 GetItemType()==3（配方类，vtable slot3 @+0x0c）。
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
int ExpertJobMaterial::IsRecipe()
{
    // b28：P0/P183 去缓存——ORIG 每次调用独立物化 m_item 载入（无具名局部槽），
    // vcall this 参数二次重读成员。
    if (!reinterpret_cast<CItem*>(m_item)->is_stackable())
        return 0x17;
    if (reinterpret_cast<CItem*>(m_item)->GetItemType() != 3)
        return 0x17;
    return 0;
}

// ORIG _ZN10expert_job17ExpertJobMaterial9CheckTypeEbi @0x0849bfd0：
// 非 stackable 材料（equipment 附魔卷轴类）只要求 IsEnchantalbe()
// （vtable slot7 @+0x1c）；stackable 材料要求 GetItemType()==itemType。
int ExpertJobMaterial::CheckType(bool checkStackable, int itemType)
{
    if (Empty())
        return 0x11;

    // b28d-36（修法，P0）：ORIG 0x0849bfd0 每次访问均重发 `mov 0x8(%ebp),%eax;
    // mov 0x4(%eax),%eax` 直传 is_stackable/GetItemType/IsEnchantalbe（无 item
    // 具名槽往返）⇒ 删局部、逐点内联 reinterpret_cast<CItem*>(m_item)。
    if (reinterpret_cast<CItem*>(m_item)->is_stackable() != checkStackable)
        return 0x17;

    if (checkStackable)
    {
        if (reinterpret_cast<CItem*>(m_item)->GetItemType() != itemType)
            return 0x17;
    }
    else if (!reinterpret_cast<CItem*>(m_item)->IsEnchantalbe())
    {
        return 0x17;
    }
    return 0;
}

// ORIG _ZN10expert_job17ExpertJobMaterial7IsRightEv @0x0849c06e：
// 复核当前背包对应槽仍持有登记时的同一物品索引。
int ExpertJobMaterial::IsRight()
{
    if (Empty())
        return 0x11;

    if (reinterpret_cast<CUser*>(m_user)->getCurCharacR() == 0)
        return 3;
    if (m_field0e != 0)
        return 0x11;

    Inven_Item* invenRef =
        reinterpret_cast<CUser*>(m_user)->GetCurCharacInventoryRef(m_field0e, m_slot);
    if (invenRef == 0)
        return 0x11;
    if (static_cast<unsigned int>(invenRef->m_addInfo) != m_itemIndex)
        return 0x11;
    return 0;
}

// ORIG _ZN10expert_job17ExpertJobMaterial15FindMonsterCardERKSt6vectorISt4p
// airIiiESaIS5_EE @0x0849c612：清单内第一枚 sub_type==1 的 stackable 材料
// （怪物卡片）。
// b28d-solo-ExpertJobOnlyMaterial: AE — 匿名 pair 值临时取 first 复现
// `mov 0x4(%eax),%edx; mov (%eax),%eax` 双段；P296 嵌套正向包裹消 continue 岛。
CItem* ExpertJobMaterial::FindMonsterCard(
    const std::vector<std::pair<int, int> >& materials)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             materials.begin();
         it != materials.end(); ++it)
    {
        CStackableItem* found = static_cast<CStackableItem*>(
            G_CDataManager()->find_item(std::pair<int, int>(*it).first));
        // b28d-solo-ExpertJobOnlyMaterial（P254 族）：ORIG 0x0849c63b 对 first
        // 发 `mov 0x4(%eax),%edx; mov (%eax),%eax` 双段 ⇒ 原码经匿名 pair 值临时
        // 取 first（具名局部落栈、裸 it->first 单段，探针实测）。
        // b28d-solo-ExpertJobOnlyMaterial（P296）：嵌套正向包裹消 continue 的
        // nop+jmp 中继岛（ORIG 三处 je 直落统一增量尾 0x0849c685）。
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

// ORIG _ZN10expert_job17ExpertJobMaterial14GetEnchantItemEv @0x0849c6ca。
CItem* ExpertJobMaterial::GetEnchantItem()
{
    if (Empty())
        return 0;
    if (IsRecipe() != 0)
        return 0;
    // ORIG 0x0849c6ca：inline helper get_recipe_materials 被 4.1 内联为
    // `mov 0x4(%eax),%eax; add $0x174,%eax`（m_item→CStackableItem+RecipeInfo
    // @+0x174 首向量 +0）。4.4 -O0 不外联该 inline ⇒ 退化 call；源形直接展开成员访问。
    return FindMonsterCard(
        reinterpret_cast<CStackableItem*>(m_item)->m_recipe174.m_vec0);
}

// ORIG _ZN10expert_job17ExpertJobMaterial23IsExistMaterialOfRecipeEv
// @0x0849c166：配方成立且背包满足整份材料需求才通过。
// b28d-solo-ExpertJobOnlyMaterial: AE — P225 while+break 复现槽比零
// `cmpl;setne;test;je` 物化；P292 档① CheckNeedItemList bool 化消 test/sete。
int ExpertJobMaterial::IsExistMaterialOfRecipe()
{
    if (Empty())
        return 0x11;

    int recipeErr = 0;
    recipeErr = IsRecipe();   // ORIG 0x849c17b 前置 movl $0x0,-0xc 预清（P198-①）
    // b28d-36（P225 试形）：ORIG 0x849c183 `cmpl $0x0,-0xc; setne %al;
    // test %al,%al; je` 为 4.1 槽比零 bool 物化；4.4 对 `if (x != 0)` 恒折
    // 直跳。while+break 形复现该四段物化（P225 定稿）。
    while (recipeErr != 0)
    {
        return recipeErr;
    }

    if (reinterpret_cast<CUser*>(m_user)->getCurCharacR() == 0)
        return 3;

    // ORIG 0x849c1cb：materials 经 lea 0x174(%eax) 内联（P49：勿经 static inline helper）；
    // 0x849c1ea `call; xor $1,%eax; test %al,%al; je` ⇒ callee 返回 bool
    // （P292 档①；CInventory.h/.cpp 的 CheckNeedItemList 已由 Main 代落 bool）。
    if (!reinterpret_cast<CUser*>(m_user)->getCurCharacInvenR()->CheckNeedItemList(
            reinterpret_cast<CStackableItem*>(m_item)->m_recipe174.m_vec0))
        return 0x15;
    return 0;
}

// ORIG _ZN10expert_job17ExpertJobMaterial19DelMaterialOfRecipeEP5CUserRKSt
// 6vectorISt4pairIiiESaIS5_EEiP11PacketGuardRb @0x0849c204：
// 先 delete_event_items 清单材料（moneyReason 0xc / delReason 8 / 不改
// changed），失败记 trace 返 false；成功 put_byte(delCount+1)、删除配方
// 物品本体槽（count 固定 1）并按 byte/short/int 三连写本体条目，随后对
// 每个事件删除槽位同样三连写入。
// b28d-solo-ExpertJobOnlyMaterial: AE — P248 档② removedBody 局部 bool 化
// 复现 ORIG `movzbl -0x9; xor $1; test %al,%al; je`（char 局部 4.4 恒折 cmpb）。
bool ExpertJobMaterial::DelMaterialOfRecipe(
    CUser* user, const std::vector<std::pair<int, int> >& materials,
    int slot, PacketGuard* packet, bool& changed)
{
    int materialCount = 1;   // 配方本体固定计 1 份

    std::vector<int> deletedSlots;
    std::vector<int> unusedSlots;                 // ORIG 0x849c221：未使用局部（保形）
    std::vector<int>::iterator it;                // ORIG 0x849c22c：迭代器前置构造
    int delCount = user->getCurCharacInvenW()->delete_event_items(
        materials, deletedSlots, changed,
        static_cast<eMoneySubReason>(0xc),
        static_cast<eItemDelReason>(8), false);
    if (delCount == 0)
    {
        cMyTrace(
            "bool expert_job::ExpertJobMaterial::DelMaterialOfRecipe("
            "CUser*, const IntPairVector&, int, PacketGuard*, bool&)",
            0x288, 5)(
            "ExpertJobMaterial::DelMaterialOfRecipe, "
            "getCurCharacInvenR()->delete_event_items , size does not "
            "match! %d %d!=%d",
            user->getCurCharacNo(), delCount, materials.size());
        return false;
    }

    packet->put_byte(delCount + materialCount);

    // b28d-solo-ExpertJobOnlyMaterial：P248 档②——局部改 bool 复现 ORIG
    // `movzbl -0x9; xor $1,%eax; test %al,%al; je`（char 局部 4.4 恒折 cmpb）。
    bool removedBody = user->getCurCharacInvenW()->delete_item(
        INVEN_TYPE_EQUIP, slot, materialCount,
        static_cast<eItemDelReason>(8), true);
    if (!removedBody)
    {
        cMyTrace(
            "bool expert_job::ExpertJobMaterial::DelMaterialOfRecipe("
            "CUser*, const IntPairVector&, int, PacketGuard*, bool&)",
            0x292, 5)(
            "CUser::compound_item, getCurCharacInvenW()->delete_item "
            "failed , %d %d",
            user->getCurCharacNo(), slot);
        return false;
    }

    packet->put_byte(0);
    packet->put_short(slot);
    packet->put_int(materialCount);

    if (delCount > 0)
    {
        for (it = deletedSlots.begin(); it != deletedSlots.end(); it++)
        {
            packet->put_byte(*it);
            it++;
            packet->put_short(*it);
            it++;
            packet->put_int(*it);
        }
    }
    return true;
}

// ORIG _ZN10expert_job17ExpertJobMaterial19UseMaterialOfRecipeEP11PacketGu
// ard @0x0849c536：套用配方，删全部材料 + 本体；changed 时 send_equip(0x16)。
// b28d-solo-ExpertJobOnlyMaterial: AE — P0 删 m_user 具名缓存（逐点重读）；
// P171 删 changed 初始化（ORIG 只读 -0xd）；P225 while+break 槽比零物化。
int ExpertJobMaterial::UseMaterialOfRecipe(PacketGuard* packet)
{
    if (Empty())
        return 0x11;

    // b28d-36：ORIG 0x0849c556 前置 `movl $0x0,-0xc` 预清（P198-①）+
    // `cmpl $0; setne %al; test %al,%al; je` 槽比零物化（P225 while+break 形）。
    int recipeErr = 0;
    recipeErr = IsRecipe();
    while (recipeErr != 0)
    {
        return recipeErr;
    }

    // b28d-solo-ExpertJobOnlyMaterial（P0）：ORIG 0x0849c59b/0x0849c5e9 均重读
    // m_user（无 user 具名槽）⇒ 删局部，每使用点内联。
    if (reinterpret_cast<CUser*>(m_user)->getCurCharacR() == 0)
        return 3;

    // b28d-solo-ExpertJobOnlyMaterial（P171）：ORIG 0x0849c5e9 只读 -0xd 槽，
    // 无 `movb $0` 初始化 ⇒ 原码 `bool changed;` 未初始化（出参由被调写入）。
    bool changed;
    if (!DelMaterialOfRecipe(reinterpret_cast<CUser*>(m_user),
                             reinterpret_cast<CStackableItem*>(m_item)
                                 ->m_recipe174.m_vec0,
                             static_cast<int>(m_slot), packet, changed))
        return 0x11;

    if (changed)
        reinterpret_cast<CUser*>(m_user)->send_equip(0x16);
    return 0;
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
void GetRecipeInputItems(CItem* recipe,
                         std::vector<std::pair<int, int> >& inputs)
{
    // b28d-solo-ExpertJobOnlyMaterial：ORIG 0x0849b737 `movl $0x0,-0x10(%ebp)`
    // 为函数头死初始化局部（P171/P232：照抄未使用局部，同时撑帧 0x24→0x34）。
    int unused = 0;
    (void)unused;
    // b28d-36（P49）：同 FindMonsterCard，删 helper/引用局部，逐点内联。
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
        // （ORIG 0x0849b79b/0x0849b7aa/0x0849b7bf 三处 je 直落 0x0849b7db）。
        if (found != 0)
        {
            if (found->is_stackable())
            {
                if (found->get_sub_type() != 1)
                    inputs.push_back(*it);
            }
        }
    }
}

// ORIG _ZN10expert_job17IsExistRecipeItemEP5CUserP5CItemb @0x0849b820：
// onlyInputs=true 时只校验输入材料，否则校验完整材料清单。
// b28d-solo-ExpertJobOnlyMaterial: AE — clear 移入两分支各自开头（ORIG
// 0x0849b845/0x0849b8b8 各一次）；else 分支 recipe 具名局部（-0xc 槽，
// P0）；两分支各自 CheckNeedItemList（ORIG 两个调用点）；P292 档① bool 化。
// 返回类型 int→bool（P292 档① callee 侧）：ORIG 调用点 _DoEnchant
// 0x0849cc58 `call; xor $1,%eax; test %al,%al; je` 无 setne ⇒ callee bool；
// 返回类型不入 mangled，函数体 return 0/1 不变（S370 同步改 CExpertJob.h:450）。
bool IsExistRecipeItem(CUser* user, CItem* recipe, bool onlyInputs)
{
    std::vector<std::pair<int, int> > materials;

    if (onlyInputs)
    {
        materials.clear();
        GetRecipeInputItems(recipe, materials);
        user->CalculateExpertJobMaterials(materials);
        if (materials.empty())
            return 0;
        if (!user->getCurCharacInvenR()->CheckNeedItemList(materials))
            return 0;
    }
    else
    {
        materials.clear();
        // b28d-solo-ExpertJobOnlyMaterial：ORIG 0x0849b8bd 把 recipe 存入
        // -0xc 具名槽后再判空（P0：有槽 ⇒ 原码有具名局部）。
        CItem* item = recipe;
        if (item == 0)
            return 0;
        materials = reinterpret_cast<CStackableItem*>(item)
                        ->m_recipe174.m_vec0;
        user->CalculateExpertJobMaterials(materials);
        if (!user->getCurCharacInvenR()->CheckNeedItemList(materials))
            return 0;
    }
    return 1;
}

// ORIG _ZN10expert_job19DelMaterialOfRecipeEP5CUserRSt6vectorISt4pairIiiES
// aIS4_EEiP11PacketGuardRbi @0x0849b957：与成员版差别——清单先经
// CalculateExpertJobMaterials 归一化；尾参 bodyCount 取 1 时附带删除并包
// 上报配方本体条目，其它值只上报材料。
// b28d-solo-ExpertJobOnlyMaterial: AE — P248 档②（同成员版 bool 局部）。
bool DelMaterialOfRecipe(CUser* user,
                         std::vector<std::pair<int, int> >& materials,
                         int slot, PacketGuard* packet, bool& changed,
                         int bodyCount)
{
    std::vector<std::pair<int, int> > normalized;
    normalized.clear();
    normalized = materials;
    user->CalculateExpertJobMaterials(normalized);

    std::vector<int> deletedSlots;
    std::vector<int> unusedSlots;                 // ORIG 0x849b9xx：未使用局部（保形）
    std::vector<int>::iterator it;                // ORIG 0x849b9xx：迭代器前置构造
    int delCount = user->getCurCharacInvenW()->delete_event_items(
        normalized, deletedSlots, changed,
        static_cast<eMoneySubReason>(0xc),
        static_cast<eItemDelReason>(8), false);
    if (delCount == 0)
    {
        cMyTrace(
            "bool expert_job::DelMaterialOfRecipe(CUser*, IntPairVector&, "
            "int, PacketGuard*, bool&, int)",
            0x1c0, 5)(
            "ExpertJobMaterial::DelMaterialOfRecipe, "
            "getCurCharacInvenR()->delete_event_items , size does not "
            "match! %d %d!=%d",
            user->getCurCharacNo(), delCount, normalized.size());
        return false;
    }

    if (bodyCount == 1)
    {
        packet->put_byte(delCount + bodyCount);
        // b28d-solo-ExpertJobOnlyMaterial：P248 档②（同成员版）。
        bool removedBody = user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, bodyCount,
            static_cast<eItemDelReason>(8), true);
        if (!removedBody)
        {
            cMyTrace(
                "bool expert_job::DelMaterialOfRecipe(CUser*, "
                "IntPairVector&, int, PacketGuard*, bool&, int)",
                0x1cf, 5)(
                "CUser::compound_item, getCurCharacInvenW()"
                "->delete_item failed , %d %d",
                user->getCurCharacNo(), slot);
            return false;
        }
        packet->put_byte(0);
        packet->put_short(slot);
        packet->put_int(bodyCount);
    }
    else
    {
        packet->put_byte(delCount);
    }

    if (delCount > 0)
    {
        for (it = deletedSlots.begin(); it != deletedSlots.end(); it++)
        {
            packet->put_byte(*it);
            it++;
            packet->put_short(*it);
            it++;
            packet->put_int(*it);
        }
    }
    return true;
}

// ORIG _ZN10expert_job19UseMaterialOfRecipeEP5CUserP5CItemP11PacketGuardb
// @0x0849bcfb。
// b28d-solo-ExpertJobOnlyMaterial: AE — 删 onlyInputs 分支多余的
// materials.clear()（ORIG C1Ev 后直接 GetRecipeInputItems）。
int UseMaterialOfRecipe(CUser* user, CItem* recipe, PacketGuard* packet,
                        bool onlyInputs)
{
    bool changed = false;
    if (onlyInputs)
    {
        std::vector<std::pair<int, int> > materials;
        GetRecipeInputItems(recipe, materials);
        if (materials.empty())
            return 0x11;
        if (!DelMaterialOfRecipe(user, materials, 0, packet, changed, 0))
            return 0x11;
    }
    else
    {
        if (!DelMaterialOfRecipe(user,
                                 reinterpret_cast<CStackableItem*>(recipe)
                                     ->m_recipe174.m_vec0, 0,
                                 packet, changed, 0))
            return 0x11;
    }

    if (changed)
        user->send_equip(0x16);
    return 0;
}

}  // namespace expert_job
