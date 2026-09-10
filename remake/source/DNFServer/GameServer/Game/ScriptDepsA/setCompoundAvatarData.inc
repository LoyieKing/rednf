bool setCompoundAvatarData(tagCompoundAvatar* avatar, ENUM_EQUIPMENTTYPE equip)
{
    // [b28d-solo-ScriptDepsA 判词] AE（p167 16/16 CLEAR）：修 (1) m_b/m_c/m_d
    // 字段 unsigned 化（CCompoundAvatar.h，ORIG pair converting ctor `C1IRiRj`
    // 实证）；(2) 三处 make_pair→pair 直构（P192）+ item/weight 去 const
    // （`IRKiRj` 是错误变体）；(3) 删 idx 具名槽（P0，每次重读 0xc(%ebp)）、
    // ok 去预清、count 改 int（循环尾 setl）。
    bool ok;

    const int countA = ScanInt(&ok);
    if (!ok)
        return false;
    avatar->m_optsA[static_cast<int>(equip)].reserve(countA);
    for (int i = 0; i < countA; ++i)
    {
        // [b28d-solo-ScriptDepsA] ORIG pair ctor `C1IRiRj` 第一参非 const
        //（IRKiRj 是 const int& 变体，符号不符）⇒ item/weight 去 const。
        int item = ScanInt(&ok);
        if (!ok)
            return false;
        int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_b[static_cast<int>(equip)] += weight;
        // [b28d-solo-ScriptDepsA] P192：ORIG 0x08902d84 直构 pair（converting
        // ctor `C1IRiRj`，第二参 m_b[idx] 为 unsigned 左值），非 make_pair。
        avatar->m_optsA[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item, avatar->m_b[static_cast<int>(equip)]));
    }

    const int countC = ScanInt(&ok);
    if (!ok)
        return false;
    avatar->m_optsC[static_cast<int>(equip)].reserve(countC);
    for (int i = 0; i < countC; ++i)
    {
        int item = ScanInt(&ok);
        if (!ok)
            return false;
        int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_d[static_cast<int>(equip)] += weight;
        avatar->m_optsC[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item, avatar->m_d[static_cast<int>(equip)]));
    }

    while (true)
    {
        int item = ScanInt(&ok);
        if (!ok)
            break;
        int weight = ScanInt(&ok);
        if (!ok)
            return false;
        avatar->m_c[static_cast<int>(equip)] += weight;
        avatar->m_optsB[static_cast<int>(equip)].push_back(
            std::pair<int, int>(item, avatar->m_c[static_cast<int>(equip)]));
    }
    return true;
