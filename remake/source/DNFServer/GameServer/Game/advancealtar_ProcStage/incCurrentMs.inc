// ORIG T 0x0812e146（incCurrentMs.md 全文）：m_field8 += gmTimeSpeed_ * 1000。
void ProcStage::incCurrentMs()
{
    int speed = Manager::gmTimeSpeed_ * 1000;   // ORIG -0x4 槽往返（P0 具名局部）
    m_field8 = m_field8 + speed;
}