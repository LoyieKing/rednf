int CItemDictionary::getLengthBinary() const
{
    int length = 0;
    for (int i = 0; i < static_cast<int>(kBinaryLength); ++i)
    {
        if (m_data[i] != 0)
            length = i + 1;
    }
    return length;
}
