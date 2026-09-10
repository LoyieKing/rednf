void CharacManageScript::clear()
{
    // ORIG 0x088e0904：两 vector clear → +0x18/+0x30 map clear →
    // +0x48=0 → map<int,string> clear → +0x64 map clear（依 asm 顺序）。
    m_pointBonus.clear();
    m_manageLevelPoint.clear();
    m_expBonus.clear();
    m_goldBonus.clear();
    m_manageLevelMax = 0;
    m_basicExplain.clear();
    m_manageOption.clear();
