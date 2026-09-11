void DB_Login::_LoadEventItem(SIG_LOGIN_DATA* sig, int serverId,
                              vecDNFEventEntry_t& entries)
{
    // [b28d-solo-DB_Login] P167 差集：ORIG 走 begin/end/operator->/operator++/operator!=
    // （我方旧索引形多 size/operator[]）；迭代器双缓存照 P13，箭头形照 P167 新判据。
    int seen[10] = {0};
    vecDNFEventEntry_t::iterator it = entries.begin();
    vecDNFEventEntry_t::iterator itEnd = entries.end();
    while (it != itEnd)
    {
        // P223 负条件 goto 尾标签：复现 ORIG `setg; test; jne <nop 岛>` 布局。
        if (it->m_eventId > 9)
        {
            goto next;
        }
        seen[it->m_eventId] = 1;
    next:
        ++it;
    }
    for (int eventId = 0; eventId < 10; ++eventId)
    {
        if (seen[eventId] == 0)
        {
            continue;
        }
        switch (eventId)
        {
        case 2:
            WongWork::EventATI::ArrangeATIEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 3:
            WongWork::EventSigong::ArrangeSigongEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 5:
            WongWork::EventMuz::ArrangeMuzEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 6:
            WongWork::EventAlttle::ArrangeAlttleEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        case 7:
            WongWork::EventFCCombo::ArrangeFCComboEventItem(
                sig->m_accId, (unsigned int)serverId);
            break;
        default:
            break;
        }
    }
}