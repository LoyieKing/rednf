CEventManager::CEventManager()
{
    std::memset(m_events, 0, sizeof(m_events));
    // ORIG 0x08114ce4：memset 0x298，operator new(0x18) 后调 CDelivery::CDelivery()
    // （0x08116386），存 CDelivery* 于 +0x298。此处用真实 CDelivery 构造。
    // [b24] 直接 `m_delivery = new CDelivery`（ORIG 寄存器直传 %ebx→+0x298，
    // 无具名局部栈槽 -0x1c 中转；具名局部在 GCC11 生成 -0x1c(%ebp) 槽，A 类 1 insn 差）。
    m_delivery = new CDelivery;
}