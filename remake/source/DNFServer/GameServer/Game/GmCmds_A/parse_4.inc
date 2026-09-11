// ORIG parse 0x084b22ca：三标记清零、token 计数清零，trim 后整串交给 getParam。
void BlueMarbleSet::parse()
{
    m_flag1 = 0;
    m_flag2 = 0;
    m_flag0 = 0;
    m_param = 0;

    trim(m_parser->m_args);
    this->getParam(m_parser->m_args);
}