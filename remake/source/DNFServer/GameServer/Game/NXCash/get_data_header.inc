// ORIG 0x081ad168：从入包 body 读 pid/flag/requestId 三元组。
// [b23S3C] P192：sret 返回入包视图对象（NRVO 直构 caller 槽，无额外拷贝）。
// [b28d-D43] NEAR 帧量化：ORIG sub $0x10 vs 4.4 $0xc（P190附注②；体 39/39 全等）（ORIG 0x081ad168）。
ByteBuffer NCashImpl::get_data_header(unsigned char& a, unsigned char& b,
                                      unsigned int& c)
{
    // [b28d-D43] ORIG 0x081ad168 无 in 的 ctor call（wrap 的 sret 即 in 槽，
    // %ebx=get_data_header sret 槽）：直接初始化形令 4.4 把 in 地址传给 wrap。
    // [b28d-solo-nexon_cash] N 封顶（帧量化跨版本）：ORIG `sub $0x10` vs 4.4
    // `sub $0xc`；实测补 4 字节未用局部跳 $0x1c，$0x10 不可达（P190 附注②）。
    // 指令 39/39 全等，仅帧 sub/add 立即数差，语义等价。
    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> a;
    in >> b;
    in >> c;
    return in;
}