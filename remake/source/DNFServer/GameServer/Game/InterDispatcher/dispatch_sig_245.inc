int Inter_LoadAccountCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadAccountCargoPktView* pPkt = reinterpret_cast<Inter_LoadAccountCargoPktView*>(data);
    if (user->get_state() <= 1)
        return 0;
    if (user->IsExistAccountCargo())
    {
        // [b28d-solo-InterDispatcher] P227 全表达式临时链：ORIG 0x084e24ec 段把两个
        // vararg 在 cMyTrace ctor 前求值并以 %esi/%ebx 跨 ctor 保活（`mov %eax,%esi;
        // mov %eax,%ebx; …ctor…; mov %esi,0xc(%esp); mov %ebx,0x8(%esp)`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x51e9, 5)(
            "AccountCargo duplicated!(%s(%s))",
            user->get_acc_name(),
            NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    user->SetAccountCargoDBData(
        reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(pPkt) + 0x8),  // ORIG 从 pPkt 槽 -0xc 取址
        pPkt->m_4,
        pPkt->m_0);
    return 0;
