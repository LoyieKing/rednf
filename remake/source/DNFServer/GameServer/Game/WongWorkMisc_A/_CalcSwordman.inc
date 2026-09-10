_Additioal_info* CCalcCharacStat::_CalcSwordman(_Additioal_info* out,
                                                int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x20d + 0x5dcu;  // (sVar1-1)*0x1e 写回 u16
        out->m_field04 = (level - 1) * 0xaf + 0x5dcu;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x19 + 0x32);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_hitRecovery = static_cast<short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 400 + 0xc99u;
        out->m_field04 = level * 300 - 0x39du;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x46);
        out->m_field0a = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field0e = static_cast<unsigned short>((level + 1) * 0x19);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2080);
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    case 2:
        out->m_field00 = level * 0x15e + 0x101du;
        out->m_field04 = level * 0x15e - 0x721u;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 200);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 200);
        out->m_field0c = static_cast<unsigned short>((level * 4 + level) * 8 - 0x41);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 - 0xf5);
        // [2026-09-10 D06] P321-T1：ORIG 0x0831f? `shl $0x3,%eax; mov %eax,%edx`
        // ⇒ +0x14 源视角为 **signed short**（探针 /tmp/d06/p14.cpp g3：signed
        // short 字段产该形，unsigned short 恒产 `lea 0x0(,%eax,8),%edx`）。
        out->m_field10s[2] = static_cast<short>((level + 7) << 3);  // +0x14
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 - 0xd2);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x19f0);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x1810);
        break;
    case 3:
        out->m_field00 = level * 0x226 + 0x20du;
        out->m_field04 = level * 0x96 + 0x6efu;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 8 - 0xa0);
        out->m_field0a = static_cast<unsigned short>(level * 0x2d - 0xfa);
        out->m_field0c = static_cast<unsigned short>(level * 16 - level + 0x181);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 4 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_field42 = level * 10 + 0x2080u;
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 8 + 0x15f4);
        break;
    case 4:
        out->m_field00 = level * 0x20d + 0x3cfu;
        out->m_field04 = level * 0xaf + 0x52du;
        out->m_field08 = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0a = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field0e = static_cast<unsigned short>(level * 32 - level * 2 - 0x41);
        out->m_field3a = (level - 1) * 3000 + 400000;
        out->m_mpRegen = static_cast<short>(level * 4 + level + 0x3c);
        out->m_field48 = static_cast<unsigned short>(level * 10 + 0x1aa4);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 4 + 0x1608);
        break;
    default:
        break;
    }
    return out;
