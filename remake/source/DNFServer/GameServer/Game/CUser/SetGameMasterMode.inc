// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086796dc | _ZN5CUser17SetGameMasterModeEb
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
        //   `mov %eax,-0xc(%ebp)` 的槽址 = pkt 槽基 -0x16 + 0x0a = -0xc ⇒ 按 **P322①**
        //   （参数对象成员写入判据）该 store 是 **pkt.m_field0a = getCurCharacNo()**，
        //   不是具名 int 死局部（旧 "P171 死写" 判词作废，其两次复测读数属补救具名
        //   标量反劣，与本形无关）。语义：GM 注册包必须带当前 characNo，我方旧形
        //   (void) 丢弃 ⇒ 包内容错误（真语义 bug）。
        //   4.4 形验证（探针 /tmp/CuB/probe/p1.cpp 的 f，c6root 4.4.7 -m32 -O0）：
        //   `PKT pkt; pkt.f0a = getNo();` ⇒ `call getNo; mov %eax,-12(%ebp)` 直落成员。
        pkt.m_field0a = getCurCharacNo();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendTcpPacket((char*)&pkt, 0xe);
    }
    return true;   // ORIG 尾恒 mov $0x1,%eax（两路径同返 1）
}