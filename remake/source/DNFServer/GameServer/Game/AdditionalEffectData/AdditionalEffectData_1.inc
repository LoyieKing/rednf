// ORIG C1ERKSs 0x08929074：初始化列表 m_id 先行、vector 拷贝构造次之
// （ORIG 汇编 m_id@+0 拷贝在 vector C1ERKS1 之前），其余标量函数体逐句赋值。
AdditionalEffectData::AdditionalEffectData(AdditionalEffectData const& other)
    : m_id(other.m_id), m_strings(other.m_strings)
{
    m_mode = other.m_mode;
    m_count = other.m_count;
    m_min = other.m_min;
    m_max = other.m_max;
    m_valueA = other.m_valueA;
    m_valueB = other.m_valueB;
    m_valueC = other.m_valueC;
    m_flag = other.m_flag;
}