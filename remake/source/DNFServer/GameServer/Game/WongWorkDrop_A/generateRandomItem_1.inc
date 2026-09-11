// ===========================================================================
// CObjectDrop::generateRandomItem — ORIG T 0x0853818a
// b21 同族：u8_at/i8_at → m_field34/m_field35/m_rateSlot/m_pad37 成员直读（语义不变）。
// ===========================================================================
// [b28d-solo-WongWorkDrop_A] verdict=AE：去 type 具名局部（P0，ORIG 两次独立
// `movzbl 0x34` 直读；帧 -4、done/n 槽 -0x21/-0x20 → -0x1d/-0x1c 全等）。
void CObjectDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                     stGenerateResult_t& result)
{
    bool done = false;                                  // local_21
    for (int n = 0; n < static_cast<signed char>(ref.m_pad37); ++n)
    {
        // [b28d-solo-WongWorkDrop_A] P0：ORIG 两次独立 movzbl 0x34 直读，
        // 无 type 具名局部（去掉后帧 -4、槽号 -0x21/-0x20 → -0x1d/-0x1c）。
        _generateRandomItem(static_cast<unsigned char>(ref.m_field34), 0,
                            static_cast<unsigned char>(ref.m_field34),
                            static_cast<unsigned char>(ref.m_field35),
                            static_cast<unsigned char>(ref.m_rateSlot),
                            0,                          /* context3c = NULL */
                            ref.m_dropRate, result, done);
    }
}