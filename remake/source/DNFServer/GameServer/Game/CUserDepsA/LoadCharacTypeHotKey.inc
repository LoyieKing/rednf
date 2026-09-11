// ----------------------------------------------------------------------------
// 4. ORIG T 0x08692d2a sz 0x171。
// hotkeyType = GetCharacHotKeyType(characJob)（==10）。装了热键加载开关时：
//   CheckLoadHotKeyCashedCharacType(cashed, job) 通过才发包——
//   PacketGuard: put_header(0,0x1c4)/put_byte(type)/put_int(0xbc)/
//   put_binary(get_hotkey_option(type),0xbc)/finalize(true)，Send 后析构；
// 未开开关：DB_LoadCharacHotKeyOption::makeRequest(GetUID, acc_id, type)。
// ----------------------------------------------------------------------------
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
}