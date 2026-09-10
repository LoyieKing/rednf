bool importActiveStatusTable(const char* path)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N115 封顶（§6.2：237/235，gap=2，
    // p167 31/31 CLEAR）：本波修三处 level-rate 的具名 float 槽往返
    // （fstps→槽→读回→存全局，P252）。残差=帧 0x60 vs 0x50（ORIG 多 3 个
    // dead 槽 -0x30..-0x38）+ 全帧槽号平移 + 4.1/4.4 bool while 归一化方向
    // 相反（ORIG `xor $1;test;jne` vs 4.4 `test;jne`，探针 g1/g2 实证不可翻）
    // + power 段索引地址计算时刻差（P75）。
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(
            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0x162b,
            "ERROR : %s \306\304\300\317\300\273\040\303\243\300\273\274\366\040"
            "\276\370\275\300\264\317\264\331.", path);   // EUC-KR 字面量（ORIG 0x08e103d8）
        return false;
    }

    std::string token;
    for (;;)
    {
        // [P292/P283] ORIG 循环头 `call; xor $0x1,%eax; test %al,%al; jne exit`
        // ⇒ 原码是 for(;;)+`if (!ScanType(...)) break;`（bool 返回的取反形），
        // 非 while(ScanType(...))（后者 4.4 出无 xor 的 `test;jne body`）。
        if (!ScanType(token, true))
            break;
        if (token == "[active status max level]")
        {
            ACTIVESTATUS_MAXLEVEL = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[active status duration]")
        {
            for (int i = 0; i < 18; ++i)
                ACTIVESTATUS_DURATION[i] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[active status power 1]")
        {
            for (int i = 0; i < 18; ++i)
            {
                ACTIVESTATUS_POWER1.m_pair[i].m_lo = ScanFloat(static_cast<bool*>(0));
                ACTIVESTATUS_POWER1.m_flat[i * 2 + 1] = ScanFloat(static_cast<bool*>(0));
            }
        }
        else if (token == "[active status power 2]")
        {
            for (int i = 0; i < 18; ++i)
            {
                ACTIVESTATUS_POWER2.m_pair[i].m_lo = ScanFloat(static_cast<bool*>(0));
                ACTIVESTATUS_POWER2.m_flat[i * 2 + 1] = ScanFloat(static_cast<bool*>(0));
            }
        }
        else if (token == "[active status etc parameters]")
        {
            for (int i = 0; i < 18; ++i)
                for (int j = 0; j < 8; ++j)
                    ACTIVESTATUS_ETC_PARAMETERS[i][j] = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[active status level rate for prob]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            // [b28d-solo-ScriptDepsA] ORIG 0x089071dc 段：fstps 先落具名 float
            // 槽(-0x14) 再读回存全局（P252 槽往返形）。
            const float rate = 0.01f * static_cast<float>(v);
            ACTIVESTATUS_LEVEL_RATE_FOR_PROB = rate;
        }
        else if (token == "[active status level rate for power]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            const float rate = 0.01f * static_cast<float>(v);
            ACTIVESTATUS_LEVEL_RATE_FOR_POWER = rate;
        }
        else if (token == "[active status level absolute rate]")
        {
            const int v = ScanInt(static_cast<bool*>(0));
            const float rate = 0.01f * static_cast<float>(v);
            ACTIVESTATUS_LEVEL_ABSOLUTE_RATE = rate;
        }
        else if (token == "[lightning time per number]")
        {
            LIGHTNING_TIME_PER_NUMBER = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[active status user to user damage rate]")
        {
            ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[dead tower active status user to user damage rate]")
        {
            ACTIVESTATUS_USER_TO_USER_DAMAGE_RATE_DEATH_TOWER =
                ScanFloat(static_cast<bool*>(0));
        }
    }
    return true;
