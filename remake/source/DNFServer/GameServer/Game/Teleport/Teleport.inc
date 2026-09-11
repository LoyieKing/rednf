Teleport::Teleport()
{
    // ORIG 0x08283c30：设 vptr；stPartyTelePort::reset(this+4)；*(this+0x10)=0。
    m_tp.reset();
    m_endCoolTime = 0;
}