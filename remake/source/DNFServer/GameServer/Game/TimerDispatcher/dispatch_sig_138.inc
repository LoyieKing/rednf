bool Timer_AssaultGhostTimer::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);

    if (user == 0)
    {
        return 0;
    }

    // ORIG 0x0863a??（报告 Timer_AssaultGhostTimer/dispatch_sig.md）：
    // getCurCharacNo(unsigned) != p3 同型 ⇒ cmp 0x14(%ebp),%eax 直比。
    if (user->getCurCharacNo() != p3)
    {
        return 0;
    }

    if (user->IsCurCharacGhost())
    {
        user->SetCurCharacGhost(false);
        user->SetCurCharacChaosDieTime(0);
        user->SetCurCharacChaosResponTime(0);
        user->m_characterView.enableSaveCharacView();

        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 3);
        packet.put_byte(1);
        packet.put_short(user->get_unique_id() & 0xffff);
        packet.put_byte(0);
        packet.finalize(true);

        G_GameWorld()->send_all(packet, user);
    }

    return 1;
}