// ---- respond_special_item_skill_tree_style —— ORIG 0x0827ce7e ----
// 置技能树 index 0 → UpdateData → InitSkill(user, true, kind=1)；组包
// (header 0/2, byte 0, short 1, make_basic_info, finalize) 后 Send。
void SpecialItemHandler::respond_special_item_skill_tree_style(int,
                                                               const STSpecailItem_Param& param,
                                                               STSpecailItem_Result&)
{
    // b28d-D42 P28：ORIG 0x0827ce7e 先取 mgr（-0x10 槽）后取 user（-0xc 槽）。
    CPremiumLetheManager* mgr = G_CGameManager()->GetPremiumLetheManager();
    CUser* user = static_cast<CUser*>(param.m_user);
    // b28d-D42 P227：ORIG 0x0827ce7e 区 edi/ebx 缓存两变参值（先于 ctor）⇒
    // 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 499, 0)(
        "SKILL_TREE_STYLE : BUY_SKILL_TREE_STYLE, char(%s), char_no(%d)",
        user->getCurCharacName(), user->getCurCharacNo());
    user->SetCurCharacSkillTreeIndex(0);
    user->UpdateData();
    mgr->InitSkill(user, true, (ENUM_SKILL_TREE_KIND)1);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    user->Send(packet);
}