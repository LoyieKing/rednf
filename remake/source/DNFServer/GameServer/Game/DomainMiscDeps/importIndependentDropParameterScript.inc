bool IndependentDropParameterScript::importIndependentDropParameterScript(
    const char* path)
{
    if (!loadRDARScriptFile(g_independentDropScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    IndependentDropListInfo info;
    info.m_field0 =
        g_independentDropScriptFileList.FindIndexByFullScan(path);
    int acc = 0;
    // P212/P309：ORIG 槽 itemId=-0x3c / rate=-0x40（声明序在 acc 之后）⇒ 头部声明。
    int rate;
    int itemId;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        // P222/P59 正向包裹：ORIG 0x0897c25f `test %al; je +0x65`（假→循环头，
        // 真直落内层）；`!=`+continue 反产 `je 内层; jmp 循环头`。
        if (token == "[list]")
        {
            while (true)
            {
                itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                rate = ScanInt(&ok);
                if (!ok)
                {
                    return false;                  // 报告 0x0897c296 路径
                }
                // ORIG 0x0897c2a0：`mov acc,%edx; mov rate,%eax; lea (%edx,%eax,1)`
                // ⇒ 两操作数均先载寄存器（unsigned 加法形；`acc += rate` 产
                // `mov acc,%eax; add rate,%eax` 少 1 条）。
                acc = (int)((unsigned int)acc + (unsigned int)rate);
                info.m_list04.push_back(std::pair<int, int>(itemId, acc));
            }
        }
    }
    // ORIG 0x0897c2e8：count 槽 -0xc 直传地址（make_pair<unsigned int,int&> 第一参
    // `unsigned int&&` ⇒ 无转换临时；P234 `static_cast<T&&>` 零 call）。
    unsigned int count = m_vec0.size();
    m_map24[info.m_field0] =
        std::make_pair<unsigned int, int&>(
            static_cast<unsigned int&&>(count), acc);
    m_vec0.push_back(info);
    return true;
