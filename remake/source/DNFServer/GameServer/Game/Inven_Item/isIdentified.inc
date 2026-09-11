bool stAmplifyOption_t::isIdentified() const
{
    // ORIG 0x081507bc：not 后 shr $0x7,%al（8 位逻辑右移，无 setne）。& 0x80 形式保持
    // 字节移位；原 (uchar)>>7 被 4.4 升格 sar$7+test+setne（N87 移位升格）。
    return ((~m_abilityType) & 0x80) != 0;
}