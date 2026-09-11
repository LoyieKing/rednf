// ============================================================================
// 7. Dispatcher_Client_Spec_Statistic::read  ORIG 0x081ce5fe
//    msg+0xd/+0xe/+0xf 三字节，+0xf 为条数；条目步长 0xc：
//    +0x12 byte、+0x16 dword、+0x1a dword（ORIG 裸偏移直读 MSG 体）。
//    b28c-34 判词（identical）：ORIG 寻址为 (p + i*0xc) + K（先 add 基址、后 add $K，
//    0x081ce65e..0x081ce6a5）。4.4 前端会把纯指针形 `p + i*0xc + K` 折叠成
//    `p + (i*0xc + K)`（先 add 常量、后 add 基址，后两条再多 mov %ecx/lea）；
//    经 unsigned long 中转 `(unsigned long)(p + i*0xc) + K` 阻止该折叠，逐条复现 ORIG。
// ============================================================================
int Dispatcher_Client_Spec_Statistic::read(PacketBuf& packet, MSG_BASE& msg)
{
    unsigned char* const p = reinterpret_cast<unsigned char*>(&msg);
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd));
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xe));
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xf));
    for (int i = 0; i < (int)reinterpret_cast<unsigned char (&)[0x10]>(*p)[0xf]; ++i) {  // ORIG movzbl 0xf(%eax)
        packet.get_byte(*reinterpret_cast<unsigned char*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x12));
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x16));
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x1a));
    }
    return 0;
}