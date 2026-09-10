void CParty::finish_loading(CUser* user)
{
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (m_member[i].m_pUser != user)
            continue;
        // ORIG 0x85b161b: movzbl 0x37c; test; jne +0x126（nop;leave）——
        // 直落函数出口，是 return 非 continue（continue 应跳 +0x88 addl）。
        if (m_battleData.m_arr50[i] != 0)
            return;
        m_battleData.m_arr50[i] = 1;
        // P248 档②/P292：ORIG 0x085b15e0 域为 `movzbl 0xb10(%edx,%eax,1);
        // xor $0x1,%eax; test %al,%al; je <循环尾>`（bool 字段取反三件套）
        // ⇒ 字段 bool 化（BattleData.h 已同步）+ 裸 `if (!field){...}` 正形
        // （内层 `if (call)` 的 `call; test; je 循环尾` 与 ORIG 0x85b170a 段一致）。
        // 旧形 `unsigned char fv = ...; fv ^= 1; if(!fv) continue;` 在 4.4 下
        // 落栈槽三段（movb/xorb/cmpb）且 continue 链多 jmp+nop 中继。
        // 实测（19:41）：`&&` 短路形被 4.4 折成 `mov $1/%eax; jmp; mov $0; test;
        // je` 三行 bool 汇聚（P133 警告形，+3）⇒ 回退嵌套 if。嵌套形仅余
        // body 尾 `jmp <循环尾>; nop` 中继（P63），gap 86:93 → 86:90。
        if (!m_battleData.m_arr7e4[i])
        {
            if (OnStartMapFinishLoading(user))
                m_battleData.m_arr7e4[i] = 1;
        }
    }

    // ORIG 两次独立 this+0xb24 计算（GetDungeon / IsFirstEnterDungeon），
    // 不保存 CBattle_Field 引用。
    CDungeon* dungeon =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->GetDungeon();  // -0x10
    // ORIG 0x085b16??：&& 短路 flag-island 双层物化（`mov $1,%eax; jmp; mov $0,%eax;
    // test %al; je`，技巧 40/22 形）——独立 enterEachMap 槽会反产 movl 存回。
    if (dungeon != 0 &&
        dungeon->IsEnterEachMap())
    {
        if (reinterpret_cast<CBattle_Field*>(&m_battleField)->IsFirstEnterDungeon())
            check_allmember_loading(false);
        else
            check_allmember_loading(true);
    }
    else
    {
        check_allmember_loading(false);
    }
