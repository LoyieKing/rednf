// ORIG T 0x82edb6a：map 摘除 + place->Reset() + 归还 StaticPool。
void CAssaultMgr::FreeAssaultPlace(int id, CAssaultPlace* place)
{
    // ORIG 0x82edb6a：id==0 的 je 直接落到 place 检查块（0x82edbde），
    // 无尾 jmp 岛 ⇒ 两个正向 if 块，非 early-return。
    if (id != 0)
    {
        std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
        if (it != m_places.end())
            m_places.erase(it);
    }
    if (place != 0)
    {
        place->Reset();
        GetInstanceAssaultPlaceFactory()->FreeAssaultPlace(place);
    }
}