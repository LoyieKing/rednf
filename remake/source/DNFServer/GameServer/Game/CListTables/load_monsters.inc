bool CMonsterManager::load_monsters()  // [P330 int→bool] 与 CDataManager.h 声明同步（ORIG caller xor $1 形）
{
    // [b28d-solo-CListTables P168a] ORIG 0x0834fa8e `call; xor $1; test %al,%al;
    // je 主体` = 负形早退（失败分支 fall-through 到 puts/return 0）。
    if (!sync_script::GetInstanceSyncScript()->truncate_monster_info_table())
    {
        puts("Fail truncate_monster_info_table");
        return 0;
    }
    {
        char* path = 0;
        CMonster* monster = 0;
        // [b28d-solo-CListTables] ORIG：主循环 map 迭代器先 C1Ev 后 begin() 赋值
        // （P50）+ 尾后置 it++（ppEi，P28）。
        // [round5-A29] 未使用的 list<int>::iterator 必须与 map 迭代器**同批声明**
        // （在 begin() 赋值之前）：ORIG 0x0834fa?? 段构造序为
        //   `lea -0x38; call _Rb_tree_iterator::C1Ev` → `lea -0x3c; call
        //   _List_iteratorIiEC1Ev` → `map::begin` 回填 -0x38
        // 旧序（begin 后声明）令 4.4 把 C1Ev 排到 begin 之后（判据⑩/声明序、
        // P28 声明序即槽序 -0x38/-0x3c 逐值对齐）。
        std::map<int, std::string>::iterator it;
        // [b28d-solo-CListTables] ORIG 有一处未使用的 list<int>::iterator C1Ev
        // （p167 MISSING 1；P171/P232 死局部照抄）。声明序按判据⑩（后声明者深）。
        std::list<int>::iterator unusedIt;
        it = g_stMonsterScriptFileList.begin();
        while (it != g_stMonsterScriptFileList.end())
        {
            path = (char*)it->second.c_str();
            if (path == 0)
            {
                // [round5-A29] P325 多 return 共享尾出口：ORIG 0x0834fa?? 对
                // `path == 0` 发 `je <共享 ret-true 块>`（单一出口，块内含
                // `mov $1,%eax`），4.4 对裸 `return 1;` 恒内联成
                // `mov $1,%ebx; jmp <epilogue>`（+2 条，fn_diff 唯一残差）。
                // 改 goto 与循环后 `return 1` 共享同一标签即复现 `je` 直跳。
                goto ret_true;
            }
            STMonsterScript script;
            if (!ImportMonsterScript(&script, 3, path, false))
            {
                printf("Monster Script Error: %s\n", path);
                return 0;
            }
            if (!sync_script::GetInstanceSyncScript()
                     ->insert_monster_info_to_db(script))
            {
                printf("insert_monster_category Error: %s\n", path);
                return 0;
            }
            // [round5-A29 实测后回退] 曾试删本 `vector&` 引用（P183/P329 去具名缓存，
            // 假设 ORIG script 槽 -0x1918 vs 我方 -0x191c 的 ±4 即本槽）——实测
            // **反劣**：删后 script 侧对齐但函数头标量区反而整体浅 4
            // （ORIG -0x2c/-0x28 vs 我方 -0x28/-0x24，near 5/diff 7 → near 4/diff 8），
            // 即两组槽向相反（P331「混合尺寸区声明序不可翻」）⇒ 维持原形。
            std::vector<stIndependentDropInfo>& drops =
                G_CDataManager()->m_independentDrop.m_vecC;
            // [b28d-solo-CListTables P196/P218] ORIG 0x0834fc48 `cmp i; setg` 有符号
            // 比较（unsigned i 生 seta）⇒ 下标局部 int 化。
            for (int i = 0; i < (int)drops.size(); ++i)
            {
                if (drops[i].m_field0 == 0 &&
                    drops[i].m_monsterIndex == script.m_index)
                {
                    script.m_dropList.push_back(drops[i].m_drop);
                }
            }
            monster = new CMonster();
            monster->set_monster(script);
            for (int i = script.m_minLevel; i <= script.m_maxLevel; ++i)
            {
                insert_monster(i, monster->get_index());
            }
            insert_monster(monster->get_index(), monster);
            it++;
        }
    ret_true:   // P325：与循环内 `goto ret_true` 共享真出口（见上）
        return 1;
    }
