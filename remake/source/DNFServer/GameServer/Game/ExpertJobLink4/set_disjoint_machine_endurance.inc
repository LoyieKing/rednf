// ORIG W 0x085d3e72：alter() 后写 +0x08。
void CCharacExpertJob::set_disjoint_machine_endurance(int value)
{
    alter();
    m_endurance = value;
}