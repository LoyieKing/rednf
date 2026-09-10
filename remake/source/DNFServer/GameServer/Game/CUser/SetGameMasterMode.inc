bool CUser::SetGameMasterMode(bool flag)
{
    m_gmUserFlag = flag;
    Packet_Register_GM_MID pkt;
    if (flag)
    {
        // b28d-solo-CUser 重写（2026-09-09，P52 链式 receiver+R2L 实参）：ORIG
        //   0x086796dc 把 `lea -0x16(%ebp),%ebx`（pkt 地址）跨 GetServerProxy 调用
        //   保活，group/proxy 均不落槽（链式直传）；characNo 为**死槽**（P171 照抄）。
        //   另：ORIG 尾恒 `mov $0x1,%eax` ⇒ 真实返回 bool（CUser.h 冻结，待 Main 代落）。
        // ORIG 0x086796dc 有 `call getCurCharacNo` 但结果**不落槽**（P0 表达式直用）；
        // 具名 characNo 反致帧 +0x10（sub 0x34→0x44、槽 -0x1c→-0x2c）。
        // b28d-solo-CUser 三试（报告双段复核 + Ghidra local_10）：ORIG 0x08679712
        //   `mov %eax,-0xc(%ebp)` 确认是 getCurCharacNo 结果的具名槽（Ghidra local_10
        //   赋后全程未读 = P171 死写）；但补具名 `int characNo` 两句式实测帧
        //   0x34→0x44、`pkt` -0x16→-0x1a、bool spill -0x1c→-0x2c，nd 1→7 反劣
        //   （4.4 具名标量触发 outgoing 保守撑帧，同 P210/P332(b)）⇒ 回退，
        //   该 -0xc 死槽属跨版本帧策略差，维持 N 封顶。
        (void)getCurCharacNo();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendTcpPacket((char*)&pkt, 0xe);
    }
    return true;   // ORIG 尾恒 mov $0x1,%eax（两路径同返 1）
