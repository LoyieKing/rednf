int GameWorld::get_gate_area(int vill) const
{
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return -1;
    }
    return m_pVill[vill].get_gate_area();
}