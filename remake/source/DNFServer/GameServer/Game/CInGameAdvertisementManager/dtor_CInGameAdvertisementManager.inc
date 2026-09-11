// ORIG W _ZN27CInGameAdvertisementManagerD1Ev @0x080fca7c：delete m_rand(+0x18)
// → multimap 析构（ORIG 无 m_rand 置零）。
CInGameAdvertisementManager::~CInGameAdvertisementManager()
{
    delete m_rand;
}