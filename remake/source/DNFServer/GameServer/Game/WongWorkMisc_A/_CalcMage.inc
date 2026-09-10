_Additioal_info* CCalcCharacStat::_CalcMage(_Additioal_info* out,
                                            int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x145 + 1000u;
        out->m_field04 = (level - 1) * 0x177 + 2000u;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x14 + 0x1e);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x14 + 0x1e);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x28 + 0x50);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x28 + 0x46);
        out->m_field3a = (level - 1) * 0x9c4 + 280000;
        // [2026-09-10 D06] P321-T1/P196 类型直证：ORIG 0x0831fd1f `add $0xc8,%ax`
        // / 0x0831fd3d `add $0x1388,%ax`（16 位窄化 add）⇒ 源类型为 **signed
        // short**（探针 /tmp/d06/p16.cpp f0/f14/f16：unsigned short 恒产
        // `lea 0xc8(%eax),%edx`，signed short 才产 `add $imm,%ax; mov %eax,%edx`）。
        // 用 m_mpRegen/m_hitRecovery（CCharacter.h 同址 signed 视角，+0x40/+0x4a）。
        out->m_mpRegen = static_cast<short>((level - 1) * 10 + 200);
        out->m_hitRecovery = static_cast<short>((level - 1) * 10 + 5000);
        break;
    case 1:
        out->m_field00 = level * 0x113 + 0x627u;
        out->m_field04 = level * 0x1a9 + 0x2d5u;
        out->m_field08 = static_cast<unsigned short>(level * 16 - level + 100);
        out->m_field0a = static_cast<unsigned short>(level * 16 - level + 100);
        out->m_field0c = static_cast<unsigned short>(level * 0x2d - 0x32);
        out->m_field0e = static_cast<unsigned short>(level * 0x2d - 0x3c);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x25a8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x137e);
        break;
    case 2:
        out->m_field00 = level * 300 + 0x465u;
        out->m_field04 = level * 400 + 0x497u;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 10);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 10);
        out->m_field0c = static_cast<unsigned short>((level + 1) * 0x28);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 + 0x1e);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x25a8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x137e);
        break;
    case 3:
        out->m_field00 = level * 400 - 0x2a3u;
        out->m_field04 = level * 300 + 0xb9fu;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x104);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x82);
        out->m_field0e = static_cast<unsigned short>(level * 0x19 + 300);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x265c);
        out->m_field42 = level * 10 + 0x265cu;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x1216);
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 2 + 0xbe);
        break;
    case 4:
        out->m_field00 = level * 0x15e + 0xe1u;
        out->m_field04 = level * 0x15e + 0x81bu;
        out->m_field08 = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x50);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x82);
        out->m_field0e = static_cast<unsigned short>(level * 0x23 + 0x78);
        out->m_field3a = level * 0x9c4 + 0x43bfc;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 + 10);
        out->m_field48 = static_cast<unsigned short>(level * 10 + 0x265c);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 4 + 0x12ca);
        break;
    default:
        break;
    }
    return out;
