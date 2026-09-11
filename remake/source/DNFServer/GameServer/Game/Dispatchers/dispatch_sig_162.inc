// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08226f30 orig/ours=166/160 ndiff-blocks=37
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    $0x0,%ebx | mov    %eax,-0xc(%ebp) | cmpl   $0x0,-0xc(%ebp)
int Dispatcher_SelectUltimateDifficulty::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08226f30（PacketDispatcher_Impl_1.cpp 0xe2d9 区）
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->getManager() != user)
    {
        return 0;
    }
    CBattle_Field* battleField =
        &party->m_battleField;
    if (2 < battleField->getBloodState())
    {
        return 0;
    }
    char difficulty = 0;
    if (!packet.get_byte(difficulty))
    {
        return LineFunc(0xe2d9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if ((signed char)difficulty != 1 && (signed char)difficulty != 2)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__,
                              0xe2dd, "ULTIMATE_LOG : Difficulty ERROR!! difficulty (%d)\n",
                              (int)(signed char)difficulty);
        return 0;
    }
    if (party->m_battleField.m_pDungeon == 0 || party->m_battleField.m_pDungeon->get_index() != 0x2aff)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__,
                              0xe2e3,
                              "ULTIMATE_LOG : Dispatcher_SelectUltimateDifficulty ERROR!!\n");
    }
    else if (battleField->getUltimateDifficylty() == 0)
    {
        battleField->setUltimateDifficulty(difficulty);
        PacketGuard guard;
        guard.put_header(1, 0x13e);
        guard.put_byte(1);
        guard.put_byte((int)(signed char)difficulty);
        guard.finalize(true);
        party->send_to_party(guard);
        battleField->startUltimateRound();
    }
    return 0;
}