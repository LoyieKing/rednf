// ============================================================================
// 3. advancealtar::AdvanceAltarShopParameter::getdataType —— ORIG T 0x088a2b8e
//
// 字段类型 → 数据形态：无符号比较 ft<8 且位集 0xc8（bit3/6/7 = name/explain/
// type explain，字符串型字段）命中返 0（字符串），其余返 1（整数）。
// ============================================================================
// [b28d-D43][W6] N6 位折叠攻形：内联 `1u<<ft` 被 4.4 折 `(0xc8>>t)&1`；
// 改裸 register（P231，非绑定，不占槽）承载移位基值，4.4 保 `shl %cl` 形（ORIG 0x088a2b8e）。
// [N4Free191to210.S3 2026-09-11] NEAR 真残差 = ORIG `mov $1,%edx; mov %edx,%ebx`
//   （基值 1 先落 edx 再拷进 shl 目标 ebx）+ ft 只读一次（eax 复用 cmp 结果）；
//   我方 `mov $1,%ebx` + 二次 `mov 0xc(%ebp),%eax`（2×mov 换 2×mov，20=20 NEAR）。
//   已实测反劣变体（均回滚）：V_a（bit+shifted 双 register）20→23（push esi）、
//   V_b（idx register 副本）20→22（push esi）、V_w（bit 外提）20→20 但装载序变
//   （mov $1,%ebx 提前到 cmp 前）⇒ NEAR→DIFF。
//   ⇒ 4.4.6 恒把「跨 if 存活的 register 标量」升 esi，ORIG 的 eax 复用形判 N。
int advancealtar::AdvanceAltarShopParameter::getdataType(FieldType::T ft)
{
    if (static_cast<unsigned int>(ft) < 8)
    {
        register unsigned int bit = 1u;
        bit <<= ft;
        if ((bit & 0xc8u) != 0)
            return 0;
    }
    return 1;
}