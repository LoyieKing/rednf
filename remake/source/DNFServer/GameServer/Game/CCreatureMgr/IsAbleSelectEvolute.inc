// ORIG _ZN13user_creature9CCreature19IsAbleSelectEvoluteEP5CUser T 0x08337d58：
// 同 IsAbleEvolute，但等级门槛取 GetTypeSelectEvolutionLevel()（-1 视为不可）。
bool CCreature::IsAbleSelectEvolute(CUser* user)
{
    if (m_script == 0)
        return 0;
    // ORIG 0x8337d7d/0x8337d9f：两次独立 call（无 level 缓存变量）；比较结果
    // sete/setg 物化 + test;je（技巧 9/22），非直接 jne/jle 分支。
    if ((bool)(m_script->GetTypeSelectEvolutionLevel() == -1))
        return 0;
    if ((bool)(m_script->GetTypeSelectEvolutionLevel() > m_level))
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
