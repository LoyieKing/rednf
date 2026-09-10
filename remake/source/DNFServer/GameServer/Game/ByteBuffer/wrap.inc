ByteBuffer ByteBuffer::wrap(char* buffer, unsigned int len)
{
    // [S4NexonProto 复测] 帧 $0x14 仍是 N 封顶（跨版本帧量化）。本轮再验两形：
    //   ① `ByteBuffer tmp(buffer, len); return tmp;`（具名临时 + NRVO）= 16 条全等、
    //      仍 `sub $0xc`（NRVO 把 tmp 直接构造到 sret，槽不保留）；
    //   ② `const ByteBuffer tmp(...)` 同上（仍 $0xc）。
    // ⚠ 探针 /tmp/probe/wrp.cpp W0/W3 在小函数/静态成员语境下给 $0x14，属**探针假象**
    //   （真实 TU 的 `ByteBuffer ByteBuffer::wrap` 非静态成员签名下 4.4 恒 $0xc），
    //   不得据此再判「可造」。带局部则落 $0x1c（lane 旧测 char[4]/char[8]/int 一致）。
    return ByteBuffer(buffer, len);
}
