bool loadWorldMapCoordPattern(void)
{
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory,
                            "WorldMapPatternTable2ND.tbl"))
        return false;

    bool isAbs = true;                   // ebp-0x29 初值 1
    bool gotValue = false;               // ebp-0x2a 初值 0
    std::string token;
    for (int i = 0; i <= 0x13; ++i)
    {
        int base = (int)ScanAbsOrRel(isAbs, token, gotValue, true);
        if (!gotValue)
            break;                       // test al,al; jne 0x8ab0351
        WorldMapScript::aWorldMapPatternTable2ND_[i].m_base = base;
        for (int j = 0; j <= 2; ++j)
        {
            for (int k = 0; k <= 1; ++k)
            {
                WorldMapScript::aWorldMapPatternTable2ND_[i].m_coord[j][k] =
                    ScanInt((bool*)0);
            }
        }
    }
    return gotValue;                     // movzx ebx,[ebp-0x2a]
