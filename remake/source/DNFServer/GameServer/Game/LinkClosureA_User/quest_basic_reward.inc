void CUser::quest_basic_reward(Quest* quest, int& expOut, int& goldOut,
                               int& qpOut, int& pieceOut, bool send)
{
    expOut = compute_quest_basic_reward_exp(quest);        // [0x866e7c1]
    goldOut = compute_quest_basic_reward_gold(quest);      // [0x866e7d8]

    int ratio =                                            // [-0xc uint 除法，比较有符号 jle]
        getCurCharacHelpAbuseComputedRatio() / 10000u;     // [mul 0xd1b71759 shr 0xd]
    if (ratio > 100)                                       // [cmpl $0x64 jle]
    {
        ratio = 100;
    }

    if ((int)G_CDataManager()->m_questParameter.m_field20 < (int)ratio) // [+0x8ce8 setl]
    {
        expOut = expOut * (100 - ratio) / 100;             // [imul 0x51eb851f sar5]
        goldOut = goldOut * (100 - ratio) / 100;
    }

    qpOut = static_cast<int>(static_cast<short>(quest->m_field138)); // [movzwl/movswl]
    if (static_cast<short>(quest->m_field138) > 0)
    {
        incQuestPoint(static_cast<int>(static_cast<short>(quest->m_field138)),
                      quest->m_index, send);               // [0x866e8c0 incQuestPointEiib]
    }

    pieceOut = static_cast<int>(static_cast<short>(quest->m_field13a));
    if (static_cast<short>(quest->m_field13a) > 0)
    {
        incQuestPiece(static_cast<int>(static_cast<short>(quest->m_field13a)),
                      quest->m_index, send);               // [0x866e90d incQuestPieceEiib]
    }
