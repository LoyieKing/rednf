CCompound_ExtreamDun_Item::CCompound_ExtreamDun_Item()
{
    const unsigned long seed = time(0);
    register const unsigned long* pseed = &seed;
    m_rand = new CMTRand(*pseed);
