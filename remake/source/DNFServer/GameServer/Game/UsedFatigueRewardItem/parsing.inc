int UsedFatigueRewardItem::parsing(char* path)
{
    Arad_Script script;
    if (!script.Load(path, "on_off"))
        return 0xa1;
    if (script.GetRowNum() == 0)
        return 0xa4;

    m_enable = false;
    std::string flag(script.GetString(0, "flag"),
                     std::allocator<char>());   // ORIG SaIcC1+SsC1(ptr,alloc)
    if (flag == std::string("true"))            // ORIG string==string（构造临时）
        m_enable = true;

    if (!script.Load(path, "item_reward"))
        return 0xac;
    if (script.GetRowNum() == 0)
        return 0xaf;

    for (int i = 0; i < script.GetRowNum(); ++i)
    {
        Data d;
        d.m_begin = (short)script.GetInt(i, "fatigue");
        d.m_end = (short)script.GetInt(i, "item_count");
        d.m_id = script.GetInt(i, "item_id");
        strncpy(d.m_str, script.GetString(i, "message"), 0xff);
        strncpy(d.m_name, script.GetString(i, "title"), 0x10);
        m_data.push_back(d);
    }
    return 0;
