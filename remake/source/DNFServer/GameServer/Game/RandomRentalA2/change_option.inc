bool RandomOption::change_option(ENUM_RANDOM_OPTION_NUMBER num)
{
    // [b28d-solo-RandomRentalA2 判词] AE（ORIG 0x085f4002）：①P308 位域视角
    // m_seedd.m_bits.m_low2 复现 `movzbl 0xd; and $0x3; movzbl %al,%edx`
    // （标量 (unsigned char)(m_field & 3) 被 4.4 折成先窄化，探针 rr_probe4 c1/c2）；
    // ②P210 empty() 定义后置 ⇒ 4.4 走保守帧 sub $0x18（见下方 empty 定义处注释）。
    // W 0x85f4002：!m_fielda.empty() && (m_seedd & 3) == num
    if (!m_fielda.empty())
    {
        if ((unsigned char)m_seedd.m_bits.m_low2 == (unsigned int)num)
            return true;
    }
    return false;
