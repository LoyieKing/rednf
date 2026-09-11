// [b28d-30] 复核 DIFF：越界检查形/帧差（N82 族），ORIG 0x08636a7c。
// [b28d-solo-TimerDispatcher] 判词 N104 封顶：P183 去 notices/packet 缓存 + 真语义
// bug（第二形参）修正后 112/112 助记符全等；唯一残差 = EH 尾块相对序（ORIG 正常路径
// D1 在 +0x183 最末、unwind 块居中 +0x168；4.4 就地内联 D1、unwind 压最末）⇒ N104。
// [b28d-solo-TimerDispatcher] 真语义 bug 修正：ORIG 全程读 **0x10(%ebp) = 第二形参**
// （越界检查 08636a84、size 08636a9f、operator[] 08636b1c、RegistNextTimer 08636bb6），
// 旧码误用第三形参 p3（0x14(%ebp)）。
bool TimerLightServerNotice::dispatch_sig(int /*p1*/, int p2, memberIdentificationNumber_t /*p3*/)
{
    // ORIG 0x08636a7c：越界检查（idx 负或 >= size 直接 false）→ 构造 0xc 包
    // {byte 0, short 0, byte 0, int len, str} 广播 → RegistNextTimer(idx)。
    // P183 地址重物化铁律：ORIG 每个站点重发 `call G_CDataManager; add $0xa4,%eax`
    // （08636a8d/08636b1f/08636b58），禁 `std::vector<...>& notices = ...` 引用缓存。
    // [P7HeaderAxis 2026-09-11] bool 化（ORIG 整族 dispatch_sig 返回 bool）：ORIG 尾 `mov %ebx,%eax` 直返、无 test/setne。
    register bool ret;
    if (p2 < 0 ||
        static_cast<unsigned int>(p2) >=
            G_CDataManager()->m_serverParameter.m_light3c.size())
    {
        ret = 0;
    }
    else
    {
    PacketGuard guard;
    // P89/P183：ORIG 每站点 `lea -0x1c(%ebp),%eax; mov %eax,(%esp)`（guard 地址直传），
    // 禁具名 `InterfacePacketBuf& packet`（多占 -0x18 槽）。
    static_cast<InterfacePacketBuf&>(guard).put_header(0, 0xc);
    static_cast<InterfacePacketBuf&>(guard).put_byte(0);
    static_cast<InterfacePacketBuf&>(guard).put_short(0);
    static_cast<InterfacePacketBuf&>(guard).put_byte(0);
    int len = G_CDataManager()->m_serverParameter.m_light3c[p2].m_message.size();
    // ORIG 0x08636b50：先 put_int(len) 再 put_str
    static_cast<InterfacePacketBuf&>(guard).put_int(len);
    static_cast<InterfacePacketBuf&>(guard).put_str(
        G_CDataManager()->m_serverParameter.m_light3c[p2].m_message.c_str(), len);
    static_cast<InterfacePacketBuf&>(guard).finalize(true);
    G_GameWorld()->send_all(guard);
    const bool done = RegistNextTimer(p2);
    // P231/P307：ORIG 汇聚变量驻 %ebx（08636bd6 `mov $0x0,%ebx` / 08636bdd
    // `mov $0x1,%ebx`）且无栈槽 ⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
    if (!done)
        ret = 0;
    else
        ret = 1;
    }
    return ret;
}