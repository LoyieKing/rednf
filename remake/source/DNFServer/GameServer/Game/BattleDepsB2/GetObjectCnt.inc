// ORIG 0x0830ea0a：直接返回 m_objects.size()（call _ZNKSt3mapIi20DungeonRidableObject...4sizeEv）。
int CRidable::GetObjectCnt()
{
    return (int)m_objects.size();
}