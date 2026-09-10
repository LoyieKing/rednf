int CCreature::GetStomach()
{
    return (int)m_stomach.GetStomachValue((CREATURE_STATE)m_state,
                                          m_isGrow);
}
