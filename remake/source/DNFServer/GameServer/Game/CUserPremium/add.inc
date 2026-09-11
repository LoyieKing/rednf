// add（ORIG W _ZN15_Additioal_info3addERKS_ @0x08146300）：int@+0x00/+0x04 32 位加；
//   u16@+0x08..+0x0e 显式加；循环 i=0..3 加 u16[i] @ (i+8)*2（即 m_field10[4]）；
//   循环 j=0..0x10 加 u16[j] @ 0x18+2j（即 m_field18[17]）；
//   收尾依次 +0x3a(int)、+0x3e、+0x40、+0x42(int)、+0x46、+0x48、+0x4a、+0x4c、+0x4e(int)。
//   u16 均为零扩展后 mod 2^16 运算（movzwl/add/mov），unsigned short 成员天然良构。
// [b21 N-86b] 语义已闭合：ORIG(4.1) 对 16 位和用 `lea (%edx,%eax,1)`+`mov %eax,%edx`
//   寄存器舞（0x3e/0x40/0x4a/0x4c 四 union 字段），4.4 恒发 `add %eax,%edx` 直形——
//   同算术收形源码不可择（N-86b probe 实证恒内存直形）；循环体寄存器分配同理。
//   逐字段语义逐一相等，封 N。
// [b28d-solo-CUserPremium] N-86b/N7 封顶（b28d 复测）：ORIG 0x08146300 对
// 16 位加发 `movzwl; mov %eax,%edx; movzwl; lea (%edx,%eax,1); mov %eax,%edx`
// 寄存器舞 + %edx 索引中转；4.4 恒折 `movzwl/movzwl/add/mov %dx` 直形且用
// %ecx 索引（探针 /tmp/probe_add.cpp 六形 + probe_add2.cpp 五形全败）。
// P167 CLEAR（0/0）、逐字段语义相等 ⇒ 封顶。
// 本波复测（探针 /tmp/b28d_solo/p1.o c1..c8 + p2.o v1..v5，共 13 形：
// `x+=y` / `x=x+y` / 显式 (unsigned short)/(int)/(unsigned) cast / 数组元素
// / 具名临时 / bitfield 位置）——4.4 恒 `movzx edx; movzx eax; add edx,eax`，
// ORIG 的 `movzx eax; mov edx,eax; …; lea eax,[edx+eax]; mov edx,eax` 舞
// 无任何源形可择 ⇒ N-86b 维持；clean/AE ndiff 116→52（gap 12 条全为四 union
// 字段 + 两循环体的收形差），语义逐字段相等。
// [L3CUserPremDBInven 本波复核（ORIG 0x08146300）] 复算 165/153（gap 12）：diff
// 全为上方 16 位收形 + 循环体 `%ecx/%edx` 索引选择（ORIG 用 `%edx` 中转、4.4 用
// `%ecx`）⇒ N-86b 维持，未再动。
void _Additioal_info::add(const _Additioal_info& other)
{
    m_field00 += other.m_field00;
    m_field04 += other.m_field04;
    m_field08 += other.m_field08;
    m_field0a += other.m_field0a;
    m_field0c += other.m_field0c;
    m_field0e += other.m_field0e;
    for (int i = 0; i <= 3; ++i)
        m_field10[i] += other.m_field10[i];
    for (int j = 0; j <= 0x10; ++j)
        m_field18[j] += other.m_field18[j];
    m_field3a += other.m_field3a;
    m_field3e += other.m_field3e;
    m_field40 += other.m_field40;
    m_field42 += other.m_field42;
    m_field46 += other.m_field46;
    m_field48 += other.m_field48;
    m_field4a += other.m_field4a;
    m_field4c += other.m_field4c;
    m_field4e += other.m_field4e;
}