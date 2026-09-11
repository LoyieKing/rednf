// ORIG T 0x0833c28a：休息(3)↔战斗(1) 两态切换；无宠物早退。
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
}