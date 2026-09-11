// [b14 本波·AE 归零] 三条真缺口（旧判词「N85 x87 除法 lowering 封顶」作废）：
//  ①`(double)field` → `(float)field`：4.4 对 double 常量发 `fldl <A>`，ORIG 是
//    `flds <A>`（float 常量）；且 `(float)unsigned int`（+0x0/+0x4/+0x42）走
//    `mov $0,%edx; … fildll` 零扩展形、`(float)int`（+0x3a/+0x4e）走 `fildl`
//    —— 与 ORIG 逐段同形（探针 /tmp/b14_probe2.cpp pA/pB/pC/pE）。
//  ②elemResist(+0x10..+0x16) 与 activeResist(+0x18..) 需 `(float)(short)field`：
//    ORIG 发 `movzwl; mov %ax,-0x1e; filds -0x1e`（16 位载入，共 5 处），直写
//    `(float)field` 只给 32 位 `fildl`（探针 pC 复证）。
//  ③m_hpRegen/m_mpRegen/m_hitRecovery/m_jumpPower(+0x3e/+0x40/+0x4a/+0x4c) 同为
//    filds 形，各加 `(short)`。
// 终态 orig=331 our=331 gap=0 ndiff=0。
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
            // [PoolB b28f] 共享 ORIG 表（定义见 STMonsterScript.cpp；ORIG 0x08a0fdc2
            // `mov 0x93f5480(,%eax,4),%eax` 同一实体）。
            fprintf(fp, "%s %.1f\n", ACTIVESTATUS_TAG[i],
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
}
