// ============================================================================
// importAvatarRouletteListDetail —— ORIG T 0x089115b1
//   bool (AvatarRoulette&, std::string&, ENUM_CHARACTERJOB)
// 内层装载器：至多 9 次 ScanTypeOrInt 取数值（遇 TYPE 提前终止），累计入局部
// pairs{value,3}；pairs 非空时整体 push 进 m_avatars[job]，再 ScanInt 一枚
// sticker push 进 m_stikers[job]。恒返回 true。
// ============================================================================
// b28d-solo-InitScriptProducers_6: AE（97=97）— ORIG 0x089115b1。value(-0x28，预清 0
// 且 ScanInt 结果复用)、isType(-0x29)/isInt(-0x2a)；pair 直构
// `pair<unsigned long,int>(value, 3)`——第二实参须为**右值字面量**（p167 实证
// ORIG 为 `C1IRiiEEOT_OT0_` = pair<int&,int&&>，具名 three 左值会选错重载）；job 形参直用。
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
}