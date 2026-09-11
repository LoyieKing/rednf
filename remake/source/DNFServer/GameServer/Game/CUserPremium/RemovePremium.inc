// [b28d-solo-CUserPremium] P309 + 早返尾形（b28d 复测）：ORIG 0x086adba6 两处
// std::max 临时槽对为四个独立槽（if 臂 dec/zero=-0x18/-0x14、else 臂
// dec/zero=-0x10/-0xc）⇒ 四标量提函数头且声明序 zero2(-0xc)/dec2(-0x10)/
// zero1(-0x14)/dec1(-0x18)，4.4「头部声明先声明浅」精确复现四槽。
// 尾形：ORIG 两臂各自 `jmp <leave>`（0x86adc2f/0x86adc64）+ je 落 nop 岛
// （0x86adc66）⇒ 源形必须是「if 臂 return + else 臂落体外 return」两处裸早返
// （探针 /tmp/b28d_solo/p3.o f4：`if(kind==1){…;return;} …;return;` 产
// `jne 臂2; 臂1; jmp exit; 臂2; jmp exit; nop; exit` 与 ORIG 逐条同形）；
// if/else 形 4.4 令末臂直落（缺第二 jmp）⇒ 已由本形取代。
// 终判（b28d-solo-CUserPremium，routed 口径）：IDENTICAL 59/59（ndiff 0）、
// P167 CLEAR（2/2）。
void CUserPremium::RemovePremium(int type)
{
    // ORIG 0x86adba6：计数递减走 std::max<int>(zero, dec) 调用形（两临时
    // 槽分支各独立，非三目）；链全重算。
    // P309：声明序 = ORIG 槽浅→深（勿改序，否则槽位回退）。
    int zero2;
    int dec2;
    int zero1;
    int dec1;
    if (m_entries[type].m_state != 2)
    {
        m_entries[type].m_type = 0;
        if (g_SPremiumInfo[type].m_kind == 1)
        {
            dec1 = m_normalCount + -1;
            zero1 = 0;
            m_normalCount = std::max(zero1, dec1);
            return;
        }
        dec2 = m_restrictedCount + -1;
        zero2 = 0;
        m_restrictedCount = std::max(zero2, dec2);
        return;
    }
    return;
}