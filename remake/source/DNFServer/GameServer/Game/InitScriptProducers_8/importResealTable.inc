// ============================================================================
// importResealTable —— ORIG 0x089008b8
// 报告 docs/class_func_reports/_lt_global_gt_/importResealTable.md
// [rarity weight]→m_arr1[0..5](+0xc)；[extended rarity weight]→m_arr2[0..5]
// (+0x24)；[grade]→m_vec push_back 直至 Scan 失败。Scan 失败仍写入后仅记日志。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 089008b8：clean 162/162（fresh .o 口径，
// ORIG 165 insns）；p167 CLEAR。已对齐：死 string×2（P171）、bool ok 独立槽
// （-0x2d/-0x2e/-0x2f）、三段 ScanInt(bool*) 循环、push_back(unsigned int)。
bool importResealTable(const char* path, reseal_table_t* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    // ORIG 0x89008ef/0x89008fa：另两个 string 局部（构造+析构、全程未使用；
    // P171 死对象照抄，4.4 亦保留其 C1/D1 调用）。
    std::string deadToken1;
    std::string deadToken2;
    (void)deadToken1;
    (void)deadToken2;

    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[rarity weight]")                       // 0x8e122be 区字符串
        {
            bool ok;                                          // ORIG -0x2d
            int i;
            int value;
            for (i = 0; i < 6; ++i)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xa9d, "importResealTable - invalid rarity table");
                }
                out->m_arr1[i] = value;                       // ORIG 写 param_2+0xc+i*4
            }
        }
        else if (token == "[extended rarity weight]")
        {
            bool ok;                                          // ORIG -0x2e
            int i;
            int value;
            for (i = 0; i < 6; ++i)
            {
                value = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xaaf,
                        "importResealTable - invalid extended rarity table");
                }
                out->m_arr2[i] = value;                       // ORIG 写 param_2+0x24+i*4
            }
        }
        else if (token == "[grade]")
        {
            bool ok;                                          // ORIG -0x2f
            int value;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec.push_back(static_cast<unsigned int>(value));
            }
        }
    }
    return true;
}