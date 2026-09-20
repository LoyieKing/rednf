void CTitleBook::getData(char* pData) const
{
    char* dst = pData;   // ORIG: 先将出参 pData 落 -0xc 局部槽再直传 (%esp)
    std::memcpy(dst, &m_book, sizeof(m_book));
}