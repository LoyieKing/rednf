bool importGenuineUpgradeTable(const char* path,
                              GenuineDamageUpgradeTable* table)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string deadToken1;      // ORIG -0xb0（仅 ctor/dtor，P171 照抄）
    std::string deadToken2;      // ORIG -0xb4
    int level = 0;

    for (;;)
    {
    sectionHead:                 // ORIG 7 处 continue 均为 jmp 0x089123a2（直跳循环头）；
                                 // 4.4 对 continue 恒发共享 latch 块（jmp 尾部），
                                 // goto 头标签才复现直跳形（探针 /tmp/sdb_probe/p5.o fGoto）。
        if (!ScanType(token, true))
            break;

        if (token == "[table]")
        {
            int key = 1;
            table->m_infos.clear();
            for (;;)
            {
                GenuineDamageUpgradeInfo info;
                if (!ScanInt(&info.m_a))
                    break;
                if (!ScanInt(&info.m_b))
                    break;
                if (!ScanInt(&level))
                    break;
                info.m_levelRate =
                    static_cast<float>(level) / 10000.0f;
                if (!ScanFloat(&info.m_goldRate))
                    break;
                table->m_infos.insert(
                    std::make_pair(key++, info));   // 后置 ++ ⇒ make_pair<int,T&>
            }
            goto sectionHead;
        }

        if (token == "[separate upgrade max]")
        {
            if (!ScanInt(&table->m_maxLevel))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2f9e,
                    "importGenuineUpgradeTable() - [separate_upgrade max] false");
            }
            goto sectionHead;
        }

        if (token == "[level]")
        {
            if (!ScanInt(&table->m_divisor))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2fa5,
                    "importGenuineUpgradeTable() - [level] false");
            }
            goto sectionHead;
        }

        if (token == "[item weights by grade]")
        {
            for (;;)
            {
                int key = 0;
                GenuineDamageUpgradeMaterialInfo material;
                if (!ScanInt(&key))
                    break;
                if (!ScanInt(&material.m_materialItemIdx))
                    break;
                if (!ScanInt(&material.m_materialCount))
                    break;
                table->m_materials.insert(
                    std::make_pair(key, material));
            }
            goto sectionHead;
        }

        if (token == "[item weights by rarity]")
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&table->m_itemWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fbe,
                        "importGenuineUpgradeTable() - [item weights by rarity] false");
                }
            }
            goto sectionHead;
        }

        if (token == "[stat weights by rarity]")
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&table->m_statWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fc9,
                        "importGenuineUpgradeTable() - [stat weights by rarity] false");
                }
            }
            goto sectionHead;
        }

        if (token == "[weapon weights]")
        {
            for (int a = 0; a <= 10; ++a)
            {
                for (int b = 0; b <= 5; ++b)
                {
                    if (!ScanFloat(&table->m_weaponWeights[a][b]))
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp",
                            __FUNCTION__, 0x2fd5,
                            "importGenuineUpgradeTable() - [weapon weights] false");
                    }
                }
            }
            goto sectionHead;
        }

        if (token == "[separate upgrade effect]")
        {
            int key = 1;
            for (;;)
            {
                OutLineEFFECTInfoTemp effect;
                int r = 0;
                int g = 0;
                int b = 0;
                if (!ScanInt(&r))
                    break;
                if (!ScanInt(&g))
                    break;
                if (!ScanInt(&b))
                    break;
                // ORIG 0x08912863..0x0891289a：(unsigned char)r | ((unsigned char)g
                //   << 8) | ((unsigned char)b << 16) | 0xff000000 → +0x04
                //   （movzbl 逐字节扩展；`& 0xffu` 形会退化成 and，见 /tmp/sdb_probe/p3.o）
                effect.m_b = static_cast<int>(
                    static_cast<unsigned>(static_cast<unsigned char>(r)) |
                    (static_cast<unsigned>(static_cast<unsigned char>(g)) << 8) |
                    (static_cast<unsigned>(static_cast<unsigned char>(b)) << 16) |
                    0xff000000u);
                if (!ScanInt(&effect.m_d))              // +0x0c
                    break;
                if (!ScanInt(&effect.m_c))              // +0x08
                    break;
                if (!ScanInt(&effect.m_a))              // +0x00
                    break;
                table->m_effects.insert(
                    std::make_pair(key++, effect));     // 后置 ++ ⇒ make_pair<int,T&>
            }
            // ORIG 0x0891285d `je <continue 岛>` + 内层循环尾 0x089129c4 `jmp 退出块`
            // ⇒ 本分支处理完即退出外层令牌循环（真 bug 修正：原写 continue 会继续
            //  扫描后续令牌，与 ORIG 行为不同）。
            break;
        }
    }

    return true;
