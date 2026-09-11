// ===========================================================================
// importNoviceCommandScript —— ORIG T 0x0890402a
// （docs/class_func_reports/_lt_global_gt_/importNoviceCommandScript.md）
// 先清 0xb 个 vector<vector<int>> 槽；按职业 token 逐轮 push_back：
// gunner→+0x18(10 轮)/fighter→+0x0c(8)/swordman→+0x00(8)/
// at fighter→+0x54(8)/thief→+0x48(6)/mage→+0x24(10)/
// at gunner→+0x3c(10)/priest→+0x30(10)，每轮内层 10 个 ScanInt。
// loadRDARScriptFile 失败返 0x11cc，成功返 1。
// 重建形 = ORIG 物理内联展开（P165 we-smaller 复原：8 个独立职业块、
// 各自 j/k 计数器，勿用 dst/rounds 参数化共享内层循环）。
// ===========================================================================
int importNoviceCommandScript(const char* path, stNoviceCommand& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x11cc;
    std::string token;
    for (int i = 0; i <= 0xa; ++i)   // ORIG cmpl $0xa/setle（P2）
        out.m_slots[i].clear();
    std::vector<int> row;
    int val;                          // ORIG 单槽 -0x60：各块共用
    for (;;)
    {
        if (!ScanType(token, true))
            break;                    // ORIG 出口岛在函数尾（共享 leave）
        if (token == "[gunner]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_GUNNER].push_back(row);
            }
        }
        else if (token == "[fighter]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_FIGHTER].push_back(row);
            }
        }
        else if (token == "[swordman]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_SWORDMAN].push_back(row);
            }
        }
        else if (token == "[at fighter]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_ATFIGHTER].push_back(row);
            }
        }
        else if (token == "[thief]")
        {
            for (int j = 0; j <= 0x5; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_THIEF].push_back(row);
            }
        }
        else if (token == "[mage]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_MAGE].push_back(row);
            }
        }
        else if (token == "[at gunner]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_ATGUNNER].push_back(row);
            }
        }
        else if (token == "[priest]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_PRIEST].push_back(row);
            }
        }
    }
    return 1;
}