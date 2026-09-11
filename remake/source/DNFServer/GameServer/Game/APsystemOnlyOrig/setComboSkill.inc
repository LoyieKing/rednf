// ============================================================================
// DB_UpdateComboSkill::setComboSkill —— ORIG 0x0844aa4c（报告
// docs/class_func_reports/DB_UpdateComboSkill/setComboSkill.md 全文）
// ============================================================================
// db/data 任一空 → 0。kind 非 0/1 → cMyTrace(0xbb42) 返 0。循环 i=0..<count
// 且 i<6（comoboSkillData 0xe 字节/条）：逐 short 读（+0x00 combo_idx、
// +0x02..+0x0c value1..6）→ upDate %s set value1..6 where charac_no=%u and
// combo_idx=%d（表名 getComboSkillTableName(kind)）；exec 失败返 0；
// getAffectedRowCount()==0 → inSert %s(charac_no,combo_idx,value1..6)
// values(%u,%d,...)，exec 失败且 getDBError()!=0x426 → 返 0。全过返 1。
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
bool DB_UpdateComboSkill::setComboSkill(MySQL* db, unsigned int characNo,
                                        unsigned char count,
                                        comoboSkillData* data,
                                        ENUM_SKILL_TREE_KIND kind)
{
    if (db == 0 || data == 0)
        return 0;
    if (!(kind == 0 || kind == 1))
    {
        // ORIG 0x0844a8xx：`mov 0x1c(%ebp),%ebx` 前置缓存 kind，跨 ctor 后
        // `mov %ebx,0x8(%esp)`（P87；类型 int 与 ENUM 底层一致，单条直落）。
        register int k = kind;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbb42, 5);
        trace("DB_UpdateComboSkill::setComboSkill kind(%d) error characNo(%u)",
              k, characNo);
        return 0;
    }
    int i = 0;
    while (i < (int)(count & 0xff) && i < 6)
    {
        // ORIG 每字段独立重算 i*0xe+data（mov -0x1c; add; lea 8x; sub; add
        // 0x18），故不用指针缓存，逐 data[i] 访问（O0 无 CSE 每次重算）。
        db->set_query(
            "upDate %s set value1 = %d, value2 = %d, value3 = %d, value4 = %d, value5 = %d, value6 = %d where charac_no = %u and combo_idx = %d",
            getComboSkillTableName(kind), (int)data[i].m_comboSkillList[0],
            (int)data[i].m_comboSkillList[1], (int)data[i].m_comboSkillList[2],
            (int)data[i].m_comboSkillList[3], (int)data[i].m_comboSkillList[4],
            (int)data[i].m_comboSkillList[5], characNo,
            (int)data[i].m_comboSkillIndex);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into %s(charac_no, combo_idx, value1, value2, value3, value4, value5, value6) values(%u, %d, %d, %d, %d, %d, %d, %d)",
                getComboSkillTableName(kind), characNo,
                (int)data[i].m_comboSkillIndex, (int)data[i].m_comboSkillList[0],
                (int)data[i].m_comboSkillList[1], (int)data[i].m_comboSkillList[2],
                (int)data[i].m_comboSkillList[3], (int)data[i].m_comboSkillList[4],
                (int)data[i].m_comboSkillList[5]);
            // ORIG 0x844ad74-0x844ad85 `cmp $0x426; sete %al; test; jne
            // <continue>; mov $0,%eax; jmp end` —— (bool) 物化 + **空 then +
            // else return** 才保 sete+jne；`!(bool)` 形被 4.4 fold 成
            // setne+je（探针 /tmp/w8lt_set3.o::t17 全等）。
            if (db->exec(true) != 1)
                if ((bool)(db->getDBError() == 0x426))
                    ;
                else
                    return 0;
        }
        ++i;
    }
    return 1;
}