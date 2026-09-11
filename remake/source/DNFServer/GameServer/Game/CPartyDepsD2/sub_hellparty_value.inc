// ---------------------------------------------------------------------------
// sub_hellparty_value —— ORIG T 0x085b690a
// 报告：docs/class_func_reports/CParty/sub_hellparty_value.md
// hellValue *= ratio4(CBF+0x1f8)；ROUND 后低于最小值(int@CBF+0x1ec)则钳制；
// 写回 BattleData.m_hellPartyValueTotal(+0xb0c) 后广播。
// ---------------------------------------------------------------------------
void CParty::sub_hellparty_value()
{
    // [N-类] 剩 15：ORIG 0x085b5a4e 为 4.1 位移折叠形（flds 0xb0c/0xd1c、
    // mov 0xd10 单段）；4.4.7 恒物化基址 `add $0x32c/$0xb24,%eax`+短位移
    // （地址算术折叠跨版本差，同 P112/memset-edx 类）。语义同址已验证
    // （0x32c+0x7e0=0xb0c、0xb24+0x1f8=0xd1c、0xb24+0x1ec=0xd10）。
    // [待 CParty 具名化专改]（Main 裁决转 MonC9：m_padBattleData→BattleData
    // m_battleData 后 4.4 或折叠单段位移）。
    float value = bd(this).m_hellPartyValueTotal *
                  bf(this).m_hellPartyRatio4;              // flds×2 fmulp（交换律等价，复现 ORIG 压栈序）
    if (D2_ROUND(value) < bf(this).m_hellPartyRatio1)                    // fistpl/jge
    {
        value = static_cast<float>(bf(this).m_hellPartyRatio1);          // fildl
    }
    bd(this).m_hellPartyValueTotal = value;              // fstps 0xb0c
    send_hellParty_value();                              // T 0x085b697c
}