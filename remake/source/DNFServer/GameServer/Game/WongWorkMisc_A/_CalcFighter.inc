// ORIG T 0x0831f282 _ZN8WongWork15CCalcCharacStat12_CalcFighterEP15_Additioal_infoii
// 报告：_CalcFighter.md。
// [b28d-54 N] 同 _CalcSwordman（短整扩展/lea 折叠，N1/N97）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：同 _CalcSwordman 修法（32 位字段段
//   unsigned 字面量 + `*` 化去 t 槽），段级 identical 17→33/45；残余 7 段 = 16 位
//   字段 add/sub vs lea（N1/N86b+跨版本窄化，探针见 _CalcSwordman 注释）⇒ 封顶。
//   p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 402=402] 同 _CalcSwordman 修法（signed 视角
//   m_mpRegen/m_hitRecovery + m_field10s[2]），旧"16 位字段 add/sub vs lea ⇒ 封顶"
//   判词作废。
_Additioal_info* CCalcCharacStat::_CalcFighter(_Additioal_info* out,
                                               int secondType, int level)
{
    switch (secondType)
    {
    case 0:
        out->m_field00 = (level - 1) * 0x1c2 + 0x708u;
        out->m_field04 = (level - 1) * 0xfa + 0x5dcu;
        out->m_field08 = static_cast<unsigned short>((level - 1) * 0x20 + 0x3c);
        out->m_field0a = static_cast<unsigned short>((level - 1) * 0x23 + 0x3c);
        out->m_field0c = static_cast<unsigned short>((level - 1) * 0x1e + 0x32);
        out->m_field0e = static_cast<unsigned short>((level - 1) * 0x17 + 0x32);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_hitRecovery = static_cast<short>((level - 1) * 0x14 + 6000);
        break;
    case 1:
        out->m_field00 = level * 400 + 0x8cau;
        out->m_field04 = level * 300 + 0x15eu;
        out->m_field08 = static_cast<unsigned short>((level * 4 + level) * 4 + 0xf4);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 4 + 0x127);
        out->m_field0c = static_cast<unsigned short>((level * 4 + level) * 8 - 0xa0);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 8 - 0x117);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_mpRegen = static_cast<short>((level * 4 + level) * 4 - 0xe6);
        out->m_field48 = static_cast<unsigned short>((level * 4 + level) * 4 + 0x25a8);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 2 + 0x1810);
        break;
    case 2:
        out->m_field00 = level * 500 + 0x1c2u;
        out->m_field04 = level * 200 + 0x866u;
        out->m_field08 = static_cast<unsigned short>(level * 0x23 - 0x1a);
        out->m_field0a = static_cast<unsigned short>(level * 32 - level * 2 + 0x73);
        out->m_field0c = static_cast<unsigned short>(level * 0x23 - 0x46);
        out->m_field0e = static_cast<unsigned short>((level * 4 + level) * 4 + 0x51);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_field46 = static_cast<unsigned short>(level * 10 + 0x2468);
        out->m_hitRecovery = static_cast<short>((level * 4 + level) * 8 + 0x15f4);
        break;
    case 3:
        out->m_field00 = level * 600 - 0x546u;
        out->m_field04 = level * 100 + 0xf6eu;
        out->m_field08 = static_cast<unsigned short>(level * 0x26 - 0x50);
        out->m_field0a = static_cast<unsigned short>((level * 4 + level) * 8 - 0x41);
        out->m_field0c = static_cast<unsigned short>(level * 0x1b + 0x4a);
        out->m_field0e = static_cast<unsigned short>(level * 16 - level + 0xab);
        // [2026-09-10 D06] 同上：ORIG `sub $0x134,%ax; mov %eax,%edx` ⇒ signed 视角
        // （探针 p14 g1/g5 命中；g0 unsigned 恒 lea）。
        out->m_field10s[2] = static_cast<short>(level * 6 - 0x134);  // +0x14
        out->m_field3a = level * 3000 + 0x466f4;
        out->m_field42 = level * 10 + 0x22d8u;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    case 4:
        out->m_field00 = level * 0x1c2 + 0x546u;
        out->m_field04 = level * 0xfa + 0x4e2u;
        out->m_field08 = static_cast<unsigned short>((level - 0x12) * 0x20 + 0x25c);
        out->m_field0a = static_cast<unsigned short>(level * 0x23 + 0x19);
        out->m_field0c = static_cast<unsigned short>(level * 32 - level * 2 + 0x14);
        out->m_field0e = static_cast<unsigned short>(level * 0x17 + 0x1b);
        out->m_field3a = (level - 1) * 0x9c4 + 300000;
        out->m_hitRecovery = static_cast<short>(level * 32 - level * 2 + 0x16a8);
        break;
    default:
        break;
    }
    return out;
}