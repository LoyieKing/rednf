int CUser::getTagCharacInfo(int param_1, unsigned int characNo, TAG_CHARAC_DATA* out) const
{
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    // b28d-solo-CUser p167：ORIG 0x086904f0 每字段重调 `vector::operator[](uint) const`
    //   （共 8 次；我方缓存 `const _Charac_info& src` 仅 1 次，MISSING ×7）⇒
    //   去缓存逐字段访问（P157/P183）。
    if ((unsigned int)m_characList[param_1].m_characNo != characNo)
        return 0;
    out->m_characNo = m_characList[param_1].m_characNo;
    strncpy(out->m_name, m_characList[param_1].m_name, 0x1d);
    out->m_field21 = m_characList[param_1].m_field26;   // job（ORIG movzbl 读）
    out->m_field22 = (unsigned short)m_characList[param_1].m_level;  // ORIG movzwl 0x27
    out->m_field24 = m_characList[param_1].m_field29.m_growthType;
    out->m_field25 = m_characList[param_1].m_field1239;
    memcpy(&out->m_add, &m_characList[param_1].m_additioalInfo, 0x52);
    return 1;
