// ============================================================================
// 村庄 / 区域
// ============================================================================
Village* GameWorld::GetVillage(int vill)
{
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return NULL;
    }
    return &m_pVill[vill];
}