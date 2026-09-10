int advancealtar::RewardParameter::importAchievementRewardParameter(
    const char* path)
{
    std::string token;
    int unkC = 0;       // ORIG 0x0889afc6 `movl $0x0,-0xc(%ebp)`：token 之后的 dead int（P232/P31）
    (void)unkC;
    bool ok = false;
    reset();
    if (!loadRDARScriptFile("", path))
        return 0;

    while (ScanType(token, true))
    {
        // p167 MISSING operator==×1 / EXTRA operator!=×1 ⇒ ORIG 为正向 `==` 包裹。
        if (token == "[reward data]")
        {
            AchievementRewardData data;
            while (ScanType(token, true))
            {
                if (token == "[/reward data]")
                {
                    m_rewardMap.insert(std::make_pair(data.m_type, data));
                    break;
                }
                if (token == "[type]")
                {
                    data.m_type = ScanInt(&ok);
                    if (!ok)
                        return 0x362;
                    if (!(data.m_type > 0))
                        return 0x364;
                    continue;
                }
                if (token == "[stage difficulty]")
                {
                    std::string diffName;
                    if (!ScanStr(&diffName))
                        return 0x36a;
                    // ORIG 0x0889b1d5：`lea -0xb4(%ebp),%eax; add $0x4,%eax`
                    // ⇒ 出参直接是 &data.m_difficulty（data+4），无具名 T 局部。
                    if (!StageDifficulty::getDifficulty(
                            diffName,
                            reinterpret_cast<StageDifficulty::T&>(
                                data.m_difficulty)))
                        return 0x36d;
                    continue;
                }
                if (token == "[stage clear count]")
                {
                    data.m_stageClearCount = ScanInt(&ok);
                    if (!ok)
                        return 0x373;
                    continue;
                }
                if (token == "[icon image]")
                {
                    data.m_iconImage = ScanInt(&ok);
                    if (!ok)
                        return 0x37a;
                    continue;
                }
                if (token == "[reward item]")
                {
                    for (;;)
                    {
                        _RewardItem item;
                        std::string typeName;
                        if (!ScanStr(&typeName))
                            break;                       // 静默退出本段
                        // ORIG 0x0889b327：`lea -0x84(%ebp),%eax` 直接传 &item（+0）⇒
                        // 出参直写 m_type，无具名 itemType 中转。
                        // ORIG 0x0889b339 `mov %al,-0x89(%ebp)` + 0x0889b368/0x0889b38e
                        // `movzbl -0x89(%ebp)`：本段用**独立 bool**（-0x89）接收返回并
                        // 复用为两次 ScanInt 的 ok 出参，非外层 ok(-0x71)。
                        bool itemOk = RewardItemType::getRewardItemType(
                            typeName,
                            reinterpret_cast<RewardItemType::T&>(item.m_type));
                        if (!itemOk)
                            break;
                        // [b28d-solo-CDataManagerScripts-2] 真 bug 修正：ORIG 0x0889b360
                        // 第一次 ScanInt 写 item+4（m_value1），0x0889b386 第二次写
                        // item+8（m_value2）——旧实现把第一次写进 m_value2、第二次丢弃，
                        // 导致 m_value1 恒 0 且 m_value2 取错值。
                        item.m_value1 = ScanInt(&itemOk);    // +4
                        if (!itemOk)
                            break;
                        item.m_value2 = ScanInt(&itemOk);    // +8
                        if (!itemOk)
                            return 0x391;
                        data.m_rewardItems.push_back(item);
                    }
                    continue;
                }
                if (token == "[reward title]")
                {
                    if (!ScanStr(&data.m_rewardTitle))
                        return 0x399;
                    continue;
                }
                if (token == "[postal title]")
                {
                    if (!ScanStr(&data.m_postalTitle))
                        return 0x39e;
                    continue;
                }
                if (token == "[postal letter]")
                {
                    if (!ScanStr(&data.m_postalLetter))
                        return 0x3a3;
                    continue;
                }
            }
        }
    }
    return 0;
