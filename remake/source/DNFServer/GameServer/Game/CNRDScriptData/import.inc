int CNRDScriptData::import(const char* path)
{
    if (!loadRDARScriptFile("Data/", path))
    {
        // ORIG import 失败分支（0x807fb4e 起）：logFormat 记录后返回 0。
        // [R11fixE5] ORIG 0x0807fb55 串=0x8ae5e08，raw 逐字节（CP949）
        // 'error : %s script Data 파일을 찾을 수 없습니다.'
        // (= 61 72 72 6f 72 20 3a 20 25 73 20 73 63 72 69 70 74 20 44 61 74 61 20
        //    c6 c4 c0 cf c0 bb 20 c3 a3 c0 bb 20 bc f6 20 be f8 bd c0 b4 cf b4 d9 2e)。
        LogManager::logFormat(0, "../../DNFShared/CNRDScriptData.cpp", __FUNCTION__, 0x33,
                                 "error : %s script Data \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
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