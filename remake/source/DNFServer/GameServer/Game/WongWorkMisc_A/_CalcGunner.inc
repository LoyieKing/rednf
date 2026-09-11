// ORIG T 0x0831f75c _ZN8WongWork15CCalcCharacStat11_CalcGunnerEP15_Additioal_infoii
// 报告：_CalcGunner.md。
// [b28d-54 N] 同 _CalcMage（短整扩展/lea 折叠，N1/N97）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：同 _CalcSwordman 修法，段级
//   identical 15→33/44；残余 6 段 = 16 位字段 add/sub vs lea（N1/N86b+跨版本窄化）
//   ⇒ 封顶。p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 402=402] 同 _CalcSwordman 修法（signed 视角
//   m_hitRecovery/m_mpRegen），旧"16 位字段 add/sub vs lea ⇒ 封顶"判词作废。
_Additioal_info* CCalcCharacStat::_CalcGunner(_Additioal_info* out,
                                              int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 400 + 0x4e2u;
        out->m_field04 = (level - 1) * 300 + 0x708u;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x19 + 0x28);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x17 + 0x28);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x25 + 0x46);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_hitRecovery = static_cast<short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 0x15e + 0x6d6u;
        out->m_field04 = level * 0x15e + 600u;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0xa5);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x13);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x9f);
        out->m_field0e = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2468);
        out->m_field42 = level * 10 + 0x1f54u;
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 8 + 0x15f4);
        break;
    case 2:
        out->m_field00 = level * 0x1a9 + 400u;
        out->m_field04 = level * 0x113 + 0x79eu;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 8 - 0xff);
        out->m_field0a = static_cast<unsigned short>(level * 0x2d - 0x17b);
        out->m_field0c = static_cast<unsigned short>(level * 16 - level + 0x1ad);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 4 + 0x127);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    case 3:
        out->m_field00 = level * 300 + 0xa5au;
        out->m_field04 = level * 400 - 300u;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x69);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 0x47);
        out->m_field0c = static_cast<unsigned short>((level * 4 + level) * 8 - 0x15);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 - 0x41);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 - 0xb9);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x1dd8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x1810);
        break;
    case 4:
        out->m_field00 = level * 400 + 0x352u;
        out->m_field04 = level * 300 + 0x5dcu;
        out->m_field08 = static_cast<unsigned short>(level * 32 - level * 2 - 0x4b);
        out->m_field0a = static_cast<unsigned short>(level * 0x19 - 0x13);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 + 0x45);
        out->m_field0e = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field3a = (level - 1) * 3000 + 330000;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    default:
        break;
    }
    return out;
}