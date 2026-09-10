int importBloodDungeonScript(const char* path, stBloodDungeon_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x191c;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return 0;
        }

        if (token == "[reward exp weight]")
        {
            int i = 0;                              // 本表下标从 0 开始
            for (;;)
            {
                out.m_rate[i] = ScanFloat(&ok);     // ORIG fstps 成员直写、后查 ok
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[exp standard monster cnt]")
        {
            // ORIG 0890e1/0890e4 `mov 0xc(%ebp),%edx; mov %eax,0x320(%edx)` ⇒ 直存，
            // 无具名 int 局部（P0/配方⑦）。
            out.m_field320 = ScanInt(&ok);
            if (!ok) { return 0x192f; }
        }
        else if (token == "[reward item prob]")
        {
            // ORIG 0890911e..0890919a：三次 ScanInt 结果均 `mov %eax,%edx` 后与成员
            // 相加/直存，无具名 int 局部（P0/配方⑦）；ok 检查在各直存之后。
            out.m_field324[0] = ScanInt(&ok);
            if (!ok) { return 0x1936; }
            out.m_field324[1] = out.m_field324[0] + ScanInt(&ok);
            if (!ok) { return 0x1939; }
            out.m_field324[2] = out.m_field324[1] + ScanInt(&ok);
            if (!ok) { return 0x193c; }
        }
        else if (token == "[reward gold weight]")
        {
            out.m_field330 = ScanFloat(&ok);        // ORIG fstps 0x330(%eax) 直写
            if (!ok) { return 0x1942; }
        }
        else if (token == "[reward exp inout count weight]")
        {
            for (;;)
            {
                float v = ScanFloat(&ok);
                if (!ok) { break; }
                out.m_vec334.push_back(v);
            }
        }
        else if (token == "[ultimate difficulty point]")
        {
            out.m_ultDiffPoint344 = ScanInt((bool*)0);
            out.m_ultPointTotal348 = ScanInt((bool*)0);
        }
        else if (token == "[ultimate reward prob]")
        {
            for (;;)
            {
                int key = ScanInt(&ok);
                if (!ok) { break; }
                STUltimateRewardProb prob;
                prob.m_field0 = ScanInt((bool*)0);
                prob.m_field4 = ScanInt((bool*)0);

                std::map<int, STUltimateRewardProb>::iterator it =
                    out.m_map358.find(key);
                if (it == out.m_map358.end())
                {
                    out.m_map358.insert(std::make_pair(key, prob));
                }
                else
                {
                    return 0x196e;                  // ORIG key 重复即失败
                }
            }
        }
    }
