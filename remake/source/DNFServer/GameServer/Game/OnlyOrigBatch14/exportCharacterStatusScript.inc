void exportCharacterStatusScript(FILE* fp, STRDCharacterStatus* st)
{
    if (st->m_hpMax != 0)
        fprintf(fp, "[HP MAX] %.1f\n", (float)st->m_hpMax / 10.0f);
    if (st->m_mpMax != 0)
        fprintf(fp, "[MP MAX] %.1f\n", (float)st->m_mpMax / 10.0f);
    if (st->m_physAttack != 0)
        fprintf(fp, "[physical attack] %.1f\n", (float)st->m_physAttack / 10.0f);
    if (st->m_physDefense != 0)
        fprintf(fp, "[physical defense] %.1f\n", (float)st->m_physDefense / 10.0f);
    if (st->m_magAttack != 0)
        fprintf(fp, "[magical attack] %.1f\n", (float)st->m_magAttack / 10.0f);
    if (st->m_magDefense != 0)
        fprintf(fp, "[magical defense] %.1f\n", (float)st->m_magDefense / 10.0f);
    if (st->m_elemResist[0] != 0)
        fprintf(fp, "[fire resistance] %.1f\n", (float)(short)st->m_elemResist[0] / 10.0f);
    if (st->m_elemResist[1] != 0)
        fprintf(fp, "[water resistance] %.1f\n", (float)(short)st->m_elemResist[1] / 10.0f);
    if (st->m_elemResist[2] != 0)
        fprintf(fp, "[dark resistance] %.1f\n", (float)(short)st->m_elemResist[2] / 10.0f);
    if (st->m_elemResist[3] != 0)
        fprintf(fp, "[light resistance] %.1f\n", (float)(short)st->m_elemResist[3] / 10.0f);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (st->m_activeResist[i] != 0)
        {
            fprintf(fp, "%s %.1f\n", g_activeResistName[i],
                    (float)(short)st->m_activeResist[i] / 10.0f);
        }
    }
    if (st->m_inventoryLimit != 0)
        fprintf(fp, "[inventory limit] %.1f\n", (float)st->m_inventoryLimit / 10.0f);
    if (st->m_hpRegen != 0)
        fprintf(fp, "[HP regen speed] %.1f\n", (float)(short)st->m_hpRegen / 10.0f);
    if (st->m_mpRegen != 0)
        fprintf(fp, "[MP regen speed] %.1f\n", (float)(short)st->m_mpRegen / 10.0f);
    if (st->m_moveSpeed != 0)
        fprintf(fp, "[move speed] %.1f\n", (float)st->m_moveSpeed / 10.0f);
    if (st->m_attackSpeed != 0)
        fprintf(fp, "[attack speed] %.1f\n", (float)st->m_attackSpeed / 10.0f);
    if (st->m_castSpeed != 0)
        fprintf(fp, "[cast speed] %.1f\n", (float)st->m_castSpeed / 10.0f);
    if (st->m_hitRecovery != 0)
        fprintf(fp, "[hit recovery] %.1f\n", (float)(short)st->m_hitRecovery / 10.0f);
    if (st->m_jumpPower != 0)
        fprintf(fp, "[jump power] %.1f\n", (float)(short)st->m_jumpPower / 10.0f);
    if (st->m_weight != 0)
        fprintf(fp, "[weight] %.1f\n", (float)st->m_weight / 10.0f);
