void CUser::setHackUser(unsigned int p1, unsigned int p2, unsigned int p3,
                        unsigned int p4, unsigned short type, unsigned int p6)
{
    // b28d-W8: P227 全表达式临时链 + 直接用形参 type —— ORIG 0x867f753/75a
    // `mov 0x1c(%ebp),%eax; mov %ax,-0x6c(%ebp); movzwl -0x6c(%ebp),%esi`
    //（仅形参一次性截断 spill 槽，无 notifyType 复制槽）；0x867f7be 前右起求值
    // type(%esi)→NumberToString(%ebx)→ctor，idStr 无槽（P0）。尾部改形参本身
    // 0x867fb20 `movw $0xa,-0x6c(%ebp)`（ORIG 直接改 spill 槽）。
    cMyTrace("void CUser::setHackUser(unsigned int, unsigned int, "
             "unsigned int, unsigned int, short unsigned int, unsigned int)",
             0x7e55, 0)(
        "m_id : %s, SetHackUser : %d(%d)\t  %d(%d)   ApplyFlag = %d\n",
        NumberToString(get_acc_id(), 0), p1, p2, p3, p4,
        static_cast<unsigned int>(type));

    if (type == 2)
    {
        m_lookOuts1.push_back(stLookOuts(p1, p3));
    }
    else if (type == 3)
    {
        // b28d-W8: P59/P248档② —— ORIG 0x867f820 `movzbl 0x8e950; xor $1; test;
        // je <return 岛>`（`!flag` 正极性包裹 + else return），正写
        // `if(flag) return; body` 产 `test; jne` 少 1 条 xor。
        if (!m_mcap.m_hackIpLogged)
        {
            // b28d-W8: 声明序照 ORIG —— rawIp 先清零(0x867f832 组 -0x58)+GetPeerIP2，
            // startIp 后清零(0x867f86e 组 -0x68)+Divide 输出（P28 声明序即发射序）。
            // 16 字节各 4×movl（无第 5 条 movb）；GetPeerIP2(rawIp,0x10) 写满 16B
            // 无 NUL（P171 原码遗留照抄）。
            char rawIp[16] = {0};
            m_network.GetPeerIP2(rawIp, 0x10);
            char startIp[16] = {0};
            Divide_C_ClassIP_EndIP(startIp, static_cast<char*>(0), rawIp);

            m_ipCounters.push_back(tyIPCounters(p1, p3));
            m_mcap.m_hackIpLogged = true;
            // b28d-W8 真语义修正：ORIG 0x867f8ee `lea -0x58(%ebp),%ecx` 传的是
            // rawIp（GetPeerIP2 目标缓冲 = local_6c+0x10），非 startIp（Divide 输出
            // = -0x68）；旧码传错缓冲 ⇒ 记的是点分起始段而非原始 IP。
            m_historyLog.IPCounterLog_InsertIp(static_cast<int>(p1),
                                               static_cast<int>(p3), rawIp);
            DB_HackIPSave::makeRequest(startIp, rawIp, p1, p3);
            return;
        }
        else
        {
            return;
        }
    }
    else if (type == 10)
    {
        m_lookOuts2.push_back(stLookOuts(p1, p3));
        if (isTradePunishType(4))
        {
            return;
        }
        // b28d-solo-W8_CU_G 真语义修正：ORIG 0x0867f988 只调 get_rand_int 一次
        //（结果存 -0x14 槽），printf 与 InsertTimer 复用同值；原码 printf 实参又调
        // 一次 get_rand_int（p167 EXTRA +1，且消耗额外随机数）⇒ 合并为单次。
        int waitSec = get_rand_int(0x122) + 10;
        printf("ARS TRADE PUNISH USER - Rand Sec : %d\n", waitSec);
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                    static_cast<int>(GetUID()),
                                    static_cast<TIMER_MESSAGE>(0x97), waitSec,
                                    get_unique_id(), 0xffffffffu);
    }
    else
    {
        int delaySec = get_rand_int(0x122) + 10;
        m_379.m_field0 = true;
        if (type == 8)
        {
            m_mcap.m_field561 = true;
            delaySec = 1;
        }
        // b28d-W8: P133 —— ORIG 0x867fa2e..0x867fa72 四条独立 `cmpw $X; jne;
        // movb $1,0x8e951` 守卫（非合并短路，合并形产 4×je 同落点）。
        if (type == 7)
        {
            m_mcap.m_field561 = true;
        }
        if (type == 9)
        {
            m_mcap.m_field561 = true;
        }
        if (type == 12)
        {
            m_mcap.m_field561 = true;
        }
        if (type == 5)
        {
            m_mcap.m_field561 = true;
        }
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(0),
                                    static_cast<int>(GetUID()),
                                    static_cast<TIMER_MESSAGE>(0x81), delaySec,
                                    get_unique_id(), 0xffffffffu);
        OnPunishUser();
        if (m_field796d8 != 0 && GetTradePunishType() != 0)
        {
            return;
        }
        if (m_field796d8 != 0 && (type == 1 || type == 4 || type == 9))
        {
            type = 10;
        }
    }

    // b28d-W8: ORIG 0x867fb26..0x867fb3b 4×movl 内联清零 16B（无第 5 条 movb）
    // ⇒ `= {0}` 的 16 字节数组，非未初始化 17 字节。ORIG 0x867fb66 `mov %eax,%ebx`
    // = GetPeerIP2 返回值直传 makeRequest arg1（无 ipPtr 具名槽，P0 内联实参）。
    char ip[16] = {0};
    DB_InsertAutoPunishFirstUser::makeRequest(
        get_acc_id(), m_network.GetPeerIP2(ip, 0x10), p1, p2, p3, p4, type, p6);
