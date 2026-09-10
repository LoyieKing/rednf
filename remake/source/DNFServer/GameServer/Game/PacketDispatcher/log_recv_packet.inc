void PacketDispatcher::log_recv_packet(CUser* user, ENUM_PACKETCLASS cls,
                                       unsigned short cmd, char* data, int len)
{
    (void)user; (void)data; (void)len;
    // [b28c-L18] P52 链式 receiver+实参先物化：ORIG 0x0825f72e 先 `(ushort)cmd→%esi`、
    // `cls→%ebx`（callee-saved 跨 call 存活，push esi/ebx）再 `call
    // CPacketTracerInstance`、结果 %eax 免槽直落 (%esp)。拆「命名 tracer 局部 +
    // 成员调用」两语句使 4.4 把结果槽化（-0xc round-trip）；合并为单条链式表达式
    // 即触发 callee-saved 独占形。
    CPacketTracerInstance()->AddTcpLog((int)cls, (int)(unsigned short)cmd);
