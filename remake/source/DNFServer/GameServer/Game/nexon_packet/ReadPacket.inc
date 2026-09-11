// ORIG 0x086943af (sz 0x131)：与 ctor 对称的读取路径。ORIG 将 m_body 内容
// scan 回 **packet 参数** 指向的缓冲（全部目标 = packet+偏移；out 参数未用，
// 见 CUser::SendLoginToPCRoomServer 桥接调用 (ReadPacketFn)(packet, reply)）。
//   CPacketHelper helper(this->m_body, 0x1000, 0);
//   helper.scan((unsigned char*)packet,      1);        // m_type
//   helper.scan((unsigned char*)packet+1,    1);        // m_accLen
//   helper.scan((unsigned char*)packet+2,    packet[1]); // m_account
//   helper.scan((unsigned char*)packet+0x101,1);        // m_charLen
//   helper.scan((unsigned char*)packet+0x102,packet[0x101]);
//   helper.scan((unsigned char*)packet+0x201,4);        // m_ip
//   helper.scan((unsigned char*)packet+0x205,1);        // m_canTry
//   helper.scan((unsigned char*)packet+0x206,8);        // m_userIdx+m_accId
// [b28d-D9] N95：CPacketHelper 对象槽基差 0xc（-0x18 vs -0xc）+ 帧 0x28 vs 0x20（ORIG 0x086943af sz 0x00000131）
void stLoginPacket::ReadPacket(const char* packet, stLogin& out)
{
    (void)out;  // ORIG 未使用该参数（解码目标为 packet 缓冲）

    // [S4NexonProto] ORIG 0x086943b5-0x086943be `mov 0x8(%ebp),%eax; mov %eax,-0xc(%ebp);
    // mov -0xc(%ebp),%eax; add $0x4,%eax` = 具名指针局部（承载 this）的 -O0 槽往返形；
    // 缺它令 helper 对象落 -0xc（ORIG -0x18）且帧 $0x20（ORIG $0x28）。探针
    // /tmp/probe/nxp.cpp B/C 对照：stLoginPacket* self 形逐条命中 ORIG（含帧与槽位），
    // stPacket* self 形无 `add $0x4` 反例。残差仅 arg2/arg3 的 `movzbl 0x1(%eax)`
    // 位移折叠（探针 arg.cpp/arg2.cpp 12 形实测：4.4 对 `p[1]` 恒发 add+movzbl，
    // 折叠形不可造 ⇒ N 封顶）。
    // ORIG 局部区 0x18 = helper(0xc) + self(4) + **8 字节未引用槽**（帧 $0x28 = 0x18+0x10；
    // 我方仅 0x10 ⇒ $0x20，且 helper 落 -0x10/self -0x4 而非 ORIG 的 -0x18/-0xc）。
    // 该 8 字节在 ORIG 汇编内无任何访问（同 connect_nonb 的 `int local_24` dead 槽手法，P31）；
    // 槽位订正：必须声明在 self **之前**（-O0 逆序分配）才能让 self 紧贴 helper 之上落 -0xc，
    // 空洞留最浅 -0x8..-0x1。
    unsigned int localSlotPad0;
    unsigned int localSlotPad1;
    (void)sizeof(localSlotPad0);
    (void)sizeof(localSlotPad1);
    stLoginPacket* self = this;
    CPacketHelper helper(self->m_body, MaxBodySize, 0);

    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)), 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 1, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 2,
                static_cast<int>(static_cast<unsigned char>(packet[1])));
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x101, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x102,
                static_cast<int>(static_cast<unsigned char>(packet[0x101])));
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x201, 4);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x205, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x206, 8);
}