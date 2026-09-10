CharacterStatus* FairPvPStatScript::FindCharacStat(int job, int growType)
{
    std::map<CharacterKind, CharacterStatus>::iterator it =
        m_map0.find(CharacterKind((unsigned char)job, (unsigned char)growType));
    if (it != m_map0.end())
        return &it->second;
    return 0;
