bool CVillageMonsterMgr::OnEvent()
{
    // ORIG 0x82f0f44：返回 bool(m_field04 != 0)
    return m_field04 != 0;
}