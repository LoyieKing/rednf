// ORIG C1 0x83724f0 / FindCharacStat 0x8693c66：map.find(CharacterKind(job,grow))，
// 命中返回 &it->second，未命中返回 0（迭代器 != end 判定）。
CharacterStatus* FairPvPStatScript::FindCharacStat(int job, int growType)
{
    std::map<CharacterKind, CharacterStatus>::iterator it =
        m_map0.find(CharacterKind((unsigned char)job, (unsigned char)growType));
    if (it != m_map0.end())
        return &it->second;
    return 0;
}