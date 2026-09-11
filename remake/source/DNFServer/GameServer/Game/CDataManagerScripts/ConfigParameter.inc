advancealtar::ConfigParameter::ConfigParameter()
{
    // ORIG 0x889b662：+0/+4 清零、char*+0x18=&(+8)、两段 0x10 memset。
    m_0 = 0;
    m_4 = 0.0f;
    m_strPtr = reinterpret_cast<char*>(this) + 8;
    std::memset(m_gageUpRate, 0, sizeof(m_gageUpRate));
    std::memset(m_gageUpAux, 0, sizeof(m_gageUpAux));
}