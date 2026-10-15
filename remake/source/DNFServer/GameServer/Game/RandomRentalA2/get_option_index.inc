unsigned char RandomOption::get_option_index(ENUM_RANDOM_OPTION_NUMBER num)
{
    // [b28d-solo-RandomRentalA2 判词] AE 21/21（ORIG 0x085f4040）：P235 匿名 union
    // 真实成员数组视角复现 ORIG `mov num,%edx; mov this,%ecx; …movzbl (%eax,%ecx,1)`
    // index-first 装载序（探针 /tmp/rr_probe3.cpp S3::m1 逐条全等；cast 数组指针
    // 恒 this-first 已弃）。
    // W 0x85f4040：change_option ? this[0xa] : this[num*3+0]
    if (change_option(num))
    {
        return m_changedOption.m_optionIndex;
    }
    // [b28d-solo-RandomRentalA2 P235] 匿名 union 真实成员数组视角：4.4 产 ORIG 的
    // index-first 装载序（`mov num,%edx; mov this,%ecx`）与双寄存器寻址，逐条全等
    // ORIG 0x085f4040（探针 /tmp/rr_probe3.cpp S3::m1 实证；cast 数组指针恒 this-first）。
    return m_slots[num].m_optionIndex;
}