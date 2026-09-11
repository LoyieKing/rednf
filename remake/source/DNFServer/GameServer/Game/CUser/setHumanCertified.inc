void CUser::setHumanCertified(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    *(bool*)((char*)&m_humanCertified) = flag;
}