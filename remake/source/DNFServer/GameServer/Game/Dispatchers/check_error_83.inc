int Dispatcher_PartyTeleport::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    MSG_PARTY_TELEPORT& req = reinterpret_cast<MSG_PARTY_TELEPORT&>(msg);
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsPvPSkilTreeChannel())
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());   // GetParty 返回 void*
    if (party == 0)
    {
        return 0x13;
    }
    if (party->getManager() != user)                     // 仅队长可发起
    {
        return 0x17;
    }
    Village* village = G_GameWorld()->getUserVillage(static_cast<int>(req.m_field0d));
    if (village == 0)
    {
        return 0x13;
    }
    if (static_cast<int>(req.m_field0e) == village->get_gate_area())   // 目标=城门禁传
    {
        return 0x13;
    }
    if (!user->isGMUser())
    {
        // [b28d-solo-Dispatchers] P212/N88：ORIG 0x081dc2xx 的 `movl $0x0,-0x3c(%ebp)`
        //   位于 `call isGMUser; xor $1; test; je` **之后**（进入 if 体才初始化）
        //   ⇒ 声明移入块内（块外声明会提前到分支前）。
        int auraSlotCount = 0;
        // 返回值>0 时直接作为错误码透传（光环头像占用槽位检查）。
        // ORIG `call; mov %eax,-0x28` 存 raw %eax 无 movsbl；4.4 int=<char返回>
        // 恒加 `movsbl %al,%eax`（register asm 亦无效）——4.1 信任 callee %eax
        // 已符号扩展 vs 4.4 重推，P196 fold 记 N。
        int auraEquip = user->is_equip_aura_avatar('\0', auraSlotCount);
        if (auraEquip > 0)
        {
            return auraEquip;
        }
    }
    int auraExpire = user->get_aura_avatar_option_value(0);
    if (!user->isGMUser() &&
        GlobalData::s_systemTime_.getCurSec() < auraExpire)
    {
        return 0x16;                                     // 光环传送券未过期→冷却
    }
    for (int i = 0; i < 4; ++i)                          // P2：i<=3 有符号循环
    {
        // [b28d-solo-Dispatchers] P296/P222：ORIG 0x081dc3xx `call checkValidUser;
        //   test %al,%al; je <跳过 body>`（正极性直测）⇒ 正向 if 包裹（`==0 continue`
        //   形 4.4 恒产 `xor $1; test; jne` 反极性）。
        if (party->checkValidUser(i) != 0)
        {
            CUser* member = party->get_user(i);
            if (member == 0)
            {
                continue;
            }
            int moveErr = G_GameWorld()->check_move_area(
                member, static_cast<int>(req.m_field0d), static_cast<int>(req.m_field0e),
                static_cast<int>(static_cast<short>(req.m_field0f)),
                static_cast<int>(static_cast<short>(req.m_field11)),
                static_cast<int>(req.m_field13), true, 0);
            if (moveErr > 0)
            {
                return moveErr;
            }
        }
    }
    return 0;
