int Dispatcher_Select_Card::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ff7de: state==5 门
    if (user->get_state() != 5)
        return 0;

    // +0x02c: getMoveSpace()==1 → 放弃
    if (user->getMoveSpace() == 1)
        return 0;

    // +0x04b: GetPartyIndex → CGameManager::GetParty
    // ORIG 0x81ff829：GetPartyIndex 返回 movswl %ax,%ebx 单条 sext 入 %ebx（asm 具名，无槽）
    // [b28d-solo-S206] P200：ORIG `call GetPartyIndex; movswl %ax,%ebx`（无栈槽）⇒
    //   删具名 partyIndex，内联实参直传。
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return LineFunc(0x9371, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x0a8: 副本为 TowerOfDespair → 放弃
    if (party->m_battleField.m_pDungeon != 0 && party->m_battleField.m_pDungeon->isTowerOfDespairDungeon())
        return 0;

    // +0x0e4: 读 cardType（0..1 有效）
    char cardType;                 // ORIG 槽 -0xe 无初始化
    if (!packet.get_byte(cardType))
    {
        return LineFunc(0x9382, __PRETTY_FUNCTION__, 0, 0);
    }
    if (cardType < 0 || cardType > 1)
    {
        return LineFunc(0x9384, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x15f: 读 cardNo
    char cardNo;                   // ORIG 槽 -0xd 无初始化
    if (!packet.get_byte(cardNo))
    {
        return LineFunc(0x9392, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x19a: clear_state > 4 → 放弃
    if (party->get_dungeon_clear_state() > 4)
        return 0;

    // +0x1c0: 全员未选卡完成才登记
    if (!party->IsCompleteCardSelectOfMembers(static_cast<eClearRewardCardType_t>(2)))
    {
        // +0x1ef: SetCardNumber(user, cardNo, cardType)
        if (!party->SetCardNumber(user, cardNo, static_cast<eClearRewardCardType_t>(cardType)))
            return 0;
        party->SendEachCardInfo();
    }

    // +0x21a: 全员已接收 → 若全员选卡完成则推进 clear state 5 并 dungeon_clear
    if (!party->CheckRecvFromAllMember(user))
        return 0;
    if (party->IsCompleteCardSelectOfMembers(static_cast<eClearRewardCardType_t>(2)))
    {
        party->set_dungeon_clear_state(5);
        party->dungeon_clear();
    }
    return 0;
