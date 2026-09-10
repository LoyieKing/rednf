void CMonsterDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result)
{
    // ORIG movzbl +0x35; cmp $0x3,%al; jg（movzbl 零扩展 + 8 位有符号比较；
    //   探针实证 (signed char)(unsigned char)x > 3 形）
    if (static_cast<signed char>(static_cast<unsigned char>(ref.m_field35)) > 3)
        return;

    bool done = false;                                  // local_25

    if (ref.m_field40_byte == 0)                        // hell 旗标低字节为 0
    {
        for (int n = 0; n < static_cast<char>(ref.m_field39); ++n)
        {
            m_special._generateRandomItem(
                static_cast<unsigned char>(ref.m_field34),
                static_cast<char>(ref.m_field35),
                static_cast<unsigned char>(ref.m_rateSlot),
                static_cast<unsigned char>(ref.m_pad37),
                ref.m_field38,
                ref.m_field3c,   // §4.3 具名化：+0x3c int 视图（CItemGenerator.h:224）
                ref.m_field41, ref.m_dropRate, result, done);
        }
    }

    for (int n = 0; n < static_cast<char>(ref.m_field39); ++n)
    {
        // forceTag 决定本轮首杀权重分量回退：[0]→hunt[0]，[1]→hunt[+4]
        if (ref.m_forceTag.operator[](0))
            m_firstMobWeight = m_firstMobHunt.m_weight;   // ORIG mov (m_firstMobHunt)→m_firstMobWeight
        else if (ref.m_forceTag.operator[](1))
            m_firstMobWeight = m_firstMobHunt.m_field04;  // ORIG mov (hunt+4)→m_firstMobWeight
        m_firstMobWeightRate = ref.m_field1c;           // dword ← ref+0x1c

        _generateRandomItem(static_cast<unsigned char>(ref.m_field34),
                            static_cast<char>(ref.m_field35),
                            static_cast<unsigned char>(ref.m_rateSlot),
                            static_cast<unsigned char>(ref.m_pad37),
                            ref.m_field38,
                            ref.m_field3c,   // §4.3 具名化：+0x3c int 视图
                            ref.m_dropRate, result, done);

        m_firstMobWeight = 1;
        m_firstMobWeightRate = 1.0f;                    // 0x3f800000 位还原
    }
