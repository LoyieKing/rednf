int Inter_LoadCharacTypeHotKey::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)
        return 0;

    // ORIG 0x084e87f9：pPkt 在 state 岛后物化 -0x10（技巧13）。
    SIG_LOAD_CHARAC_HOTKEY_OPTION* load = reinterpret_cast<SIG_LOAD_CHARAC_HOTKEY_OPTION*>(data);
    // ORIG 0x084e8814 `mov %eax,-0xc`：gopt 具名槽；m_4 两处独立 reload（无 slot 槽）。
    CGameOption* gopt = user->GetGameOptionRef();
    gopt->SetHotKeyOption(
        (unsigned int)load->m_hotkeyType,
        *reinterpret_cast<CHotkeyOption*>(load->m_hotkey));

    PacketGuard packet;
    packet.put_header(0, 0x1c4);
    packet.put_byte((unsigned int)load->m_hotkeyType);
    packet.put_int(0xbc);
    packet.put_binary(load->m_hotkey, 0xbc);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}