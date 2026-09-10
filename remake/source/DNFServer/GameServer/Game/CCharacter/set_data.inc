void CCharacter::set_data(STCharacterScript* script)
{
    for (int i = 0; i <= 6; ++i)
    {
        if (i == 0)
        {
            m_baseInfo0.m_field00 = script->m_status[i].m_hpMax;
            m_baseInfo0.m_field04 = script->m_status[i].m_mpMax;
            m_baseInfo0.m_field08 = script->m_status[i].m_physAttack;
            m_baseInfo0.m_field0a = script->m_status[i].m_physDefense;
            m_baseInfo0.m_field0c = script->m_status[i].m_magAttack;
            m_baseInfo0.m_field0e = script->m_status[i].m_magDefense;
            for (int j = 0; j <= 3; ++j)
                m_baseInfo0.m_field10[j] = script->m_status[i].m_elemResist[j];
            for (int k = 0; k <= 16; ++k)
                m_baseInfo0.m_field18[k] = script->m_status[i].m_activeResist[k];
            m_baseInfo0.m_field3a = script->m_status[i].m_inventoryLimit;
            m_baseInfo0.m_field3e = script->m_status[i].m_hpRegen;
            m_baseInfo0.m_field40 = script->m_status[i].m_mpRegen;
            m_baseInfo0.m_field42 = script->m_status[i].m_moveSpeed;
            m_baseInfo0.m_field46 = script->m_status[i].m_attackSpeed;
            m_baseInfo0.m_field48 = script->m_status[i].m_castSpeed;
            m_baseInfo0.m_field4a = script->m_status[i].m_hitRecovery;
            m_baseInfo0.m_field4c = script->m_status[i].m_jumpPower;
            m_baseInfo0.m_field4e = script->m_status[i].m_weight;
            m_growTypeFlag[0] = (char)script->m_status[i].m_field52;
        }
        else
        {
            m_baseInfoGrow[i - 1].m_field00 = script->m_status[i].m_hpMax;
            m_baseInfoGrow[i - 1].m_field04 = script->m_status[i].m_mpMax;
            m_baseInfoGrow[i - 1].m_field08 = script->m_status[i].m_physAttack;
            m_baseInfoGrow[i - 1].m_field0a = script->m_status[i].m_physDefense;
            m_baseInfoGrow[i - 1].m_field0c = script->m_status[i].m_magAttack;
            m_baseInfoGrow[i - 1].m_field0e = script->m_status[i].m_magDefense;
            for (int j = 0; j <= 3; ++j)
                m_baseInfoGrow[i - 1].m_field10[j] = script->m_status[i].m_elemResist[j];
            for (int k = 0; k <= 16; ++k)
                m_baseInfoGrow[i - 1].m_field18[k] = script->m_status[i].m_activeResist[k];
            m_baseInfoGrow[i - 1].m_field3a = script->m_status[i].m_inventoryLimit;
            m_baseInfoGrow[i - 1].m_field3e = script->m_status[i].m_hpRegen;
            m_baseInfoGrow[i - 1].m_field40 = script->m_status[i].m_mpRegen;
            m_baseInfoGrow[i - 1].m_field42 = script->m_status[i].m_moveSpeed;
            m_baseInfoGrow[i - 1].m_field46 = script->m_status[i].m_attackSpeed;
            m_baseInfoGrow[i - 1].m_field48 = script->m_status[i].m_castSpeed;
            m_baseInfoGrow[i - 1].m_field4a = script->m_status[i].m_hitRecovery;
            m_baseInfoGrow[i - 1].m_field4c = script->m_status[i].m_jumpPower;
            m_baseInfoGrow[i - 1].m_field4e = script->m_status[i].m_weight;
            m_growTypeFlag[i] = (char)script->m_status[i].m_field52;
        }
        m_giveSkillList[i] = script->m_skillList[i];   // vector::operator=
    }

    for (int a = 0; a <= 6; ++a)
    {
        for (int b = 0; b <= 1; ++b)
        {
            m_secondGrowTypes[a].m_addInfo[b].m_field00 = script->m_secondGrow[a].m_statusA[b].m_hpMax;
            m_secondGrowTypes[a].m_addInfo[b].m_field04 = script->m_secondGrow[a].m_statusA[b].m_mpMax;
            m_secondGrowTypes[a].m_addInfo[b].m_field08 = script->m_secondGrow[a].m_statusA[b].m_physAttack;
            m_secondGrowTypes[a].m_addInfo[b].m_field0a = script->m_secondGrow[a].m_statusA[b].m_physDefense;
            m_secondGrowTypes[a].m_addInfo[b].m_field0c = script->m_secondGrow[a].m_statusA[b].m_magAttack;
            m_secondGrowTypes[a].m_addInfo[b].m_field0e = script->m_secondGrow[a].m_statusA[b].m_magDefense;
            for (int t = 0; t <= 3; ++t)
                m_secondGrowTypes[a].m_addInfo[b].m_field10[t] = script->m_secondGrow[a].m_statusA[b].m_elemResist[t];
            for (int k = 0; k <= 16; ++k)
                m_secondGrowTypes[a].m_addInfo[b].m_field18[k] = script->m_secondGrow[a].m_statusA[b].m_activeResist[k];
            m_secondGrowTypes[a].m_addInfo[b].m_field3a = script->m_secondGrow[a].m_statusA[b].m_inventoryLimit;
            m_secondGrowTypes[a].m_addInfo[b].m_field3e = script->m_secondGrow[a].m_statusA[b].m_hpRegen;
            m_secondGrowTypes[a].m_addInfo[b].m_field40 = script->m_secondGrow[a].m_statusA[b].m_mpRegen;
            m_secondGrowTypes[a].m_addInfo[b].m_field42 = script->m_secondGrow[a].m_statusA[b].m_moveSpeed;
            m_secondGrowTypes[a].m_addInfo[b].m_field46 = script->m_secondGrow[a].m_statusA[b].m_attackSpeed;
            m_secondGrowTypes[a].m_addInfo[b].m_field48 = script->m_secondGrow[a].m_statusA[b].m_castSpeed;
            m_secondGrowTypes[a].m_addInfo[b].m_field4a = script->m_secondGrow[a].m_statusA[b].m_hitRecovery;
            m_secondGrowTypes[a].m_addInfo[b].m_field4c = script->m_secondGrow[a].m_statusA[b].m_jumpPower;
            m_secondGrowTypes[a].m_addInfo[b].m_field4e = script->m_secondGrow[a].m_statusA[b].m_weight;
            // ORIG this + a*0xc0 + b + 0x350：m_fieldbc 的第 b 个字节（源 +0x52）
            reinterpret_cast<char*>(&m_secondGrowTypes[a].m_fieldbc)[b] =
                (char)script->m_secondGrow[a].m_statusA[b].m_field52;
            m_secondGrowTypes[a].m_skills[b] = script->m_secondGrow[a].m_skills[b];
        }
    }
