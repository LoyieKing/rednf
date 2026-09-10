void _NS_PI_MakeHash_NOTI(PacketBuf* packet)
// [b28c-L23 翻案 + N(jump threading) 残余 3 行] b22 的 13 形"不可复现"判词作废：
// 本 TU 局部声明同步为 bool callee（档①）后 `!call()` 直产 ORIG
// `call; xor $0x1,%eax; test %al,%al; jne`（ndiff 7→3）。残 1 条 = ORIG 尾对
// `jmp/jmp` 跳板（4.1 为 put_int 分支保留双跳板），4.4 折叠掉一条（实测 P223 的
// goto+尾空标签形在本函数不触发，同 write_log_gain_money 判例）+ 连锁偏移。
{
    if (packet == 0)
    {
        goto L1;
    }
    {
        char* base = packet->get_packet(0);
        // ORIG：读取包首 +1 处的 ushort 字段（本函数内不使用，仅为对齐 ORIG 读取行为）
        unsigned short headerField = *(unsigned short*)(base + PKT_SIZE_OFF);
        char* region = base + PKT_BODY_OFF;
        short idx = packet->get_index() - PKT_BODY_OFF;
        int hash;
        // bool callee + 裸 `!call()` ⇒ 直产 ORIG @0x08274757 的
        // `call; xor $0x1,%eax; test %al,%al; jne`（假路径跳板）。
        if (!_NS_MakeHash_(region, idx, hash))
        {
            goto L2;
        }
        packet->put_int(hash);
        goto Lend;
    }
L1:
L2:
Lend:
    ;
