void RandomOption::set_first_value(ENUM_RANDOM_OPTION_NUMBER num, unsigned char v)
{
    // [b28d-solo-RandomRentalA2 判词] AE 26/26（ORIG 0x085f4078）：P235 真实成员
    // 数组视角 m_fields[num].m_field1 复现 index-first 装载序 + 两步 lea
    // （探针 rr_probe3.cpp S3::s1 逐条全等；帧 0x28 / v 槽 -0xc 同步归位）。
    // W 0x85f4078：change_option ? this[0xb]=v；恒写 this[num*3+1]=v
    if (change_option(num))
    {
        m_fielda.m_field1 = v;
    }
    // [b28d-solo-RandomRentalA2 P235] 真实成员数组视角复现 ORIG 两步 lea 与
    // index-first 装载序（探针 /tmp/rr_probe3.cpp S3::s1 逐条全等 ORIG 0x085f4078）。
    m_fields[num].m_field1 = v;
}