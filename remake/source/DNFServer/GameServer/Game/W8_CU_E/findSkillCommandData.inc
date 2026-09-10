bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
{
    // b28d-35: DIFF→N82（帧/槽）。ORIG @0x08654b94 size 0x2d6。
    // [轴 B 自校·记 N] axisB_points.tsv 两行（0x8654d47 / 0x8654d4d）经地址自校确为
    //   **真分叉**：ORIG `cmpl $0x5,-0x10(%ebp); jg`、`cmpl $0x0,-0x10(%ebp); jg`
    //   （有符号序），我方同点为 `cmpl $0x5,-0x1c(%ebp); ja`、`…; jne`（无符号序）。
    //   但改循环变量符号性实测 **209→211（+2 劣化，E05 已复现并回滚）**——与具名
    //   局部 `key` 的 movzbl 链寄存器分配冲突 ⇒ 按「不劣才留」判 N，源码维持原形。
    // 命令表 getter（CUserCharacInfo +0x1254 区容器）：std::map<int,STSkillCommandData>*
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    std::map<int, STSkillCommandData>::iterator hit = cmds->find(skillId);
    if (hit != cmds->end())
    {
        out.m_command = hit->second.m_command;
        out.m_count   = hit->second.m_count;
        for (int i = 0; i < (signed char)hit->second.m_count; ++i)
        {
            // [P291 本函数回退：裸下标 `(&x.m_key0)[i]` 保留（m_keyBytes 视图
            //   在本函数 4 处访问点会被 4.4 拆成三步 lea，209→212）]
            // 实测（20:04）：同三参重载的「删 key 局部 + 重读 out」形在本函数
            // **劣化 209→214**（裸下标重读要重发 4 步 lea 链，与 m_keyBytes 视图
            // 的 3 步不同）⇒ 保持 key 具名局部。
            unsigned char key = (&hit->second.m_key0)[i];
            (&out.m_key0)[i]  = key;
            if (key == 7)
                (&out.m_key0)[i] = 8;
        }
        return true;
    }

    // 未定制：取当前职业技能列表（ORIG G_CDataManager()+0x10 → CSkillList 实例指针）
    int job = get_charac_job();
    __gnu_cxx::hash_map<int, CSkill*>* list =
        G_CDataManager()->m_skillList->getSkillList(job);
    if (list == 0)
        return false;

    __gnu_cxx::hash_map<int, CSkill*>::iterator sit = list->find(skillId);
    if (sit == list->end())
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x2113, 5);
        trace("SKILL_COMMAND_LOG : KEY INDEX DUPLICATE!!");
        return false;
    }

    CSkill* skill = sit->second;          // node.first=键（与 skillId 相同）、second=CSkill*
    size_t n = skill->m_keyCommandList.size();
    if (!(n < 6) || n == 0)
    {
        if (n != 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x2102, 5);
            trace("SKILL_COMMAND_LOG : ABNOMAL COMMAND CNT!! (%d) (%d)",
                  (int)sit->first, (int)n);
        }
        return false;
    }

    out.m_command = (unsigned short)sit->first;
    out.m_count   = (unsigned char)n;
    // [轴 B 实测·本函数不适用] ORIG 0x08654e19 虽为 `cmp -0x10(%ebp),%eax; setl`
    // 有符号序（P196 前置通过），但本函数改 `int i; i < (int)n` 实测
    // **209→211 劣化**（`unsigned char key` 具名局部的 `movzbl` 链与 int 循环
    // 的寄存器分配冲突）⇒ 回退 `size_t`；残差归 N82/N93（槽向 + getSkillCommand
    // 具名缓存不可去），已按 P196/S1「一次旋钮、无改善即回滚」留痕。
    for (size_t i = 0; i < n; ++i)
    {
        // ORIG 读第 i 个 STKeyCommandUnit.m_ids[0] 低字节作命令键位
        int v = skill->m_keyCommandList[i].m_ids[0];
        (&out.m_key0)[i] = (unsigned char)v;
        if ((unsigned char)v == 7)
            (&out.m_key0)[i] = 8;
    }
    return true;
