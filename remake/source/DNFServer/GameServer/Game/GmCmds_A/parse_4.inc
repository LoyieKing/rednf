void BlueMarbleSet::parse()
{
    m_flag1 = 0;
    m_flag2 = 0;
    m_flag0 = 0;
    m_param = 0;

    trim(m_parser->m_args);
    this->getParam(m_parser->m_args);
