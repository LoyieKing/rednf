void CBattle_Field::SetAssignNPCByPotion(bool flag)
{
    // ORIG：无 cur 局部（mapInfo eax 直通），bool 形参直读形参槽。
    GetCurrentMapInfo()->m_assignNPCByPotion = flag;
