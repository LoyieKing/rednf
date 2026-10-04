// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0820b50c orig/ours=191/200 ndiff-blocks=33
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %edi | push   %esi | push   %ebx
int Dispatcher_Antibot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 段直证：`sub $0x6c,%esp; lea -0x50(%ebp),%eax` —— 入口**无** 清零 store
    // （原 local_54/local_58 `= 0` 令我方入口多 2 条 movl 且帧 0x6c→0x98）。
    unsigned int local_54;               // -0x50：包内 4 字节序列号（网络序）
    unsigned int local_58;               // -0x54：宿主序原值
    char* local_30;                      // -0x2c：get_buf_ptr 结果（ORIG char* 视图）
    void* local_28;                      // -0x24：+2 指针
    unsigned int local_24;               // -0x20：ntohl 结果
    int local_20;                        // -0x1c：getCharacAntibotSerialNum
    int local_2c;                        // -0x28：antibotChecker 虚调用结果

    // P168a①（callee 返回 bool 的裸形）+ P59（guard 直落前置）：ORIG 0x0820b522..0x0820b557
    // `call get_int; xor $0x1,%eax; test %al,%al; je <body>`，LineFunc 失败块**直落在前**
    // 并被 je 跳过 ⇒ 源码是早返形 `if (!get_int(...)) return LineFunc(...);`；
    // 旧 `(x^1)==0` 包裹形被 4.4 折叠成 `test; je fail` 且 fail 块落到函数尾。
    if (!packet.get_int(local_54))
    {
        return LineFunc(0xa7c1, __PRETTY_FUNCTION__, 0, 0);
    }
    local_30 = packet.get_buf_ptr(packet.get_index());
    if (user->m_abnormalUserLogType == 0)
    {
        user->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)0);
        user->m_abnormalUserLogType = 1;
    }
    else if (user->m_abnormalUserLogType != 4)
    {
        user->m_historyLog.AbnormalUser(
            (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b, local_54,
            (ENUM_ABNORMAL_USER_LOG_TYPE)user->m_abnormalUserLogType);
    }
    if (local_30 != 0)
    {
        if (user == 0)
        {
            cMyTrace local_50(__PRETTY_FUNCTION__, 0xa7df, 0);
            local_50("Antibot Packet log will write, but user ptr is null");
            return 0;
        }
        local_28 = (void*)(local_30 + 2);
        local_58 = 0;
        if (local_28 != (void*)0)
        {
            memcpy(&local_58, local_28, 4);
        }
        local_24 = ntohl(local_58);
        local_20 = user->getCharacAntibotSerialNum();
        if (local_20 != 0 && local_20 + 1U != local_24)
        {
            // ORIG 0x0820b69a..0x0820b702：acc_id→%esi / getCurCharacNo→%ebx /
            // local_58→%edi 三个实参在 cMyTrace C1 **之前**求值并跨构造保活
            // （P227 全表达式临时链；具名 uVar* 会槽化并撑帧）。
            cMyTrace(__PRETTY_FUNCTION__, 0xa7e9, 0)(
                "Antibot Packet Serial Number host_byte_order:%u, network_byte_order:%u, m_id:%u, charac_no:%d, pre_serial_number:%u",
                local_24, local_58, user->get_acc_id(),
                (unsigned int)user->getCurCharacNo(), local_20);
        }
        user->setCharacAntibotSerialNum(local_24);
    }
    // ORIG 0x0820b719..0x0820b769：fn 直落 %edi（`add $0xc,%eax; mov (%eax),%edi`）、
    // 无 checker/fn 具名槽；acc_id→%ebx / (ushort)local_54→%esi 跨 call 保活。
    // 全表达式形复现（P183 去缓存 + P0）。
    typedef int (*CheckerFn)(void*, unsigned int, unsigned int, unsigned int, void*, unsigned int);
    local_2c = (*(CheckerFn*)(reinterpret_cast<char*>(
                   *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) +
               0xc))(
        GlobalData::s_antibotChecker_,
        (unsigned int)G_CEnvironment()->getProcessSequence(), 0,
        (unsigned int)user->get_acc_id(), local_30, local_54 & 0xffff);
    if (user->m_abnormalUserLogType == 1)
    {
        user->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)1);
        user->m_abnormalUserLogType = 2;
    }
    else if (user->m_abnormalUserLogType != 4)
    {
        user->m_historyLog.AbnormalUser(
            (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b, local_54,
            (ENUM_ABNORMAL_USER_LOG_TYPE)user->m_abnormalUserLogType);
    }
    if (local_2c == -1)
    {
        return 0;
    }
    return 0;
}