// ORIG T 0x0831ed6c _ZN8WongWork15CCalcCharacStat13_CalcSwordmanEP15_Additioal_infoii
// 报告：_CalcSwordman.md。secondType 只认 0..4，越界不改写。
// [b28d-54 N] 4.1 内联 16 位算术 vs 4.4 cwtl/movswl 显式扩展 + `add imm,%eax;mov`
//   vs `lea imm(%eax)` 折叠（N1/N97 族）；语义等价。
// [b28d-solo-WongWorkMisc_A] verdict=DIFF(N 封顶)：本棒修法——①全部 32 位字段段
//   （m_field00/04/3a/42）改用 unsigned 字面量（`+0xCxxxu`）：4.4 对 signed 常量的
//   `add imm,%eax;mov %eax,%edx` 恒折 `lea imm(%eax),%edx`，unsigned 即复现 ORIG
//   add+mov（探针 /tmp/probe_ty.cpp fC/fD、/tmp/probe_u.cpp fN/fP/fQ/fR）；②删除
//   `const short t` 具名局部与 `(short)(level<<K)+t` 形式，改写 `level*K + level`
//   （`*` 而非 `<<`）——消除帧 sub/movswl/cwtl/t 槽（ORIG 无帧 sub，P0 判无该具名
//   局部；探针 /tmp/probe_shift.cpp fB/fC）。段级 identical 12→34/47。
//   残余 N（不可翻）：16 位字段段（m_field08..0e/10/40/46/48/4a）ORIG 发 16 位
//   `add/sub $C,%ax` 或 32 位 `add $C,%eax;mov`，4.4 对 u16 目标恒折 `lea C(%eax)`
//   （8 形探针 /tmp/probe_w.cpp gA-gF、/tmp/probe_h.cpp hA-hG 全 lea）⇒ N1/N86b +
//   跨版本窄化。p167: 0 missing / 0 extra；语义与 Ghidra C 逐字段一致。
// [2026-09-10 D06 · DIFF→IDENTICAL 423=423] 旧判词"跨版本窄化不可翻"**作废**：真因是
//   **字段符号性**——ORIG 的 16 位窄化 `add $0x1770,%ax; mov %eax,%edx` 只在 **signed
//   short 视角**下由 4.4 复现（unsigned 视角恒产 `lea`，探针 /tmp/d06/p16c.cpp
//   f20..f27）；改用 CCharacter.h `_Additioal_info` 的 union 同址 signed 视角
//   （+0x40 `m_mpRegen` / +0x4a `m_hitRecovery`）+ `static_cast<short>` 后逐条全等；
//   +0x14 用新增 `m_field10s[4]`（探针 /tmp/d06/p14.cpp g0..g7 矩阵，含
//   reinterpret_cast<short&> 反例：多 `add $0x14,%eax` 基址提前物化）。P321-T1 + P235。
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
}