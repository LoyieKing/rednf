bool importSeriaRoomDecoEvent(class seriaRoomDecoEventScript* s,
                              const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string deadToken;                         // ORIG -0x24 死 string（P171）
    (void)deadToken;
    int value = 0;
    bool ok = false;                               // ORIG -0x9 死局部（P171）
    (void)ok;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                                 // EOF → 共享尾块 return true
        if (token == "[object max count]")
        {
            ScanInt(&s->m_maxEventItemCount);
        }
        else if (token == "[using item]")
        {
            s->m_eventItemIdx.clear();
            for (;;)
            {
                // ORIG 0x895e58c：ScanInt(int*) 返回 bool（非 ScanInt(bool*)）
                if (!ScanInt(&value))
                    break;
                s->m_eventItemIdx.push_back(value);
            }
        }
        else if (token == "[fatigue]")
        {
            ScanInt(&s->m_fatigueCondition);
        }
        else if (token == "[reward list]")
        {
            for (;;)
            {
                // ORIG 0x895e5f0：std::pair<int,int> 局部（-0x30）+ 转换构造
                std::pair<int, int> p;
                if (!ScanInt(&p.first))
                    break;                             // 首列失败回标签扫描
                ScanInt(&p.second);                    // 第二列不校验
                s->m_matchEventMap.insert(p);
            }
        }
        else if (token == "[reward item mail expire]")
        {
            ScanInt(&s->m_rewardMailExpireDate);
        }
        else if (token == "[sound info]")
        {
            s->m_soundList.clear();
            seriaRoomDecoEventSoundInfo elem;
            for (;;)
            {
                elem.clear();                          // ORIG 循环首 clear
                // ORIG 0x895e6bd：ScanInt(int*)（行序号弃读）
                if (!ScanInt(&value))
                    break;
                ScanInt(&elem.m_field0);
                ScanInt(&elem.m_field4);
                ScanStr(&elem.m_name);
                ScanInt(&elem.m_fieldC);
                s->m_soundList.push_back(elem);
            }
        }
    }
    return true;
