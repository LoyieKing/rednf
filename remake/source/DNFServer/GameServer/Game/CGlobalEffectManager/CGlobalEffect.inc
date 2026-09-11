// ORIG 0x084b800a：type@+0、value@+4、name@+8（无条件 strncpy 0x1e、无尾 0）、endTime@+0x28。
CGlobalEffect::CGlobalEffect(unsigned int type, float value, const char* name,
                             long endTime)
    : m_type(type), m_value(value)
{
    strncpy(m_name, name, sizeof(m_name) - 2);  // 0x1e，匹配 ORIG
    m_endTime = endTime;
}