int ImportBroadCastItemScript(const char* path, stBroadCastItemScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool ok = false;

    for (;;)                                   // ORIG 4.1 while 落点形：check 在头
    {
        if (!ScanType(token, true))
            break;
        if (token == "[common items]")
        {
            while (true)
            {
                int itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                script.m_vec0.push_back(itemIdx);
            }
        }
        else if (token == "[uncommon items]")
        {
            int group = ScanInt(&ok);
            if (!ok)
                break;                           // ORIG：直接退出主循环

            std::vector<int> items;
            while (true)
            {
                int itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                items.push_back(itemIdx);
            }
            script.m_mapC.insert(std::make_pair(group, items));
        }
    }
    return 0;
