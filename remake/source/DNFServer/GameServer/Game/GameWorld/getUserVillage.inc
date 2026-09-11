Village* GameWorld::getUserVillage(CUser* user)
{
    if (user == NULL)
    {
        return NULL;
    }
    int vill = (int)(signed char)user->getCurCharacVill();
    if (vill < 1 || vill >= MAX_VILLAGE_NUM)
    {
        return NULL;
    }
    return &m_pVill[vill];
}