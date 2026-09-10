int importPartyMatchingInfo(const char* path, std::vector<stPartyMatchingInfo>* out)
{
    // [b28d-solo-StScriptImporters] AE：指令数 180=180、p167 26/26 CLEAR。
    // [SweepStScriptImporters 2026-09-10 复核] jmp+jcc 28/28 目标下标逐位全等（回边 8=8）。
    // 修复：①补 ORIG -0x14 死 string 局部（槽序/析构序列照抄，P232/P171）；
    // ②外层改 for(;;){ if(!ScanType) break; } 破 4.4 循环旋转；
    // ③do-while 改 do{...if(!ok)break;}while(true) 复现 `movzbl;xor$1;test;je` + 入口 jmp；
    // ④[dungeon index] 的 while 改 for(;;){ if(!(idx=ScanInt(&ok),ok)) break; ... }。
    if (!loadRDARScriptFile("", path))
        return 0;
    std::string token;
    std::string str;    // [b28d-solo-StScriptImporters] ORIG -0x14 死 string 局部（槽序/析构序列照抄，P232/P171）
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[common dungeon index]")
        {
            int v;
            do
            {
                v = ScanInt(&ok);
                if (!ok)
                    break;
            } while (true);
        }
        else if (token == "[channel name]")
        {
            stPartyMatchingInfo info;
            ScanStr(&info.m_channelName);
            while (true)
            {
                if (ScanType(token, true) && token == "[/channel name]")
                    break;
                if (token == "[region name]")
                {
                    ScanStr(&info.m_regionName);
                }
                while (true)
                {
                    if (ScanType(token, true) && token == "[/region name]")
                        break;
                    if (token == "[dungeon index]")
                    {
                        int idx;
                        for (;;)
                        {
                            if (!(idx = ScanInt(&ok), ok))
                                break;
                            info.m_dungeonIdx.push_back(idx);
                        }
                        out->push_back(info);
                    }
                }
            }
        }
    }
    return 1;
