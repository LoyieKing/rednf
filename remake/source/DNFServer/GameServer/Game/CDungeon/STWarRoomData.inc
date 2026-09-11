STWarRoomData::STWarRoomData()
{
    // ORIG C1/C2 0x0836a1f6：构造 m_vec14[4]（+0x14）/m_vec44（+0x44）/
    // m_vec6c[4]（+0x6c）/m_spawnProb（+0xb0）后调用 clear()。
    clear();
}