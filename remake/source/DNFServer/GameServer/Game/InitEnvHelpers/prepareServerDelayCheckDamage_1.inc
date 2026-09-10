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
