// CVillageObjectMgr ctor/dtor 现由 CVillageObjectMgr.cpp 提供
//（ORIG 0x86b6f30 默认构造 std::map；dtor 0x86b6f44 调用 map dtor）。
// CVillageMonsterMgr ctor/dtor 现由 CVillageMonsterMgr.cpp 提供（LinkVillageSolo）。
village_attacked::CRevengeDungeon::CRevengeDungeon()
{
    // ORIG 0x82aa6a4：+0 字节清零 + +0x04 int 清零。
    m_isOpen = false;
    m_field04 = 0;
}