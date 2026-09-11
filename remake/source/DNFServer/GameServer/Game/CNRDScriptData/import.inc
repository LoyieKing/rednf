int CNRDScriptData::import(const char* path)
{
    if (!loadRDARScriptFile("Data/", path))
    {
        // ORIG import 失败分支（0x807fb4e 起）：logFormat 记录后返回 0。
        LogManager::logFormat(0, "../../DNFShared/CNRDScriptData.cpp", __FUNCTION__, 0x33,
                                 "CNRDScriptData::import: fail to load file - %s", path);
        return 0;
    }

    reset();
    m_path = path;

    std::string section;
    std::string token;
    std::string value;
    // ORIG 39-41：函数级两个带初值 temps（int→-0x54 `movl $0x0`；float→-0x58
    // `mov $0x0,%eax; mov %eax,-0x58`），供 [int]/[float] 两支复用；
    // 44-50 段首扫描为 `movl $1,0x4(%esp); call ScanType; xor $1; test; jne 远端出口岛`
    // ⇒ for(;;) + `if (!ScanType(...)) break;`（P164 + P32 取反物化），内层
    // `while (ScanStr(&token))` 保持 test/jne 回边形（ORIG 193-194 实证）。
    int v = 0;
    float f = 0;
    for (;;)
    {
        if (!ScanType(section, true))
        {
            break;
        }
        if (section == "[int]")
        {
            while (ScanStr(&token))
            {
                v = ScanInt((bool*)0);
                m_intMap.insert(token.c_str(), v);
            }
        }
        else if (section == "[float]")
        {
            while (ScanStr(&token))
            {
                f = ScanFloat((bool*)0);
                m_floatMap.insert(token.c_str(), f);
            }
        }
        else if (section == "[clong]")
        {
            while (ScanStr(&token))
            {
                int v = ScanInt((bool*)0);
                m_clongMap.insert(token.c_str(), v);
            }
        }
        else if (section == "[cfloat]")
        {
            while (ScanStr(&token))
            {
                float f = ScanFloat((bool*)0);
                m_cfloatMap.insert(token.c_str(), f);
            }
        }
        else if (section == "[string]")
        {
            while (ScanStr(&token))
            {
                ScanStr(&value);
                m_strMap.insert(token.c_str(), value);
            }
        }
    }
    return 1;
}