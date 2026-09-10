int CItemDictionary::getGainItemCount() const
{
    int count = 0;
    for (int i = 0; i < static_cast<int>(kBinaryLength); ++i)   // ORIG setle（有符号）
    {
        unsigned char byte = m_data[i];   // ORIG byte 物化局部（-0x9）
        for (int bit = 0; bit <= 7; ++bit)   // ORIG cmpl $0x7;setle（有符号 <=7）
        {
            if ((byte >> bit) & 1)
            {
                ++count;
            }
        }
    }
    return count;
}
