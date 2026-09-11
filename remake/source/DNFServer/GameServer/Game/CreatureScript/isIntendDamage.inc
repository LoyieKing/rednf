// ORIG 0x8890760：+0x14 != 0 或 damage>0 或 absoluteDamage>0 → true。
bool AttackInfo::isIntendDamage()
{
    // [b28d-solo-CreatureScript] 已离开 DIFF：唯一残差曾是 x87 比较 lowering
    // 两向差——ORIG 0x08890776 `fucompp; fnstsw %ax; sahf; seta` vs 4.4 默认
    // `fucomip %st(1),%st; fstp %st(0); seta`（N85）。P230 补命中：本 TU 路由
    // 追加 `-march=i486 -mtune=generic`（Main 已落 dnf_game_cxx.sh:428）后 4.4
    // 即产 fnstsw/sahf 形 ⇒ 本件转 AE（双后端读数 identical 222→223 / diff 6→5）。
    if (m_weaponDamageApply != 0 || m_damage > 0.0f || m_absoluteDamage > 0)
    {
        return true;
    }
    return false;
}