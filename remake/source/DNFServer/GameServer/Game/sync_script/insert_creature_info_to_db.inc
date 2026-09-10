bool CSyncScript::insert_creature_info_to_db(unsigned int id,
                                             const CreatureScript* script,
                                             const char* param3,
                                             const char* param4)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；487=487）。
    //   归位要点：artifactSlot 循环改具名数组成员视图 `script->m_artifactSlots[i]`
    //   （P291/P311：裸 `(&m_id)[20+i]` 在 4.4 恒 shl+add 三段，具名数组产 scaled
    //   disp 折叠）。ORIG @0x08624414。
    if (m_field0 != 1)
    {
        return true;
    }
    static const int BUFSIZE = 0x100;  // ORIG _ZZN11sync_script11CSyncScript26insert_creature_info_to_db…E7BUFSIZE
    // ORIG：artifact_slot 列 = '0'+(R/G/B 三槽标志) 的 3 字符串
    // （CreatureScript+0x50/0x54/0x58，asm 0x08624437..0x0862445d，
    // 低字节存 (v+'0')&0xff；缓冲 movl$0 预清零即 NUL 结尾）。
    // ORIG local_5c 为 `movl $0x0` 单指令清零（聚合初始化），不是 memset 调用。
    char artifactSlot[4] = {0};
    // ORIG：直接按 +0x50+i*4 读三槽标志（无中间数组）；上界写成 sizeof-1
    //（ORIG 0x0862444f 为 `mov i,%eax; cmp $0x2,%eax; setbe`：有符号 i 与
    // size_t 上界比较产生的转换临时，非 `cmpl $imm,mem` 直比形态）。
    for (int i = 0; i < sizeof(artifactSlot) - 1; ++i)
    {
        // ORIG 0x0862445d `add $0x14,%ecx; mov (%edx,%ecx,4),%edx`：以结构首
        // 字段 m_id(+0x00) 为基的 int 索引 [20+i]（= +0x50 槽区）。从
        // &m_artifactSlotRed 起算会被 4.4 折成 lea 0x50+shl 两段（N18 向）。
        artifactSlot[i] = (char)(script->m_artifactSlots[i] + '0');
    }
    // ORIG 0x0862448c/0x08624562/0x08624635：三处均为
    //   CStringMaker::CStringMaker(临时 functor)
    //   → std::for_each<__normal_iterator<int const*,vector<int>>, CStringMaker>
    //   → CStringMaker::operator std::string()（sret 直接产出结果串）
    //   → ~CStringMaker ×2
    // 即 ORIG 无 c_str() → std::string(const char*, allocator) 的二次构造；
    // 走 makeString(...).c_str() 会多出 _ZNSaIcEC1Ev ×3 / _ZNSsC1EPKcRKSaIcE ×3
    // （及 _ZNSaIcED1Ev ×6）这些 ORIG 不存在的 eager 构造调用点。
    const std::string piercingStr =
        std::for_each(script->m_piercing.begin(), script->m_piercing.end(),
                      CStringMaker());
    const std::string skillLevelValuesStr =
        std::for_each(script->m_skillLevelValues.begin(),
                      script->m_skillLevelValues.end(), CStringMaker());
    const std::string overSkillLevelValuesStr =
        std::for_each(script->m_overSkillLevelValues.begin(),
                      script->m_overSkillLevelValues.end(), CStringMaker());
    // ORIG：values 区 11 个 dword 就地清零（rep stos 0xb），非 memset 调用。
    const char* values[11] = {0};
    values[0] = script->m_name.c_str();
    values[1] = artifactSlot;
    values[2] = param3;
    values[3] = param4;
    values[4] = piercingStr.c_str();
    // ORIG：skill_name/skill_desc/overskill_name/overskill_desc ←
    // m_skillString（vector<string> @+0x128）前 4 项，越界项用 ""；
    // ORIG 为 4 段展开（size() ×4 + at() ×4），非循环 + operator[]。
    values[5] = script->m_skillString.size() == 0
                    ? "" : script->m_skillString.at(0).c_str();
    values[6] = script->m_skillString.size() < 2
                    ? "" : script->m_skillString.at(1).c_str();
    values[7] = script->m_skillString.size() < 3
                    ? "" : script->m_skillString.at(2).c_str();
    values[8] = script->m_skillString.size() < 4
                    ? "" : script->m_skillString.at(3).c_str();
    values[9] = skillLevelValuesStr.c_str();
    values[10] = overSkillLevelValuesStr.c_str();
    // ORIG：escaped 区 0x2c0 个 dword 就地清零（= 11*0x100 字节），非 memset 调用；
    // GetDBHandle 在清零之后才取。
    char escaped[11][BUFSIZE] = {{0}};
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG 0x08624a0e：同为 `mov i,%eax; cmp $0xa,%eax; setbe`（size_t 上界）。
    for (int i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    {
        GetEscapeString(db, values[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_creature_info(it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level) values(%u, %d, '%s', %d, %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d)",
            id, script->m_id, escaped[0], script->m_field1c,
            script->m_field20, escaped[1], script->m_learnOverSkillLevel,
            escaped[2], escaped[3], escaped[4], escaped[5], escaped[6],
            escaped[7], escaped[8], escaped[9], escaped[10],
            script->m_evolutionCreatureId, script->m_evolutionLevel))
    {
        return false;
    }
    // ORIG 0x08624a5f：exec 结果经 `xor $1 / test` 分支回填 0/1，
    // 而非 `return db->exec(true)` 的直接搬运。
    if (!db->exec(true))
    {
        return false;
    }
    return true;
