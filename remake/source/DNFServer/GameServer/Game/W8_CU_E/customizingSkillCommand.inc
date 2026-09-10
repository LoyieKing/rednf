int CUser::customizingSkillCommand(STSkillCommandData& data)
{
    // b28d-35: DIFF→N82/N7（帧 0x54 vs 0x58；寄存器分配差）。ORIG @0x08654f74。
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    // 先还原 ORIG 的占位键 0x8 → 0x7，再以上下文键寻位
    for (int i = 0; i < (signed char)data.m_count; ++i)
    {
        // [L23-r2 P291] 同族：具名数组视角 `m_keyBytes[5]`（ORIG 单条 disp+scaled）。
        if (data.m_keyBytes[i] == 8)
            data.m_keyBytes[i] = 7;
    }

    int key = (int)(short)data.m_command;    // ORIG cwtl 有符号扩展
    std::map<int, STSkillCommandData>::iterator it = cmds->find(key);
    if (it != cmds->end())
    {
        it->second.m_command = data.m_command;
        it->second.m_count   = data.m_count;
        for (int i = 0; i < (signed char)data.m_count; ++i)
            it->second.m_keyBytes[i] = data.m_keyBytes[i];
    }
    else
    {
        // ORIG 0x086550?? make_pair 实参为 **data.m_command 左值**（mangled
        // `_ZSt9make_pairIRsR18STSkillCommandDataE` = make_pair<short&, T&>；
        // arg1 直接传 data 指针 = &data.m_command，因 m_command 偏移 0）
        // ⇒ 须写成员表达式而非具名 int key 局部（后者产 pair<int&,T&> 另一实例）。
        cmds->insert(std::make_pair(data.m_command, data));
    }
    enableSaveSkill();
    return true;
