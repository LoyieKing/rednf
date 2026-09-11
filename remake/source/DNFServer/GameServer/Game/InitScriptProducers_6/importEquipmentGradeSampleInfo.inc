// ============================================================================
// importEquipmentGradeSampleInfo —— ORIG T 0x0890e18d
//   bool (const char*, stEquipmentGradeSample*)
// [grade_0]..[grade_4] 五节分别向五个 vector<unsigned int> push（push_backEOj
// 右值重载，值序一致）；out 判空在 load 之后（ORIG 0x890e1d3）。
// ============================================================================
// b28d-solo-InitScriptProducers_6: AE（190=190）— ORIG 0x0890e18d。load 检查最前，
// 随后 token(-0x34)/unused(-0x38，零引用)/ok(-0x39) 声明，再 out 判空；五节内层
// `for(;;){ v=ScanInt(&ok); if(!ok)break; push_back((unsigned int)v); }`
// （ORIG `xor $1; test; je body` 形；push_back 取右值重载 EOj，p167 CLEAR）。
bool importEquipmentGradeSampleInfo(const char* path,
                                    stEquipmentGradeSample* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x34
    std::string unused;                 // ORIG -0x38（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x39

    if (out == 0)
    {
        return false;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[grade_0]")
        {
            for (;;)
            {
                int v0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[0].push_back((unsigned int)v0);
            }
        }
        else if (token == "[grade_1]")
        {
            for (;;)
            {
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[1].push_back((unsigned int)v1);
            }
        }
        else if (token == "[grade_2]")
        {
            for (;;)
            {
                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[2].push_back((unsigned int)v2);
            }
        }
        else if (token == "[grade_3]")
        {
            for (;;)
            {
                int v3 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[3].push_back((unsigned int)v3);
            }
        }
        else if (token == "[grade_4]")
        {
            for (;;)
            {
                int v4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                out->m_grade[4].push_back((unsigned int)v4);
            }
        }
    }
    (void)unused;
    return true;
}