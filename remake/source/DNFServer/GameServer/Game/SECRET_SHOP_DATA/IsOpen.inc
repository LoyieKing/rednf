int SECRET_SHOP_DATA::IsOpen()
{
    // ORIG: 两个 `je` 共指 return-0 岛（field==0 / ==1000），return-1 作 fall-through。
    if (m_openCode != 0 && m_openCode != 1000)
    {
        return 1;
    }
    return 0;
