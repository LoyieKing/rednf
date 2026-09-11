// ORIG 0x8580d30：按 m_rewardItems.m_slots[index][mood].m_byDir[change] 权重表
//   随机抽取（get_rand_int(weight)）；权重 0 条目（m_a==0 && m_b==0）直接 break。
// [b28d-solo-NPCScript] ORIG 0x08580d30 已逐条对齐（帧 0x2c、push edi/esi/ebx、
// 槽 -0x28/-0x24/-0x20/-0x1c/-0x10 全等；p167 CLEAR 7=7）。唯一残差 =
// create_item 第 3 实参 `mov 0x4(%eax),%eax; mov %eax,%esi`（4.1 两段 regmove）
// vs 4.4 单段直落 callee-saved ⇒ **N18a 封顶**（P248 三档已试：callee 返回
// void / 字段 int 与形参 int 同型 / 无转换旋钮；反向亦不可造）。
Inven_Item CNPCScript::getGiftRewardItem(ENUM_NPC_MOOD mood,
                                         ENUM_NPC_FAVOR_CHANGE_CONST change,
                                         unsigned int index) const
{
    Inven_Item result;
    // [b28d-solo-NPCScript] ORIG 0x08580d30：`-0x28` 槽是 `&m_byDir[change]`
    // 具名引用（循环 at/size 直接用它，不再重算 change*0xc）；weight 实参
    // 则由完整表达式重算（P157/P183 地址重物化）。ORIG 比较形
    // `mov 0x8(%eax),%eax; cmp -0x24(%ebp),%eax; jbe`（无中转）⇒ r 与
    // m_w 同为无符号；ORIG 第二次 at 只为取 m_b，m_a 取自第一次的 e（P167）。
    const std::vector<stFavorableRelationShip_t::stRewardItemElement_t>& byDir =
        m_npcScript.m_favorableRelationShip.m_rewardItems.m_slots[index][mood]
            .m_byDir[change];
    unsigned int r = static_cast<unsigned int>(get_rand_int(static_cast<int>(
        m_npcScript.m_favorableRelationShip.m_rewardItems.m_slots[index][mood]
            .m_weight[change])));
    for (unsigned int i = 0; i < byDir.size(); ++i)
    {
        const stFavorableRelationShip_t::stRewardItemElement_t& e = byDir.at(i);
        if (static_cast<unsigned int>(e.m_w) > r)
        {
            if (e.m_a == 0 && e.m_b == 0)
            {
                break;
            }
            result.m_addInfo = e.m_a;
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_0, result, byDir.at(i).m_b);
            break;
        }
    }
    return result;
}