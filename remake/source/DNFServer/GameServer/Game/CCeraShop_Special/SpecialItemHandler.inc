// ---- SpecialItemHandler::SpecialItemHandler —— ORIG 0x0827bcec ----
// 对象布局：+0x00 CCeraShop* m_owner；+0x04.. 14 组 {respond fn, offset} 派发表。
// ctor 先清零 14 组，再按 type 填 dispatch 表（respond fn 地址，offset 全 0）。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] PMF 赋值成对 store 复现 ORIG fn+adj 写
SpecialItemHandler::SpecialItemHandler(WongWork::CCeraShop* owner)
{
    // b28d-solo：Entry.fn 改真实 PMF（见 SpecialItemHandler.h 判据）。PMF 赋值
    // 在 4.4 发 ptr+adj 双 store，与 ORIG 每项 `movl $fn,disp; movl $0,disp+4`
    // 成对写同形。
    m_owner = owner;
    for (int i = 0; i < 0xe; ++i)
        m_entries[i].fn = 0;
    // ORIG 0x0827bcec：按登记表序写 PMF（ptr + adj=0 成对 store）。
    m_entries[1].fn = &SpecialItemHandler::respond_special_item_cargo;
    m_entries[7].fn = &SpecialItemHandler::respond_special_item_cargo_abs;
    m_entries[11].fn = &SpecialItemHandler::respond_special_item_inventory_upgrade;
    m_entries[8].fn = &SpecialItemHandler::respond_special_item_charac_slot_extend;
    m_entries[5].fn = &SpecialItemHandler::respond_special_item_package;
    m_entries[6].fn = &SpecialItemHandler::respond_special_item_booster;
    m_entries[12].fn = &SpecialItemHandler::respond_special_item_booster_selection;
    m_entries[9].fn = &SpecialItemHandler::respond_special_item_skill_confirm;
    m_entries[10].fn = &SpecialItemHandler::respond_special_item_skill_tree_style;
    m_entries[2].fn = &SpecialItemHandler::respond_special_item_premium;
    m_entries[13].fn = &SpecialItemHandler::respond_special_item_cerapoint_giftcard;
}