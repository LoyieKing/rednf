int Inter_MonitorGuildPointDel::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    std::vector<std::pair<int, int> > items;
    items.push_back(std::make_pair(0xc84, 0));
    G_GameWorld()->deleteSpecificItems(&items);
    Inter_MonitorGuildPointDel::setLastDeleteTime();
    return 0;
