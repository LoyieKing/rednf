// subtract（ORIG W _ZN15_Additioal_info8subtractERKS_ @0x08693d04）：与 add 同构、
//   逐字段相减——唯 ORIG 对 m_field4c 与 m_field4e 两字段仍执行加法
//   （lea (%edx,%eax,1) 与 add %eax,%edx 指令实证），忠实保留该行为。
// [b21 N-86b] 语义已闭合：与 add 同族——ORIG(4.1) 16 位差经
//   `mov %eax,%edx`+`lea/sub` 寄存器中途（含 mov %ebx,%eax 回转），4.4 恒发直接
//   `sub %ax,%cx`/`add %eax,%edx` 形；0x4c/0x4e 加法特例已忠实保留。逐字段语义
//   逐一相等，封 N。
// [b28d-solo-CUserPremium] N-86b/N7 封顶（b28d 复测）：同 add——ORIG 16 位差
// 经 lea/mov 寄存器舞 + %edx 索引中转，4.4 恒直接 sub/add + %ecx；0x4c/0x4e
//  两字段加法特例已忠实保留。P167 CLEAR（0/0）⇒ 语义等价，封顶。
// 本波复测：同 add 探针族（13 形）不可择；clean/AE ndiff 121→62，语义等价。
// [L3CUserPremDBInven 本波复核（ORIG 0x08693d04）] 复算 175/153（gap 22）：同 add
// 的 16 位收形差（ORIG `mov %eax,%edx`+`lea/sub` 中途 vs 4.4 直接 sub/add）+
// 0x4c/0x4e 两字段加法特例（ORIG `lea (%edx,%eax,1)` = 加，已忠实保留）⇒ 未再动。
void _Additioal_info::subtract(const _Additioal_info& other)
{
    m_field00 -= other.m_field00;
    m_field04 -= other.m_field04;
    m_field08 -= other.m_field08;
    m_field0a -= other.m_field0a;
    m_field0c -= other.m_field0c;
    m_field0e -= other.m_field0e;
    for (int i = 0; i <= 3; ++i)
        m_field10[i] -= other.m_field10[i];
    for (int j = 0; j <= 0x10; ++j)
        m_field18[j] -= other.m_field18[j];
    m_field3a -= other.m_field3a;
    m_field3e -= other.m_field3e;
    m_field40 -= other.m_field40;
    m_field42 -= other.m_field42;
    m_field46 -= other.m_field46;
    m_field48 -= other.m_field48;
    m_field4a -= other.m_field4a;
    m_field4c += other.m_field4c;   // ORIG：加法（非减）
    m_field4e += other.m_field4e;   // ORIG：加法（非减）
}