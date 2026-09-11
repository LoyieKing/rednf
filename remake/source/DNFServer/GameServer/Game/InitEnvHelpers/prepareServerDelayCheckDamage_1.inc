// ===========================================================================
// prepareServerDelayCheckDamage  ORIG T 0x0809510e
// 栈上构造 CreatureCommonScript → importCreatureCommonScript(
// "Creature/Common/creatureCommon.cre", &g_creature_damage_vector_) 失败→false
// → importEtcParameterScriptForServer() 失败→false → true。
// ===========================================================================
bool prepareServerDelayCheckDamage(void)
{
    CreatureCommonScript script;

    if (!importCreatureCommonScript(&script,
                                    "Creature/Common/creatureCommon.cre",
                                    &g_creature_damage_vector_))
        return false;

    if (!importEtcParameterScriptForServer())
        return false;

    return true;
}