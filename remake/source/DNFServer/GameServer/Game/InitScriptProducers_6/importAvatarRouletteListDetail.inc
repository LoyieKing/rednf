bool importAvatarRouletteListDetail(AvatarRoulette& out,
                                    std::string& token,
                                    ENUM_CHARACTERJOB job)
{
    std::vector<std::pair<unsigned long, int> > pairs;
    int value = 0;                      // ORIG -0x28
    bool isType = false;
    bool isInt = false;

    for (int i = 0; i <= 8; ++i)
    {
        value = ScanTypeOrInt(isType, token, isInt, true);
        if (isType)
        {
            break;   // TYPE token 已留在 token 中供上层判节尾
        }
        // ORIG：第二实参是右值字面量（mangled `C1IRiiEEOT_OT0_`，
        // p167 实证 pair<int&,int&&>），编译器为其分配临时槽 -0x10。
        pairs.push_back(std::pair<unsigned long, int>(value, 3));
    }

    if (!pairs.empty())
    {
        out.m_avatars[job].push_back(pairs);   // &out[job*0xc+0x14]（+0x14 数组基址实证）
        value = ScanInt((bool*)0);
        out.m_stikers[job].push_back(value);   // &out[job*0xc+0x98]
    }
    return true;
