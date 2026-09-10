bool CCreature::IsAbleEvolute(CUser* user) const
{
    if (m_script == 0)
        return 0;
    // ORIG 0x8338121/0x8338142：两次独立 GetEvolutionLevel call，==0 用
    // test;sete;test;je（0x8338126），> 用 cmp %edx,%eax;setg;test;je（0x833814d）。
    if ((bool)(m_script->GetEvolutionLevel() == 0))
        return 0;
    if ((bool)(m_script->GetEvolutionLevel() > m_level))
        return 0;
    // ORIG 0x8337dcb：`test;je e3b(mov1 尾岛)`——IsExist true 才 fall user 检查，
    // return 1 为尾岛（`if(!call())return 1` 反产 xor;mov1;jmp 物化岛）。
    if (m_script->IsExistEvolutionQuest())
    {
        if (user != 0)
        {
            PacketGuard packet;
            user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&packet));
            user->Send(packet);
        }
        return 0;
    }
    return 1;
}
