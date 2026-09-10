void CUser::LoadCharacTypeHotKey(int cashedCharacJob, int characJob)
{
    unsigned char hotkeyType = GetCharacHotKeyType(characJob);

    if (GetGameOptionRef()->get_load_hotkey_flag(hotkeyType))
    {
        if (CheckLoadHotKeyCashedCharacType(cashedCharacJob, characJob))
        {
            PacketGuard packet;
            packet.put_header(0, 0x1c4);
            packet.put_byte(hotkeyType);
            packet.put_int(0xbc);
            packet.put_binary(
                reinterpret_cast<char*>(GetGameOptionRef()->get_hotkey_option(
                    hotkeyType)),
                0xbc);
            packet.finalize(true);
            Send(packet);
        }
    }
    else
    {
        DB_LoadCharacHotKeyOption::makeRequest(GetUID(), get_acc_id(),
                                               GetCharacHotKeyType(characJob));
    }
