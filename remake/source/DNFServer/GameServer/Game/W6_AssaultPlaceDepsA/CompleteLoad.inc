void CAssaultPlace::CompleteLoad(CUser* user)
{
    if (m_state != 2)
        throw (ENUM_ERROR)0x13;

    char allLoaded = 1;
    for (int i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082eb7d9 `test %al,%al; jne +0x169`
        //   （nop 岛）+ body 尾 `jmp +0x16a` ⇒ P223 双标签 goto 形（if{}else{} 被
        //   4.4 jump-thread，少 jmp+nop 两条）。slotUser 无具名槽（P0 内联，
        //   ORIG 0x082eb7f0 `cmp 0xc(%ebp),%eax; sete`）。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        {
            if (m_assaulters[i].GetUser() == user) {
                if ((int)m_assaulters[i].GetState() != 1)
                    throw (ENUM_ERROR)0x12;
                m_assaulters[i].SetState((ENUM_ASSAULT_STATE)2);
                PacketGuard packet;
                _MakeCompleteLoad(&packet, i);
                SendPacket(packet);
            }
            if ((int)m_assaulters[i].GetState() != 2)
                allLoaded = 0;
        }
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
    }

    if (allLoaded) {
        DoConnectP2P();
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x03');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x03');
    }
