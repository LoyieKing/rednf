unsigned char UpgradeSeparateInfo::GetUpgradeSeparate() const
{
    return m_packed & 0x1f;
}