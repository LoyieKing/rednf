int DisPatcher_BuySkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);            // ORIG -0x1c(%ebp)
    if (errNo >= 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f, (unsigned char)errNo);
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x3ef,
                        __PRETTY_FUNCTION__,
                        errNo, user->get_acc_id());
    }

    // ORIG 在错误分支之后才把 param/msg 存入局部（-0x18/-0x14），声明位置保持一致
    stOnBuySkillParam& out = static_cast<stOnBuySkillParam&>(param);
    MSG_BUY_SKILL& req = *reinterpret_cast<MSG_BUY_SKILL*>(&msg);
    out.m_355 = req.m_count;
    if (req.m_count == 1)
    {   // 单条模式（只读 entry0；ORIG 不经 backupSkill）
        out.m_infos[0].m_0 = req.m_entries[0].m_skillIdx;
        out.m_infos[0].m_1 = req.m_entries[0].m_level;
        if (req.m_entries[0].m_refundFlag == 0)
        {
            master_new_skill_process(user, (char)req.m_treeKind, out.m_infos[0]);
        }
        else
        {
            refund_skill_process(user, (char)req.m_treeKind, out.m_infos[0]);
        }
        out.m_354 = out.m_infos[0].m_0;
        out.m_350 = out.m_infos[0].m_4;
    }
    else
    {   // 批量模式：先备份当前技能树
        ((SkillSlot*)user->getCurCharacSkillW())->set_parent(user);
        if (!((SkillSlot*)user->getCurCharacSkillW())
                 ->backupSkill((ENUM_SKILL_TREE_KIND)(signed char)req.m_treeKind))
        {   // backupSkill 失败：ORIG 读未初始化的 infos[0].m_0 写 m_354 —— 忠实转写
            out.m_350 = -1;
            out.m_354 = out.m_infos[0].m_0;
            return 0;
        }
        // ORIG 两遍循环各占独立槽（-0x10/-0xc，Ghidra local_14/local_10）：
        // O0 无槽共享，必须两个不同变量名。
        for (int i = 0; (signed char)req.m_count > i; ++i)
        {   // 第一遍：try_* 试算
            out.m_infos[i].m_0 = req.m_entries[i].m_skillIdx;
            out.m_infos[i].m_1 = req.m_entries[i].m_level;
            if (req.m_entries[i].m_refundFlag == 0)
            {
                try_master_new_skills_process(user, (char)req.m_treeKind, out.m_infos[i]);
            }
            else
            {
                try_refund_skills_process(user, (char)req.m_treeKind, out.m_infos[i]);
            }
            out.m_354 = out.m_infos[i].m_0;
            out.m_350 = out.m_infos[i].m_4;
            if (out.m_infos[i].m_4 < 0)
            {
                return 0;
            }
        }
        for (int j = 0; (signed char)req.m_count > j; ++j)
        {   // 第二遍：正式执行
            out.m_infos[j].m_0 = req.m_entries[j].m_skillIdx;
            out.m_infos[j].m_1 = req.m_entries[j].m_level;
            if (req.m_entries[j].m_refundFlag == 0)
            {
                master_new_skill_process(user, (char)req.m_treeKind, out.m_infos[j]);
            }
            else
            {
                refund_skill_process(user, (char)req.m_treeKind, out.m_infos[j]);
            }
            out.m_354 = out.m_infos[j].m_0;
            out.m_350 = out.m_infos[j].m_4;
            if (out.m_infos[j].m_4 < 0)
            {
                return 0;
            }
        }
    }
    return errNo;   // 该路径 errNo==0，与 ORIG `mov -0x1c(%ebp),%eax` 一致
