void STEnchantedSkillOption::reset()
{
    // [b28d-solo-Inven_Item] ORIG 0x080cb778（W，0x26 字节）三段位清：
    //   movzbl (%eax),%edx; and $0xfe/%edx; mov %dl,(%eax) ×3（掩码 0xfe/0xf9/0x7）
    // ⇒ 源形 = 位域清零（1+2+5 布局）。uchar 标量/指针 cast 形 4.4 恒发
    //   `movzbl (%eax),%eax; mov %eax,%edx; and…; mov this,%eax`（+4 条，探针 E1
    //   r1/r2/r3 三形同），位域形逐条全等（探针 E2/E3）。
    m_bits.m_flag = 0;
    m_bits.m_type = 0;
    m_bits.m_skillId = 0;
}