int Dispatcher_DungeonEventStoryPause::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08216dcc: state==5 门
    if (user->get_state() != 5)
        return 0;

    // +0x034: 读 flag1（paused 字段）/ flag2（resume 全体标志）
    char flag1 = 0;
    if (!packet.get_byte(flag1))
    {
        return LineFunc(0xc886, __PRETTY_FUNCTION__, 0, 0);
    }
    char flag2 = 0;
    if (!packet.get_byte(flag2))
    {
        return LineFunc(0xc88b, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x0b1: 角色非空才继续
    if (user->getCurCharacR() != 0)
    {
        CParty* party = (CParty*)user->GetParty();
        UserQuest* quest = user->getCurCharacQuestR();
        if (quest == 0)
            return 0;
        if (party == 0)
            return 0;

        // +0x103: 判定是否广播暂停
        char pauseValid = 0;
        if (flag2 == 0)
        {
            // party+0xb24 内嵌 CBattle_Field：getMaze / GetCurrentMapIndex
            CBattle_Field* battleField =
                &party->m_battleField;
            if (battleField->getMaze() != 0)
            {
                int mapIndex = battleField->GetCurrentMapIndex();
                int dungeonNo = user->getPlayingDungeonIndex();
                if (quest->CheckDungeonEventStoryPause(dungeonNo, flag2, mapIndex))
                {
                    pauseValid = 1;
                }
            }
        }
        else if (flag2 == 1)
        {
            pauseValid = 1;
        }

        // +0x186: 广播 0xaa 通知
        if (pauseValid != 0)
        {
            PacketGuard pg;
            pg.put_header(0, 0xaa);
            pg.put_short(static_cast<int>(static_cast<unsigned short>(user->get_unique_id())));
            pg.put_byte(static_cast<int>(flag1));
            pg.put_byte(static_cast<int>(flag2));
            pg.finalize(true);
            party->send_to_party(pg);
        }
    }
    return 0;
