int Inter_LoadCharacTypeHotKey::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)
        return 0;

    // ORIG 0x084e87f9：pPkt 在 state 岛后物化 -0x10（技巧13）。
    Inter_LoadCharacTypeHotKeyPktView* pPkt = reinterpret_cast<Inter_LoadCharacTypeHotKeyPktView*>(data);
    // ORIG 0x084e8814 `mov %eax,-0xc`：gopt 具名槽；m_4 两处独立 reload（无 slot 槽）。
    CGameOption* gopt = user->GetGameOptionRef();
    gopt->SetHotKeyOption(
        (unsigned int)pPkt->m_4,
        *reinterpret_cast<CHotkeyOption*>(reinterpret_cast<char*>(pPkt) + 5));

    PacketGuard packet;
    packet.put_header(0, 0x1c4);
    packet.put_byte((unsigned int)pPkt->m_4);
    packet.put_int(0xbc);
    packet.put_binary(reinterpret_cast<char*>(pPkt) + 5, 0xbc);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}