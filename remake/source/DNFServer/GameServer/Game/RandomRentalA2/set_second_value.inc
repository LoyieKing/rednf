void RandomOption::set_second_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v)
{
    // [b28d-solo-RandomRentalA2 判词] AE 26/26（ORIG 0x085f40be）：同 set_first_value（P235）。
    // W 0x85f40be：change_option ? this[0xc]=v；恒写 this[num*3+2]=v
    if (change_option(num))
    {
        m_fielda.m_field2 = v;
    }
    // [b28d-solo-RandomRentalA2 P235] 同 set_first_value（探针 rr_probe3.cpp S3::s1）。
    m_fields[num].m_field2 = v;
