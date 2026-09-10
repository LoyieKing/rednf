void ProcStage::incCurrentMs()
{
    int speed = Manager::gmTimeSpeed_ * 1000;   // ORIG -0x4 槽往返（P0 具名局部）
    m_field8 = m_field8 + speed;
