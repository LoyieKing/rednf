Village* GameWorld::getUserVillage(int vill)
{
    if (vill < 1 || vill >= MAX_VILLAGE_NUM)
    {
        return NULL;
    }
    return &m_pVill[vill];
}