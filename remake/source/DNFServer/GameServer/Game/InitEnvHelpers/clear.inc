void CreatureCommonScript::clear()
{
    // [b28c-L29] ORIG 0x08096a2c：首字段 store 为 mov $0x0,%edx; mov %edx,(%eax)
    // （常量经 edx 中转，P238 字段 float 形），后续字段直存。
    m_field00 = 0.0f;
    m_field04 = 0;
    m_field08 = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_vec18.clear();
    m_vec24.clear();
    m_field30 = 1;
    m_field34 = 1;
    m_field38 = 0x64;
    m_field3c = 0;
