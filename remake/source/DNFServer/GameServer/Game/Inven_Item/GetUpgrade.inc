unsigned char Inven_Item::GetUpgrade() const
{
    return m_attr & 0x1f;
}