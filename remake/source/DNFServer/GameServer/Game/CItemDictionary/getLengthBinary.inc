// ORIG 0x0811db60：最后一个非零字节下标+1（无非零字节返回 0）。
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
