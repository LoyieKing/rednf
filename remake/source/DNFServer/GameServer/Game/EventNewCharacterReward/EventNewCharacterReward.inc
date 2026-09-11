// ORIG C1 0x0816bac4：CEventBase ctor + vptr 设置，+0x0c m_job=-1、
// +0x10 m_field10=0（每字段重读 this，函数体赋值形态）。
EventNewCharacterReward::EventNewCharacterReward()
{
    m_job = -1;
    m_field10 = 0;
}