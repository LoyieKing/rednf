void CUser::setSkillCommandData(char* data)
{
    memcpy(&m_skillCommands[0], data, 200);

    std::map<int, STSkillCommandData>* cmds = reinterpret_cast<std::map<int, STSkillCommandData>*>(
        const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return;

    for (int i = 0; i < 50; ++i)
    {
        // [A06 2026-09-10 负结论·已回滚] ORIG 每次访问重算完整地址
        // （`mov -0x10(%ebp),%edx; mov 0x8(%ebp),%eax; add $0x1c0f4,%edx;
        //   movzbl 0xb(%eax,%edx,4),%ecx`，无 e 指针槽），但 4.4 对**同义表达式**
        // 两形实测均反劣：① `reinterpret_cast<const unsigned char*>(&m_skillCommands[i])[k]`
        // 逐处内联 = 200→230 条；② `((const unsigned char*)m_skillCommands)[i*4+k]` = 230→239 条
        // （4.4 恒发 add 链，不复现 4.1 的 `disp(%base,%idx,4)` 折叠）。
        // ⇒ 正解需 STCommand 内建**字节数组视角**（`union { 七个 packed 位域; unsigned char
        // m_bytes[4]; }`，sizeof 4/align 1 不变）才能出 ORIG 的单条 scaled 访存；属**头改动**，
        // 按 R2 只报告不动（见交付清单）。此前保留指针缓存形（4.4 最优，200 条）。
        const unsigned char* e =
            reinterpret_cast<const unsigned char*>(&m_skillCommands[i]);  // 步长 4
        unsigned short cmd = static_cast<unsigned short>(
            (((unsigned short)e[3]) & 0x3f) << 8 | ((unsigned short)e[0]));
        if (cmd == 0)
            break;

        unsigned char count = e[3] >> 5;
        unsigned char keys[4];
        keys[0] = ((((unsigned short)e[1]) | ((unsigned short)e[2] << 8)) >> 6) & 7;
        keys[1] = (unsigned char)((e[2] >> 1) & 7);
        keys[2] = (unsigned char)((e[2] >> 4) & 7);
        keys[3] = (unsigned char)((((unsigned short)e[1]) |
                                   ((unsigned short)e[2] << 8)) >> 15) & 7;
        // 第 5 提取:e[3]>>2&7 对应 STCommand bit29-31(m_key4/m_count 区)

        // [b28d-solo-CUserOnlyOrig P167/P61] ORIG 0x08654xxx 命中
        // `make_pair<short&, STSkillCommandData&>`（非 int&）⇒ key 声明 short；
        // 且命中 `_Rb_tree_iterator::operator==`（非 !=）⇒ 原码 `if (it == end())`
        // 先走插入分支（P222/P220 极性按 ORIG 取 ==）。
        short key = static_cast<short>(cmd);                // -0x48 有符号 16 位
        std::map<int, STSkillCommandData>::iterator it = cmds->find(key);
        if (it == cmds->end())
        {
            STSkillCommandData sc;
            sc.m_command = cmd;
            sc.m_count   = count;
            for (int k = 0; k < static_cast<char>(count); ++k)
                (&sc.m_key0)[k] = keys[k];
            cmds->insert(std::make_pair(key, sc));          // make_pair(short&,data&) 收敛为值对
        }
        else
        {
            it->second.m_command = cmd;
            it->second.m_count   = count;
            for (int k = 0; k < static_cast<char>(count); ++k)
                (&it->second.m_key0)[k] = keys[k];          // +7 起逐键写(ORIG 循环)
        }
    }
    memset(&m_skillCommands[0], 0, 200);
