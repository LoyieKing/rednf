bool CCreature::IsEvolutionCreatureMulty(int creatureId)
{
    // ORIG `test; je 返false岛`：call 作 fall-through、return false 作岛（P196 极性）。
    if (m_script != 0)
        return m_script->isEvoluationCreature(creatureId);
    return false;
