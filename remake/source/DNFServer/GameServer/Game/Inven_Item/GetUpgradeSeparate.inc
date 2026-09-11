unsigned char UpgradeSeparateInfo::GetUpgradeSeparate() const
{
    return m_field & 0x1f;
}