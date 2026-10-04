int Inter_LoadTicket::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x1a70;
    if (user->get_state() == 0)
        return 0x1a72;
    // ORIG 0x084c515a：pPkt 在 state 检查后才物化 -0x10；count 无槽（每次重载 +0x8）
    SIG_LOAD_TICKET* load = reinterpret_cast<SIG_LOAD_TICKET*>(data);
    if (load->m_field4 == 1)
        return 0x1a79;
    if (load->m_count > 99)
        return 0x1a7e;
    for (int i = 0; load->m_count > i; ++i)
        user->AddTicket(load->m_tickets[i], false);
    user->EnableCharacInfo((CACHE_SECTION)4);
    return 0;
}