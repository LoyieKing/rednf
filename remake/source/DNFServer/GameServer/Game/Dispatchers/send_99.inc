void Dispatcher_TournamentRewardSelect::send(CUser* user, ParamBase& param)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1cb);
    if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(&param) + 4) == 0)
    {
        guard.put_byte(1);
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            CTournamentDungeonReward& reward =
                reinterpret_cast<CBattle_Field*>(&party->m_battleField)->m_tournamentReward;   // +0x17bc
            reward.MakeSelectPacket(&guard);
            guard.finalize(true);
            party->send_to_party(guard);

            // 全员已接收后结算（ORIG test %al 前取反：假 → 直接析构返回）
            if (!party->CheckRecvFromAllMember(user))
            {
                return;   // 未收齐：仅析构返回，不结算
            }
            std::vector<int> members;
            party->GetValidPartyMember(members);
            if (reward.isCardSelectComplete(
                    (ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
            {
                bool victory = party->isTournamentVictory();
                party->SetTournamentDungeonClearState(3);
                party->TournamentDungeonClear(victory);
            }
        }
    }
    // err != 0 或 party 缺失：仅析构返回（不 finalize、不发送）
