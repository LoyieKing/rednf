// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08216dcc orig/ours=163/167 ndiff-blocks=29
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | lea    -0x19(%ebp),%eax | movl   $&_ZZN33Dispatcher_DungeonEventStoryPause12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,
int Dispatcher_DungeonEventStoryPause::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08216dcc: state==5 门
    if (user->get_state() != 5)
        return 0;

    // +0x034: 读 flag1（paused 字段）/ flag2（resume 全体标志）
    // ORIG 0x08216df3 / 0x08216e35 直接 `lea -0x19(%ebp)/-0x1a(%ebp),%eax` 取址后
    //   get_byte(char&)，两处之前均无 `movb $0x0,-0x19/-0x1a(%ebp)`（我方 -0x21/-0x22
    //   各多一条零初始化）⇒ 原码是**未初始化**的 char 局部：get_byte 失败即 return，
    //   成功路径必已赋值，故删 `= 0`（P328⑤ 去预清）。
    char flag1;
    if (!packet.get_byte(flag1))
    {
        return LineFunc(0xc886, __PRETTY_FUNCTION__, 0, 0);
    }
    char flag2;
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
}