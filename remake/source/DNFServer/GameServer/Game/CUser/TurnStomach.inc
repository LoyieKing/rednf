void user_creature::CCreatureMgr::TurnStomach(bool fighting)
{
    if (m_equippedCreature != 0)
    {
        if (fighting)
        {
            if (m_equippedCreature->GetState() == 3)
                m_equippedCreature->SetState(m_user, (CREATURE_STATE)1);
        }
        else
        {
            if (m_equippedCreature->GetState() == 1)
                m_equippedCreature->SetState(m_user, (CREATURE_STATE)3);
        }
    }
