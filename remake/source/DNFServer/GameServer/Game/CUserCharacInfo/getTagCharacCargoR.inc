const CCargo* CUserCharacInfo::getTagCharacCargoR() const
{
    // ORIG（0x14）读 m_tag（tag 角色），非 m_selected（CUR）
    return &((CharacInfoFields*)m_tag)->m_cargo;
}