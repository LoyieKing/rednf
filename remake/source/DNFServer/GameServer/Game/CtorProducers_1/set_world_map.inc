void CWorldMap::set_world_map(WorldMapScript* script)
{
    // [b28d-sweepC 判词·DIFF→NEAR，残余 N82 槽基差] ORIG 0x0836421a：
    // 历史修复：const_iterator + 具名 last；getDungeonKind() 两次独立调用；
    // 直读 `m_villageAttackEventFlag`（P292）；正向包裹（P296）。
    // 本轮修（gap 13→1，助记符序列全等）：①三段 assign 的 begin/end 改**内联
    // 实参**（4.1/4.4 均右→左求值 ⇒ 先 end 后 begin，与 ORIG 0x0836423e 的
    // `call end` 在前一致；旧具名迭代器语句序 begin 先 ⇒ 相反）；②
    // `!m_villageAttackEventFlag` 改 `if (flag) goto` 尾标签（P223/N122：4.4 对
    // `!bool` 恒 xor+je，goto 守卫复现 ORIG 0x08364468 `test; jne`）；③
    // `if (dungeon == 0) goto` 复现 ORIG 0x083644d7/0x083644d8 的 `nop`+`jmp`
    // 中继岛（P223）。
    // 残余：槽基整体浅 0x10（ORIG it/last -0x40/-0x44 vs 我们 -0x30/-0x34，
    // 4.1 的 4 个匿名临时槽）⇒ N82/N95 封顶。
    // ORIG 0x0836421a：标量回写 + 三段 vector assign + m_dungeonIndex=MAXINT；
    // 遍历 script->m_dungeons：find_dungeon 命中者 push 进 m_dungeonList，
    // 并按 kind==1/kind==2/其它三分支以 min 收敛 m_dungeonIndex（kind==1 另置
    // m_hasDeathTower=1）；kind 其它分支先查 dungeon+0x87a 字节跳过标记，
    // 再按 index==100 与 standard_level 收敛。
    m_index = script->m_index;
    m_isHellDungeon = script->m_isHellDungeon;
    // [b28d-sweepC] ORIG 0x0836423e..0x0836431e：assign 的 begin/end **内联实参**
    // （4.1/4.4 均右→左求值 ⇒ 先 end 后 begin，与 ORIG 的 `call end` 在前一致；
    // 旧具名迭代器使语句序 begin 先 ⇒ 与 ORIG 相反）。
    m_field1c.assign(script->m_hellQuests.begin(), script->m_hellQuests.end());
    m_hasDeathTower = 0;
    m_hellPassItemList.assign(script->m_freepassItems.begin(),
                              script->m_freepassItems.end());
    m_field34.assign(script->m_itemConditions.begin(),
                     script->m_itemConditions.end());
    m_dungeonIndex = 0x7fffffff;
    // [b28d-solo-CtorProducers_1] Ghidra C + p167 实证三处修：
    // ①迭代器 **const_iterator**（ORIG `__normal_iterator<WorldMapDungeon const*>`；
    //   非 const 版多转换构造/实例差，P96/P270）；②`getDungeonKind()` **两次独立
    //   调用**（ORIG else 内再调一次，P48 无 CSE；旧 `int kind=` 缓存致 MISSING×1）；
    // ③else 分支条件直接读字段 `!dungeon->m_villageAttackEventFlag`（+0x87a bool，
    //   P292），旧 `ctors1::IsBlockWorldMapMark()` inline 调用致 EXTRA×1（已删该 helper）。
    // ORIG 两枚 const_iterator 各自经「非 const→const」转换构造（Ghidra C 两处
    // `__normal_iterator<const*>::__normal_iterator<WorldMapDungeon*>`）+ 同型
    // `ne(const,const)`；`it != script->m_dungeons.end()` 直写会产混合实例差。
    std::vector<WorldMapDungeon>::const_iterator it = script->m_dungeons.begin();
    std::vector<WorldMapDungeon>::const_iterator last = script->m_dungeons.end();
    while (it != last)
    {
        CDungeon* dungeon =
            G_CDataManager()->find_dungeon(it->m_dungeonIndex);
        // ORIG 0x083644d7/0x083644d8：`if (dungeon != 0)` 的假分支落 `nop` +
        // `jmp <循环尾>` 中继岛（P223 4.1 空 then 占位）；4.4 jump-thread 掉 ⇒
        // 用 `if (dungeon == 0) goto` + 尾标签复现岛。
        if (dungeon == 0)
            goto skip_dungeon_body;
        {
            m_dungeonList.push_back(it->m_dungeonIndex);
            if (dungeon->getDungeonKind() == 1)
            {
                int minLevel = dungeon->get_min_level();
                m_dungeonIndex = std::min(m_dungeonIndex, minLevel);
                m_hasDeathTower = 1;
            }
            else if (dungeon->getDungeonKind() == 2)
            {
                int minLevel = dungeon->get_min_level();
                m_dungeonIndex = std::min(m_dungeonIndex, minLevel);
            }
            else
            {
                // ORIG 0x08364371：`movzbl 0x87a(%eax),%eax; test %al,%al;
                // jne <循环尾>`（4.1 无 xor 的取反守卫）；4.4 对 `!bool` 恒
                // xor+je（N122）⇒ 用 goto 尾标签复现 jne 直跳。
                if (dungeon->m_villageAttackEventFlag)
                    goto skip_village_map;
                {
                    if (dungeon->get_index() == 0x64)
                    {
                        int minLevel = dungeon->get_min_level();
                        m_dungeonIndex = std::min(m_dungeonIndex, minLevel);
                    }
                    int standardLevel = dungeon->get_standard_level();
                    m_dungeonIndex = std::min(m_dungeonIndex, standardLevel);
                }
            skip_village_map:;
            }
        }
    skip_dungeon_body:;
        ++it;
    }
}