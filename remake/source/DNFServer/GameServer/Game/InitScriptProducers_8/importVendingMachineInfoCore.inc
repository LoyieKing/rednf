int importVendingMachineInfoCore(const char* dir, const char* file,
                                 stVendingMachine_t& vm)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1e0b;

    std::string token;
    bool ok = false;

    for (;;)
    {
    scan_again:
        // ---- 组外：未知标签记日志，直到 [item group] 或 EOF ----
        for (;;)
        {
            if (!ScanType(token, true))
                goto eof_exit;                     // ORIG jne 尾岛（890b714）
            if (token == "[item group]")
                break;
            goto unknown_tag;                      // ORIG 冷块落函数尾
        }

        stVendingMachineInfo_t info;                   // ctor 后自 clear

        for (;;)
        {
            if (!ScanType(token, true))
                break;                                 // EOF：析构后外层再扫失败→0
            if (token == "[/item group]")
            {
                vm.m_infoList.push_back(info);
                info.clear();
                break;
            }
            if (token == "[group num]")
            {
                info.m_groupNum = ScanInt(&ok);
                if (ok != true)
                    return 0x1e27;
                continue;
            }
            if (token == "[material]")
            {
                info.m_payType = ScanInt(&ok);
                if (ok != true)
                    return 0x1e2d;
                info.m_payAmount = ScanInt(&ok);
                if (ok != true)
                    return 0x1e30;
                continue;
            }
            // P165：ORIG 0x890b444/0x890b572 为两份物理独立块（各 4 次
            // ScanInt(bool*)，槽集不同）⇒ 源内展开两份，勿用 bonus 标志参数化。
            if (token == kVendOutputTag)
            {
                stVendingMachineOutput_t entry;
                for (;;)
                {
                    entry.m_itemId = ScanInt(&ok);
                    if (!ok)
                        break;
                    const int rate = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e3d;
                    entry.m_count = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e40;
                    if (ScanInt(&ok) == 1)
                        info.m_lottery.push_back(
                            std::make_pair(
                                reinterpret_cast<unsigned long&>(
                                    entry.m_itemId),
                                entry.m_count));
                    if (ok != true)
                        return 0x1e44;
                    info.m_giveCount += rate;
                    entry.m_cumWeight = info.m_giveCount;
                    info.m_vecGive.push_back(entry);
                }
                continue;
            }
            if (token == kVendBonusTag)
            {
                stVendingMachineOutput_t entry;
                for (;;)
                {
                    entry.m_itemId = ScanInt(&ok);
                    if (!ok)
                        break;
                    const int rate = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e55;
                    entry.m_count = ScanInt(&ok);
                    if (ok != true)
                        return 0x1e58;
                    if (ScanInt(&ok) == 1)
                        info.m_lottery.push_back(
                            std::make_pair(
                                reinterpret_cast<unsigned long&>(
                                    entry.m_itemId),
                                entry.m_count));
                    if (ok != true)
                        return 0x1e5c;
                    info.m_bonusCount += rate;
                    entry.m_cumWeight = info.m_bonusCount;
                    info.m_vecBonus.push_back(entry);
                }
                continue;
            }
            // 组内其他标签：静默继续（ORIG 无日志路径）
        }
    }
unknown_tag:
    LogManager::logFormat(
        0, "../RDARScriptEtcParameter.cpp",
        __FUNCTION__, 0x1e66,
        "ERROR : Unknown tag error. %s", token.c_str());
    goto scan_again;
eof_exit:
    return 0;
