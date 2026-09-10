void Manager::makeSyncPacket(CUser* user)
{
    UserInfo& info = user->m_ceraUserInfo;
    // [b28d-solo P274/P211] ORIG 0x080e0f69 累加段 `mov -0x10(%ebp),%edx;
    // add %edx,%eax; mov %eax,-0x10(%ebp)`（size2 在 %eax 为左操作数）⇒ 源写
    // `total = size2 + total;`；`total += size2;` 使 4.4 折 `add %eax,-0x14`
    // 内存直加（少 2 条 + 槽号漂移）。
    int total = (int)info.m_accountDaily.size();
    total = (int)info.m_characDaily.size() + (unsigned int)total;
    // [b28d-solo P223] ORIG 0x080e10cf 尾空标签 nop 岛（`je +0x195` 落点即
    // `nop`，紧接 epilogue）⇒ 负条件 goto 共享尾空标签复现。
    if (total == 0)
        goto sync_packet_end;
    {
        std::vector<unsigned int> list;
        GetItemListForPacket(list, info.m_accountDaily);
        GetItemListForPacket(list, info.m_characDaily);
        // [b28d-solo P292/P168a] ORIG 0x080e0fc1 `call empty; test %al,%al; jne
        // <vector 析构前>` = 正极性直接判 empty；`if (!list.empty())` 使 4.4 多
        // 一条 `xor $0x1,%eax`（bool 取反物化）⇒ 正极性空 then + else 形。
        if (list.empty()) {
        } else {
            PacketGuard packet;
            // [b28d-solo P183/P89] ORIG 每处 `lea -0x2c(%ebp),%eax; mov %eax,(%esp)`
            // 重算 packet 地址；具名 `InterfacePacketBuf* buf` 多一个 -0x10 槽并
            // 使 vector 槽整体深移 4。
            ((InterfacePacketBuf*)&packet)->clear();
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x222);
            ((InterfacePacketBuf*)&packet)->put_int((int)list.size());
            for (unsigned int i = 0; i < list.size(); ++i)
                ((InterfacePacketBuf*)&packet)->put_int((int)list[i]);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            user->Send(packet);
        }
    }
sync_packet_end:;
