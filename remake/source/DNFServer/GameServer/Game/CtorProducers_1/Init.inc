bool AvatarRouletteServer::Init(AvatarRoulette* roulette)
{
    // [b28d-sweepC 判词·DIFF→NEAR，残余 N7+N82] ORIG 0x0817f706：
    // 本轮真缺口修复（gap 40→2）：①四处 `result = false` 原码在各自 if 块**内**
    // （ORIG 0x0817f874/0x0817f8d5/0x0817f946/0x0817f9b9 均紧跟 logFormat），旧写
    // 块外无条件（P171 死写入位置错）；②`CItem* pItem = item`（ORIG 0x0817f8d9
    // `mov -0x2c,%eax; mov %eax,-0x28`）——IsAvatarItem 之后原码另有一枚具名指针，
    // check_job_type/GetItemType/get_grade 全走它；③`&&` 拆嵌套 if（P225，
    // ORIG 0x0817f9dc `ja` + 0x0817fa4e `jbe` 两段守卫）；④`if (type==8) goto`
    // 复现 0x0817f9c1 `je <nop>` + 体尾 `jmp` 的空 then 占位岛（P223）；
    // ⑤删 logFormat 变参 `static_cast<int>(li)` 冗余 cast（P41 变体：诱发
    // %ebx callee-saved 缓存，ORIG 0x0817fa03 直读槽）。
    // 残余：`movsbl %al,%edx`（ORIG）vs `%eax`（4.4）寄存器角色（N7）+ 尾部 nop。
    // ORIG 0x0817f706。
    m_pRoulette = new AvatarRoulette;
    m_pRoulette->swap(roulette);
    m_totalCount = m_pRoulette->m_upperAvatarRate + m_pRoulette->m_normalAvatarRate;

    // [b28d-solo-CtorProducers_1 判词·N82/P112 残余] 历史修复：①p167 差集 6 次
    // `inner[type]` 重读（P157/P48）；②ORIG 两次 `outer.size()`（死调用 P171 +
    // 循环上界缓存）；③具名 expectedGrade 局部（P0）。
    // 残余：槽阶梯整体浅 4（N82；4.1 声明序 vs 4.4 首次使用序）⇒ 封顶。
    bool result = true;   // ORIG movb $0x1,-0x45；失败路径写 0，最终返回常量 true
    if (G_CEnvironment()->m_serverEnvir.m_validateScript)
    {
        for (int job = 0; job < 0xb; ++job)
        {
            std::vector<std::vector<std::pair<unsigned long, int> > >& outer =
                m_pRoulette->m_avatars[job];
            // [b28d-solo-CtorProducers_1] ORIG 0x0817f7b1..0x0817f7da 逐条：
            // ① `outer.size()` 死调用（结果丢弃，P171 照抄）；② `li=0`；
            // ③ 第二次 `outer.size()` 存 -0x38 作循环上界（cond 块
            // 0x0817fad8 只 `cmp -0x38`，不再调 size）⇒ 显式缓存 n。
            outer.size();
            size_t li = 0;
            size_t n = outer.size();
            for (; li < n; ++li)
            {
                std::vector<std::pair<unsigned long, int> >& inner = outer[li];
                for (int type = 0; type < 9; ++type)
                {
                    // P157/P48：ORIG 每个使用点重新 `inner[type]`（p167 差集
                    // 实证 operator[] MISSING ×6），禁具名 itemNo 缓存。
                    CItem* item = G_CDataManager()->find_item(
                        static_cast<int>(inner[type].first));
                    if (item == 0)
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x80,
                            "AvatarRouletteServer not exist item_no(%d)",
                            static_cast<int>(inner[type].first));
                        result = false;   // ORIG movb $0x0,-0x45（无 continue）
                    }
                    if (!item->IsAvatarItem())
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x86,
                            "AvatarRouletteServer no avatar item_no(%d)",
                            static_cast<int>(inner[type].first));
                        result = false;   // ORIG 0x0817f8d5（块内，非无条件）
                    }
                    // ORIG 0x0817f8d9 `mov -0x2c(%ebp),%eax; mov %eax,-0x28(%ebp)`
                    // ⇒ IsAvatarItem 之后原码另有一枚具名 CItem*（后续 check_job_type/
                    // GetItemType/get_grade 全走它；-0x2c 只用于判空与 IsAvatarItem）。
                    CItem* pItem = item;
                    if (!pItem->check_job_type(static_cast<char>(job)))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x8d,
                            "AvatarRouletteServer item_no(%d) check_job_type(%d)==false",
                            static_cast<int>(inner[type].first), job);
                        result = false;   // ORIG 0x0817f946（块内）
                    }
                    unsigned int itemType = pItem->GetItemType();
                    if (itemType != static_cast<unsigned int>(type))
                    {
                        LogManager::logFormat(
                            1, "localjapan/Arad_AvatarRoulette.cpp",
                            __PRETTY_FUNCTION__, 0x95,
                            "AvatarRouletteServer item_no(%d) GetItemType(%d)!=%d",
                            static_cast<int>(inner[type].first), itemType, type);
                        result = false;   // ORIG 0x0817f9b9（块内）
                    }
                    // ORIG 0x0817f9c1 `je 0817fac0(nop)` + 体尾 0x0817fabe `jmp
                    // 0817fac1` ⇒ 4.1 空 then 占位岛（P223）；4.4 jump-thread 掉，
                    // 用 `if (type==8) goto` + 尾标签复现。
                    if (type == 8)
                        goto skip_grade_check;
                    {
                        int grade = pItem->get_grade();
                        // P0：ORIG `movl $0x0,-0x1c` 预清 + 两支各 `movl $2/$1,-0x1c`
                        // ⇒ 原码有具名 expectedGrade 局部并在 logFormat 实参处读取
                        // （我们旧写立即数 2/1，缺槽往返与 store）。
                        int expectedGrade = 0;
                        // ORIG 0x0817f9dc/0x0817fa4e：`&&` 拆嵌套 if（P225），
                        // 两支各在体内写 result=false 后 jmp 循环尾。
                        if (li < 2)
                        {
                            if (grade != 2)
                            {
                                expectedGrade = 2;
                                LogManager::logFormat(
                                    1, "localjapan/Arad_AvatarRoulette.cpp",
                                    __PRETTY_FUNCTION__, 0xa3,
                                    "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                    static_cast<int>(inner[type].first), grade,
                                    expectedGrade,
                                    li);
                                result = false;   // ORIG 0x0817fa48（块内）
                            }
                        }
                        else if (li > 1)
                        {
                            if (grade != 1)
                            {
                                expectedGrade = 1;
                                LogManager::logFormat(
                                    1, "localjapan/Arad_AvatarRoulette.cpp",
                                    __PRETTY_FUNCTION__, 0xa9,
                                    "AvatarRouletteServer item_no(%d) get_grade(%d)!=%d, line_num(%d)",
                                    static_cast<int>(inner[type].first), grade,
                                    expectedGrade,
                                    li);
                                result = false;   // ORIG 0x0817faba（块内）
                            }
                        }
                    }
                skip_grade_check:;
                }
            }
        }
    }
    return true;
}