void CAssaultPlaceFactory::FreeAssaultPlace(CAssaultPlace* place)  // ORIG 0x82f0f68
{
    m_pool.Free(place);
}