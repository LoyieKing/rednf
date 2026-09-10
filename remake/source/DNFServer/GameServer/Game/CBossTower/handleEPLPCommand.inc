int CBossTower::handleEPLPCommand(int cmd)
{
    // ORIG 0x08142b7c..0x08142bb3：skip = (m_party==0) || (GetEPLPState()!=2)
    // 物化岛 mov$1/jmp/mov$0 + test %al,%al;je 无槽 ⇒ P226 否定式语句表达式。
    if (({ (m_party == 0) || (m_party->GetEPLPState() != 2); }))
        return 0;

    {
        // ORIG 0x08142bb3 `mov 0xc(%ebp),%eax; add $0x0,%eax; cmp $2,%eax; ja`
        // （int→unsigned 转换的 no-op add 为 4.1 特有；4.4 消除该条，P41 反向）
        // ⇒ 用连续 case switch 复现「形参载入 + 寄存器比较 + ja 落 _destroy」
        // （P286/P320），残余仅 1 条 no-op add。
        switch (cmd)
        {
        case 0:
        case 1:
        case 2:
            {
            char memberNames[255] = {0};          // ebp-0x110（ORIG 单段 rep stos）

            const char partyTypeByte =
                static_cast<char>(m_party->m_battleField.m_bloodState);   // 低字节 movsbl
            m_party->getMemberNames(memberNames);

            for (int i = 0; i <= 3; ++i)
            {
                CUser* member = m_party->get_user(i);
                if (member == 0 || m_party->checkValidUser(i) != 1)
                    continue;
                member->m_historyLog.LeaveDungeon(
                    m_dungeon->GetDungeonName(), 0, memberNames,
                    static_cast<int>(partyTypeByte));
            }
            m_party->ReturnToVillage();
            }
            break;
        default:
            break;
        }

        _destroy();
        return 1;
    }
    return 0;
