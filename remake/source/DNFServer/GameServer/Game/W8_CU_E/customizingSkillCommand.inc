// ---------------------------------------------------------------------------
// 3. int CUser::customizingSkillCommand(STSkillCommandData& data)
//    @0x08654f74
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
int CUser::customizingSkillCommand(STSkillCommandData& data)
{
    // b28d-35: DIFF→N82/N7（帧 0x54 vs 0x58；寄存器分配差）。ORIG @0x08654f74。
    // [M3S1 2026-09-11 复核·登记勿动] show_func_diff：orig=114 our=114 **gap=0 ndiff=0**
    //   （逐指令全等；含 ORIG 0x0865502b/0x08655040 的 operator-> 双取、0x086550a3
    //   `make_pair<short&, T&>` 左值 data.m_command 实参形、0x086550b8 pair 转换 ctor）
    //   ⇒ 本件已是 ORIG 形，P212/P309/P339 均无可转之旋钮。
    // [C4A0Nine 2026-09-11 复验] n1_v=AE（114/114，clean 归一口径 identical）；
    //   strict 残差纯为 .o 未链接 reloc 表示（`call <off> <self+0x..>` vs 链接后
    //   `<sym>`），非源码可控 ⇒ 无需改 STSkillCommandData.h:43 位域类型（该位域是
    //   STCommand 内嵌结构，与本函数读的 +0x00 `unsigned short m_command` 非同字段）。
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
}