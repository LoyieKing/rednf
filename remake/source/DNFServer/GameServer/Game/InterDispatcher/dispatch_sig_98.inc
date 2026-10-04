// [b28d-lane2] verdict=DIFF ndiff=76（ORIG 0x084d18ac）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（ORIG 0x084d18ac）：P248 档①（callee
// DeleteLetterKeepCount int→bool + 裸 `if (!f())`，Main 代落头/定义）⇒ 本波归零。
int Inter_ChangeLetterStat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_LETTER_STAT* st;
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    st = reinterpret_cast<SIG_LETTER_STAT*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != st->m_characNo)
        return 0x30a2;

    if ((short)st->m_state == 3)
    {
        if (static_cast<WongWork::CMailBox*>(user->GetMailBox()) != 0)
        {
            register int letterIdx = st->m_mailIdx;
            static_cast<WongWork::CMailBox*>(user->GetMailBox())
                ->InsertLetterKeepCount(letterIdx);
            static_cast<WongWork::CMailBox*>(user->GetMailBox())
                ->DecLoadedLetterCount();
        }
    }
    else if ((short)st->m_state == 0)
    {
        if (static_cast<WongWork::CMailBox*>(user->GetMailBox()) != 0)
        {
            register int deleteIdx = st->m_mailIdx;
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
    if (st->m_mailIdx == 0)
    {
        packet.put_byte(0);
        packet.put_byte(2);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int(st->m_mailIdx);
        packet.put_short((int)(short)st->m_state);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}