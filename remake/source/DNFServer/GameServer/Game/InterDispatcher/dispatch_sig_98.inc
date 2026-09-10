int Inter_ChangeLetterStat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ChangeLetterStatPktView* pPkt;
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    pPkt = reinterpret_cast<Inter_ChangeLetterStatPktView*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_0)
        return 0x30a2;

    if ((short)pPkt->m_8 == 3)
    {
        if (static_cast<WongWork::CMailBox*>(user->GetMailBox()) != 0)
        {
            register int letterIdx = pPkt->m_4;
            static_cast<WongWork::CMailBox*>(user->GetMailBox())
                ->InsertLetterKeepCount(letterIdx);
            static_cast<WongWork::CMailBox*>(user->GetMailBox())
                ->DecLoadedLetterCount();
        }
    }
    else if ((short)pPkt->m_8 == 0)
    {
        if (static_cast<WongWork::CMailBox*>(user->GetMailBox()) != 0)
        {
            register int deleteIdx = pPkt->m_4;
            // [b28d-solo-InterDispatcher] P248 档①：ORIG 0x084d19aa 调用点为
            // `call; xor $0x1,%eax; test %al,%al; je`（3 条）⇒ callee 返回 bool；
            // 裸 `if (!f())` 即复现（Main 已代落 CMailBoxHelper.h/W6_RankMailDeps.cpp int→bool）。
            if (!static_cast<WongWork::CMailBox*>(user->GetMailBox())
                     ->DeleteLetterKeepCount(deleteIdx))
                static_cast<WongWork::CMailBox*>(user->GetMailBox())
                    ->DecLoadedLetterCount();
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x89);
    if (pPkt->m_4 == 0)
    {
        packet.put_byte(0);
        packet.put_byte(2);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_4);
        packet.put_short((int)(short)pPkt->m_8);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
