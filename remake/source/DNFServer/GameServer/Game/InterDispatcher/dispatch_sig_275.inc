int Inter_LoadKillMonsterInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x6248;
    charac_expand::CData* p = reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xc));
    p->loadData(user, data);
    user->EnableCharacInfo((CACHE_SECTION)9);
    return 0;
