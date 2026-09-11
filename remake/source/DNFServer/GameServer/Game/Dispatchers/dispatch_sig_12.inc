int Dispatcher_DieMobAll::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x81ecce6 (89 insn)：战斗中全灭上报；state==5 时检查
    // 副本未清且(非0x2afe图或无副本)记 hack。GetParty(GetPartyIndex()) 为查找，
    // 勿用无参 GetParty()（会新建 party）。
    if (user->get_state() == 5)
    {
        // ORIG：GetPartyIndex 短整返回值作普通实参直传（P200），
        // `call GetPartyIndex; movswl %ax,%ebx` 单条 sext 跨 G_CGameManager() 存活于 %ebx。
        // 禁 register asm/具名中间局部（退化 cwtl+mov 两步）。
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
        {
            return LineFunc(0x6ef6,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        // ORIG：field 地址无槽，就地 add $0xb24 传参（不落局部引用）
        if (!reinterpret_cast<CBattle_Field*>(&party->m_battleField)
                ->IsClearDungeon())   // ORIG xor $1,test al → 未清才进 body
        {
            if (party->m_battleField.m_pDungeon == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1, 0, 0);
            }
            else
            {
                if (static_cast<CParty*>(user->GetParty())->m_battleField.m_pDungeon->get_index()
                    == 0x2afe)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1, 2, 0);
                }
                else
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1, 0, 0);
                }
            }
        }
    }
    return 0;
}