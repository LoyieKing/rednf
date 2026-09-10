void CUserPremium::RecalcAdditionalInfo(CUser const* user)
{
    if (user->getCurCharacR() == 0 || GetAdvPremiumCount() == 0)
        return;

    // 残差豁免（O=316/N=325，+9：乘除段个别操作数 4.4 双段物化/单段折叠
    // 位差，Main 裁决标注不追）。
    // ORIG（0x086ae8c6）：读侧一律 this+0x8bd（= m_additionalBase），写侧一律
    // this+0x90f（= m_additionalInfo）——asm 实证 `mov 0x8bd(%eax),%eax` …
    // `mov %edx,0x90f(%eax)`，Ghidra C 同形。此前把结果写回 +0x8bd 属偏移错配。

    // 0x00/0x04/0x42 的 ORIG 除法使用无符号乘积（mul+shr）；其余 int/short
    // 分支按汇编的有符号乘除（imul+sar）处理。pack(1) 成员直访保留
    // +0x3a/+0x42 等非自然对齐字段的 ABI 宽度。
    // ORIG：getCurCharacAddInfoR 只调用一次并缓存于 ebp-0x10，各语句按
    // `mov -0x10(%ebp),%eax` + disp 重读（故此处用单局部指针）。
    const _Additioal_info* addInfo =
        static_cast<const _Additioal_info*>(user->getCurCharacAddInfoR());
    // probe p16/m1：4.4 乘法 L2R 求值——ORIG 先 *(addInfo 槽) 后 m_8bd ⇒ 左操作数
    // 必为 addInfo->field（原写 m_additionalBase 在左=先读 m_8bd，反序）。
    // [b28d-solo-CUserPremium] P235/P211：ORIG 读侧为单段 `mov (%eax),%edx`
    // （无 eax→edx 中转）⇒ 源用 unsigned int 同型视图 m_maxHP/m_maxMP/
    // m_moveSpeed 直读；`static_cast<unsigned int>(int 视图)` 使 4.4 多发
    // `mov %eax,%edx` 两段（3 处 ×7 条 = ndiff 21）。
    m_additionalInfo.m_maxHP =
        addInfo->m_maxHP * m_additionalBase.m_maxHP / 100U;
    m_additionalInfo.m_maxMP =
        addInfo->m_maxMP * m_additionalBase.m_maxMP / 100U;
    m_additionalInfo.m_field08 = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field08) *
         static_cast<int>(m_additionalBase.m_field08)) / 100);
    m_additionalInfo.m_field0a = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field0a) *
         static_cast<int>(m_additionalBase.m_field0a)) / 100);
    m_additionalInfo.m_field0c = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field0c) *
         static_cast<int>(m_additionalBase.m_field0c)) / 100);
    m_additionalInfo.m_field0e = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field0e) *
         static_cast<int>(m_additionalBase.m_field0e)) / 100);

    for (int i = 0; i < 4; ++i)
    {
        m_additionalInfo.m_field10[i] = static_cast<unsigned short>(
            (static_cast<short>(addInfo->m_field10[i]) *
             static_cast<short>(m_additionalBase.m_field10[i])) / 100);
    }

    m_additionalInfo.m_field3a = (addInfo->m_field3a * m_additionalBase.m_field3a) / 100;
    m_additionalInfo.m_field3e = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field3e) *
         static_cast<short>(m_additionalBase.m_field3e)) / 100);
    m_additionalInfo.m_field40 = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field40) *
         static_cast<short>(m_additionalBase.m_field40)) / 100);
    m_additionalInfo.m_moveSpeed =
        addInfo->m_moveSpeed * m_additionalBase.m_moveSpeed / 100U;
    m_additionalInfo.m_field46 = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field46) *
         static_cast<int>(m_additionalBase.m_field46)) / 100);
    m_additionalInfo.m_field48 = static_cast<unsigned short>(
        (static_cast<int>(addInfo->m_field48) *
         static_cast<int>(m_additionalBase.m_field48)) / 100);
    m_additionalInfo.m_field4a = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field4a) *
         static_cast<short>(m_additionalBase.m_field4a)) / 100);
    m_additionalInfo.m_field4c = static_cast<unsigned short>(
        (static_cast<short>(addInfo->m_field4c) *
         static_cast<short>(m_additionalBase.m_field4c)) / 100);
