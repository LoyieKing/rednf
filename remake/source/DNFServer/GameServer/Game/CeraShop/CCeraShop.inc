WongWork::CCeraShop::CCeraShop()
{
    // ORIG 0x83202b8：new SpecialItemHandler(0x74) 存 +0；+4 置 0。
    m_handler = new SpecialItemHandler(this);
    m_originalItemIdx = 0;
}