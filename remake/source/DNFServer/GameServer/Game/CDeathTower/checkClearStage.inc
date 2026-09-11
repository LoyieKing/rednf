bool CDeathTower::CStage::checkClearStage()
{
    // [b28d-solo-CDeathTower] ORIG 0x8461b3e：局部 map_monster（默认构造 local_360，
    // 带 EH cleanup）+ 每轮 operator= 拷贝（p167：C1 ×1 / operator= ×1 / D1 ×2 /
    // _Unwind_Resume ×1 实证）；m_dropCnt(+0x2c)==100 且 m_field13(+0x13)==0 → false。
    map_monster mob;
    for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
         it != m_mapInfo.m_monsterMap.end(); ++it)
    {
        mob = it->second;
        if (mob.m_dropCnt == 100 && mob.m_field13 == 0)
        {
            return false;
        }
    }
    return true;
}