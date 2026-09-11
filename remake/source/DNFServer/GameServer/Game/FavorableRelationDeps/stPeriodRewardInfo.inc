// ORIG 0x08a00c70：三个 string（+0x4/+0x10/+0x14）默认构造 → 标量清零
// （word@0、int@8、int@c）→ 三个 string 依次赋 ""（ORIG 空串常量 @0x08e1ce20）。
stFavorableRelationShip_t::stPeriodRewardInfo::stPeriodRewardInfo()
{
    m_type = 0;
    // [OurOnly33 符号名轴 2026-09-11] ORIG 全 ELF 无 `kEmptyString`（我方发射
    // `_ZZN25stFavorableRelationShip_t18stPeriodRewardInfoC1EvE12kEmptyString`，ORIG None）——
    // ORIG 三处直接以匿名空串常量 @0x08e1ce20 赋 string ⇒ 删 local static、使用点内联字面量
    // （空串仍落匿名 .rodata，装载形与 ORIG 一致）。
    m_name = "";
    m_a = 0;
    m_b = 0;
    m_desc = "";
    m_param = "";
}