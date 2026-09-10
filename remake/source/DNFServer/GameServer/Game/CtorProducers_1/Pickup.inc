bool AvatarRouletteServer::Pickup(PickedAvatarInfo& picked, int grade, int index)
{
    // [b28d-sweepC 判词·残余 N89+N7+P211，未闭合] ORIG 0x0817fb28：
    // 历史修复：P157（两次独立 operator[]）；P96（非 const _Rb_tree_iterator）；
    // P123（二次 operator[]）；P171（bandKind 死写入，gap 10→7）。
    // 本轮修：P196 比较书写序改 `size() <= band`（复现 ORIG `call size; cmp band;
    // setbe`，消 setae/setbe 助记符差；四数无回退）。
    // 残余：①ORIG out-of-line `map::end()`（0x08180c68，N89 4.4 恒内联不可发射）；
    // ②jobIdx/levelIdx 用 %edi/%esi 跨调用（N7）；③`entry.first` 拷贝 ORIG 两段
    // `mov (%eax),%eax; mov %eax,%edx` vs 4.4 单段（P211/N18a）⇒ 封顶。
    // ORIG 0x0817fb28（报告 AvatarRouletteServer/Pickup.md 全文）。
    // 档位抽取：randVal<roulette+0x0c 时 band∈{0,1}，否则从
    // m_avatars[0].size()-2 偏移取 band（内联 size helper 08180760 实证）。
    int randVal = get_rand_int(m_totalCount);
    // [b28d-solo-CtorProducers_1 修复·P171] ORIG 0x0817fb42 `movl $0x0,-0x2c`
    // 并在两分支写 2/1，但全程**未读**（Ghidra C `local_30` 无消费者）= 原码死
    // 写入，必须照抄（缺它 -0x2c 槽与后续装载形态全异）。
    int bandKind = 0;
    if (randVal < m_pRoulette->m_upperAvatarRate)
    {
        picked.m_gradeBand = static_cast<char>(get_rand_int(2));
        bandKind = 2;   // ORIG 0x0817fb6b
    }
    else
    {
        picked.m_gradeBand = static_cast<char>(
            get_rand_int(static_cast<int>(m_pRoulette->m_avatars[0].size()) - 2) + 2);
        bandKind = 1;   // ORIG 0x0817fb9b
    }
    (void)bandKind;   // 死写入（P171 照抄；无消费者）

    if (grade < 0 || grade > 10)
        return false;

    // [b28d-solo-CtorProducers_1] P157/p167：ORIG 对 `outer[band]` 调
    // `vector<vector<...>>::operator[]` **两次**（size 检查一次、取值一次），
    // 禁具名 outer/inner 引用缓存（旧写法仅 1 次，p167 MISSING×1）。
    // [b28d-sweepC 修复·P196] ORIG `call size; cmp band,%eax; setbe` ⇒ 源码写
    // `size() <= band`（4.4 同形复现，消旧 `band >= size()` 的 setae 助记符差；
    // 四数不回退）。
    if (m_pRoulette->m_avatars[grade].size() <=
        static_cast<unsigned int>(picked.m_gradeBand))
        return false;
    if (m_pRoulette->m_avatars[grade][static_cast<int>(picked.m_gradeBand)].size()
        <= static_cast<unsigned int>(index))
        return false;

    std::pair<unsigned long, int>& entry =
        m_pRoulette->m_avatars[grade][static_cast<int>(picked.m_gradeBand)][index];
    // [b28e-L21 已试无效] P41 去 cast 试验：ORIG 0x0817fc90 `mov (%eax),%eax;
    // mov %eax,%edx`（两段）vs 我方单段 `mov (%eax),%edx`；去 static_cast（改隐式
    // 窄化）实测形态不变（TU 四数零变化）⇒ 属 N18a/P211 跨版本单段折叠，封顶。
    picked.m_itemId = static_cast<unsigned int>(entry.first);   // ORIG 仅写低 dword
    picked.m_zoneType = static_cast<char>(entry.second);

    // [b28e-L21 已试无效] ORIG 0x0817fcab `mov 0x4(%eax),%esi` 单段直读 vs 我方
    // `mov 0x4(%eax),%eax; mov %eax,%esi` 两段；去 (int) cast 实测无变化 ⇒ 同封顶。
    // [T1 直证] m_itemId 已 int 化（DataManagerInitDepends.h）⇒ 形参同型直传，
    // 复现 ORIG 0x0817fcab 单条 `mov 0x4(%eax),%esi`。
    CItem* item = G_CDataManager()->find_item(picked.m_itemId);
    if (item == 0)
        return false;
    if (!item->IsAvatarItem())                                  // vtable+0x10==1 才继续
        return false;

    CEquipItem* equip = static_cast<CEquipItem*>(item);
    // [b28e-L21 判据⑩] ORIG randIdx 槽 -0x1c（全身最浅）⇒ 原码声明在 equip 之后
    // （0x0817fd0e `mov %eax,-0x1c(%ebp)` + 0x0817fd11 回读 = 先声明后赋值两段式）。
    unsigned int randIdx;
    randIdx = static_cast<unsigned int>(get_rand_int(equip->getAvatarOptionSize()));
    picked.m_optionOrSkillIdx = static_cast<char>(randIdx);

    // [BigUserLogic] ORIG 伪C/汇编（0x0817fe??..0817fec6 唯一 `movb $0,0x9`）：
    // 特殊段与「非特殊」路径在 pad9 写入点**汇合**（单一物理写）；旧 `if(!special)
    // {pad9=0;return}` 早返形造成两处物理写（+3 条）。改回正向块 + fallthrough。
    if (index == 3 && (grade == 9 || grade == 0))
    {
        // 加成技能选择：ORIG 将 m_addCharacSkillMap[uid] 的 value map 拷贝到栈
        // （copy ctor 08180ca2 实证），找不到 grade 键时直接返回 false（不写 pad9）。
        // p167：ORIG 用**非 const** `_Rb_tree_iterator`（operator==/operator-> 非 const
        // 实例）+ `operator[]` 二次取值（find 判存在 + operator[] 取 map，P123），
        // 旧 const_iterator 多转换构造 ×2 + const 版比较。
        // [P293] ORIG 0x0817fd43 先 `end()`（sret -0x48）后 `find(key)`（sret -0x44）
        // + `operator==` ⇒ 原码是**单表达式**比较形；具名 it 形 4.4 恒 find 先/end 后。
        if (m_addCharacSkillMap.find(
                static_cast<unsigned long>(picked.m_itemId)) ==
            m_addCharacSkillMap.end())
            return false;

        std::map<int, std::vector<LevelUpSkill> > skillMap =
            m_addCharacSkillMap[static_cast<unsigned long>(picked.m_itemId)];
        // [P293] 同族：ORIG 0x0817fdfb `end()` 先于 `find(grade)`（grade 为同型形参
        // ⇒ 直接传址 `lea 0x10(%ebp)`，无 key 临时）。
        if (skillMap.find(grade) == skillMap.end())
            return false;

        std::vector<LevelUpSkill> skillVec = skillMap[grade];
        randIdx = static_cast<unsigned int>(
            get_rand_int(static_cast<int>(skillVec.size())));
        picked.m_optionOrSkillIdx =
            static_cast<char>(skillVec[randIdx].m_field4);
    }

    picked.m_pad9 = 0;
    return true;
