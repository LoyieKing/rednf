void* CMonster::getMonsterScript() const
{
    return const_cast<STMonsterScript*>(&m_script);
}