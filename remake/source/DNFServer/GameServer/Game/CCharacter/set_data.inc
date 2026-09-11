// ORIG CCharacter::set_data @ 0x08348894（objdump 全量转写，2901B / 932 条）。
// §6.2 超大函数（932 insns）验收：调用点差集 ZERO；gap 43→16（1.7%），残余为
// 4.1 常量拆分二段 `add $0x50/add $0x2` 寻址 vs 4.4 `lea disp(,%e)` 折叠
// （ORIG-only add 58 / OURS-only lea 46 净额，N82/P183 同族机器码折叠差）
// ⇒ 判语义完成、byte 层 N 封顶。
// ORIG 结构为 if(i==0)/else **双展开体**，两支各写一整套 24 条赋值，不共享缓存：
//   读侧两支统一为 script->m_status[i]（ORIG 每次读都重算 script+i*0x57+0x24；
//   i=0 落 m_initialStatus，i=1..6 落 m_growStatus[i-1]，布局见 STCharacterScript.h）；
//   写侧 i==0 支为常量偏移 this+0x00 一整套（m_baseInfo[0] / m_growTypeFlag[0]），
//   else 支为 m_baseInfo[i] / m_growTypeFlag[i]（ORIG 写侧算式为
//   (i-1)*0x52+0x52 与 (i-1)+0x7d5，地址等价；本工程 g++4.4.7 会把
//   (i-1)*0x52+0x52 常量折叠成 i*0x52，两种写法生成码逐字节相同，
//   故取无 UB 的下标形式）。
//   两支之后共用 m_giveSkillList[i] = script->m_skillList[i]（vector::operator=）。
// 第二成长域（a=0..6 × b=0..1）同样逐字段重算地址、不缓存引用：
//   dst = m_secondGrowTypes[a].m_addInfo[b]（this + 0x294 + a*0xc0 + b*0x52），
//   src = script->m_secondGrow[a].m_statusA[b]（script + 0x3d0 + a*0x100 + b*0x57），
//   resist 两段循环按具名字段寻址即与 ORIG 逐字节一致（+0x2a4 / +0x2ac），
//   尾字节写 ((char*)&m_secondGrowTypes[a].m_fieldbc)[b]（ORIG +0x350+a*0xc0+b），
//   末尾 m_secondGrowTypes[a].m_skills[b] = script->m_secondGrow[a].m_skills[b]。
// [b28d-7 N82] ORIG 0x08348894：帧 $0x5c + `imul $0x57` 后 add 两步寻址（-0x3c 先载），4.4 帧 $0x4c + lea 折叠；P167 CLEAR ⇒ 封顶。
void CCharacter::set_data(STCharacterScript* script)
{
    for (int i = 0; i <= 6; ++i)
    {
        if (i == 0)
        {
            // [b28c-L15 P211 类型旋钮] 与 RHS 同型化（union 同址成员 m_maxHP/
            //   m_maxMP）：ORIG 0x083488c2/0x083488d8 为 `mov (%eax),%edx` 单段
            //   直落；写 int 视角 m_field00（int ← unsigned int 异型）令 4.4 多
            //   一条 `mov (%eax),%eax; mov %eax,%edx` 中转（探针
            //   /tmp/r5/n5/BossCharac/p1.cpp v1 异型 / v2 同型对照）。
            m_baseInfo0.m_maxHP = script->m_status[i].m_hpMax;
            m_baseInfo0.m_maxMP = script->m_status[i].m_mpMax;
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
            // [b28c-L15 P211] 同上：ORIG 0x08348a2e `mov 0x6(%eax),%edx` 单段。
            m_baseInfo0.m_moveSpeed = script->m_status[i].m_moveSpeed;
            m_baseInfo0.m_field46 = script->m_status[i].m_attackSpeed;
            m_baseInfo0.m_field48 = script->m_status[i].m_castSpeed;
            m_baseInfo0.m_field4a = script->m_status[i].m_hitRecovery;
            m_baseInfo0.m_field4c = script->m_status[i].m_jumpPower;
            m_baseInfo0.m_field4e = script->m_status[i].m_weight;
            // [b28c-L15 P211] ORIG 0x08348ac0 `movzbl 0x6(%eax),%edx` 单段直存；
            //   char 视角 + (char) cast 令 4.4 多一条 mov %eax,%edx 中转 ⇒ 用同址
            //   uchar 视角按同型写（探针 p1.cpp v3 异型 / v4 同型）。
            m_growTypeFlagU[0] = script->m_status[i].m_field52;
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
            //   [b28c-L15 P211] ORIG 0x08349328..0x08349360 = 源字节先入 ecx、地址
            //   后算、`mov %cl,0x10(%eax)` 直存无中转；reinterpret_cast<char*> 地址
            //   视图令 4.4 多 1 条 mov %eax,%edx 且装载序翻反 ⇒ 改用同址 uchar 数组
            //   视角按元素同型写（真实头探针 probe_sd_f2d 与 ORIG 该段逐条同形）。
            m_secondGrowTypes[a].m_fieldbcBytes[b] =
                script->m_secondGrow[a].m_statusA[b].m_field52;
            m_secondGrowTypes[a].m_skills[b] = script->m_secondGrow[a].m_skills[b];
        }
    }
}