// ============================================================================
// importFairPVPEquipment — ORIG 0x088ffbfc（init.md +0x2328，
// "Etc/FairPVPEquipment.etc" → m_stackableMap）。
// "[group]"：ScanStr 职业→getCharacterJob；ScanInt((bool*)0) 段位；
// key=makeFairPVPEquipmentPackageKey(job,grade)。随后收集若干
// "[package selection]" 包（成对 itemId/count 直至 ScanInt 失败），整组赋给
// (*out)[key]。[/fair pvp package] 或流结束均返回 true。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x088ffbfc
//   P164 外层/内层 `for(;;)+break` 双改 + itemId 去预清（槽 -0x2c 对齐）；
//   p167 95/95 CLEAR。
bool importFairPVPEquipment(char* path, std::map<int, std::vector<std::vector<STStackablePackageData> > >* out)
{
    if (out == NULL)
    {
        return false;
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string jobName;

    // [b28d-solo-InitScriptProducers_7] ORIG 0x088ffbfc：循环头 ScanType 检查
    //（P164 `for(;;){ if(!ScanType) break; }`），内层 items 循环亦 `for(;;)+break`
    //（ORIG `call ScanInt; xor$1; test; jne 退出`）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[group]")
        {
            if (!ScanStr(&jobName))
            {
                return false;
            }
            const int job = getCharacterJob(jobName.c_str());
            const int grade = ScanInt((bool*)NULL);
            const int key = makeFairPVPEquipmentPackageKey(job, grade);

            std::vector<std::vector<STStackablePackageData> > groups;
            ScanType(token, true);
            while (token == "[package selection]")
            {
                int itemId;                      // ORIG -0x2c（无预清 store）
                std::vector<STStackablePackageData> pack;
                for (;;)
                {
                    if (!ScanInt(&itemId))
                        break;
                    STStackablePackageData data;
                    data.m_field0 = itemId;
                    data.m_field4 = ScanInt((bool*)NULL);
                    pack.push_back(data);
                }
                groups.push_back(pack);
                ScanType(token, true);
            }
            (*out)[key] = groups;
        }
        else if (token == "[/fair pvp package]")
        {
            break;
        }
    }
    return true;
}