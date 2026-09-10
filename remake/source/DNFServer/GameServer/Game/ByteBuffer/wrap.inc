ByteBuffer ByteBuffer::wrap(char* buffer, unsigned int len)
{
    // [b28d-solo-nexon_cash] N 封顶（帧量化跨版本）：ORIG `sub $0x14` vs 4.4
    // `sub $0xc`。实测 4.4 帧增量恒 align16(局部需求)（char[4]/char[8]/int 均
    // 落 $0x1c），$0x14 不可达（P190 附注②/N95 族）。体 16/16 条全等，仅帧
    // sub/add 立即数差，语义等价。
    return ByteBuffer(buffer, len);
}
