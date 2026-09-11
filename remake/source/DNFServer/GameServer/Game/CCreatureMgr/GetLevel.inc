// ORIG GetLevel @ 0x833f09a：+0x2c。
// 定义点挪至 CCreatureMgr::GetEquipedCreatureLevel 之后（同 GetName 帧塌缩原因）。
int CCreature::GetLevel()
{
    return m_level;
}
