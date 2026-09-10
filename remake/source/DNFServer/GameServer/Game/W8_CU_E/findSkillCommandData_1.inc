bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out, bool flag)
{
    // b28d-35: DIFF→N82（帧 0x34 vs 0x48、槽平移；bool 形参截断槽）。ORIG @0x08654e6a。
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    std::map<int, STSkillCommandData>::iterator it = cmds->find(skillId);
    // P222/P59：ORIG 0x08654ed3 `test %al,%al; je 8654f69`（**尾部** `mov $0x0,%eax`
    // 岛）+ 主体 fall-through 到 `mov $0x1,%eax; jmp epilogue` ⇒ 源码是**正向包裹**
    // `if (it != end) { 主体; return true; } return false;`；原「早返 return false +
    // 尾部 return true」形 4.4 恒产反向岛。
    if (it != cmds->end())
    {
        out.m_command = it->second.m_command;
        out.m_count   = it->second.m_count;
        // P0：ORIG 0x08654f4e 循环上界读 **out**+0x2（`mov 0x10(%ebp),%eax;
        // movzbl 0x2(%eax),%eax; movsbl %al,%eax; cmp -0xc(%ebp),%eax`）而非
        // it->second ⇒ 上界表达式须写 out.m_count。
        for (int i = 0; i < (signed char)out.m_count; ++i)
        {
            // P0/P291：ORIG 0x08654f13..0x08654f3d 为「读 it->second.m_keyBytes[i] →
            // 写 out.m_keyBytes[i] → **重读 out.m_keyBytes[i]** 与 7 比较」，全程无中间
            // 字节槽（我方原 `key` 局部多出 -0x9 槽 + `mov %al,-0x9`/`cmpb $7,-0x9`
            // 三处偏移）⇒ 删具名局部、条件处重读目标表达式。
            out.m_keyBytes[i] = it->second.m_keyBytes[i];
            if (out.m_keyBytes[i] == 7)
                out.m_keyBytes[i] = 8;
        }
        return true;
    }
    return false;
