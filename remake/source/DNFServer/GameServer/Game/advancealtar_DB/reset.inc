// ORIG T 0x08899244（objdump 全文核对）：memset 前 0x21（_RidableBase）→
// m_update=0 → 7 个 map clear（stages/buyItems1..3/slots1/slots2/rewards）。
void AdvanceAltarData::reset()
{
    std::memset((char*)this, 0, 0x21);
    m_update = 0;
    m_stages.clear();
    m_buyItems1.clear();
    m_buyItems2.clear();
    m_buyItems3.clear();
    m_slots1.clear();
    m_slots2.clear();
    m_rewards.clear();
}