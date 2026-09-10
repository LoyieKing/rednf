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

    // ORIG 段直证：`call <T>; xor $0x1,%eax; test %al,%al; je`（无 `mov %al,-0x41` 槽
    // 往返）⇒ 删 cVar3 具名 char，表达式内联。
    if ((char)(packet.get_int(local_54) ^ 1) == 0)   // xor $0x1: 只有返回值==1 才继续
    {
        local_30 = packet.get_buf_ptr(packet.get_index());
        if (user->m_field8cf00 == 0)
        {
            user->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)0);
            user->m_field8cf00 = 1;
        }
        else if (user->m_field8cf00 != 4)
        {
            user->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b, local_54,
                user->m_field8cf00);
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
                unsigned int uVar4 = (unsigned int)user->getCurCharacNo();
                unsigned int uVar6 = user->get_acc_id();
                unsigned int uVar2 = local_58;
                cMyTrace local_40(__PRETTY_FUNCTION__, 0xa7e9, 0);
                local_40(
                    "Antibot Packet Serial Number host_byte_order:%u, network_byte_order:%u, m_id:%u, charac_no:%d, pre_serial_number:%u",
                    local_24, uVar2, uVar6, uVar4, local_20);
            }
            user->setCharacAntibotSerialNum(local_24);
        }
        // pcVar1 = *(code**)(*(int*)GlobalData::s_antibotChecker_ + 0xc)
        void* checkerObj = *(void**)&GlobalData::s_antibotChecker_;
        typedef int (*CheckerFn)(void*, unsigned int, unsigned int, unsigned int, void*, unsigned int);
        CheckerFn pcVar1 = *(CheckerFn*)(*(int*)checkerObj + 0xc);
        unsigned int uVar7 = local_54 & 0xffff;
        unsigned int uVar4 = user->get_acc_id();
        CEnvironment* this_00 = G_CEnvironment();
        unsigned int uVar6 = (unsigned int)this_00->getProcessSequence();
        local_2c = (*pcVar1)(checkerObj, uVar6, 0, uVar4, local_30, uVar7);
        if (user->m_field8cf00 == 1)
        {
            user->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)1);
            user->m_field8cf00 = 2;
        }
        else if (user->m_field8cf00 != 4)
        {
            user->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b, local_54,
                user->m_field8cf00);
        }
        if (local_2c == -1)
        {
            return 0;
        }
        return 0;
    }
    LineFunc(0xa7c1, __PRETTY_FUNCTION__, 0, 0);
    return 0;
