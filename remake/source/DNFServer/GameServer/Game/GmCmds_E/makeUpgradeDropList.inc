void CMonsterDropTest::makeUpgradeDropList(std::list<map_item>& items, GradeStat& out)
{
    for (std::list<map_item>::iterator it = items.begin(); it != items.end(); ++it)
    {
        if (it->m_item.m_addInfo == 0)
        {
            continue;
        }
        CItem* ci = G_CDataManager()->find_item(it->m_item.m_addInfo);
        if (ci->is_stackable())
        {
            continue;
        }

        // ORIG 0x084ae14c：`cmp $imm,%eax` 决策树 + 尾部 movl $N,-0xc 叶块
        // （case 值升序书写 ⇒ 叶块序 1..5）；v 先声明占深槽、special 后声明。
        int v = it->m_item.m_addInfo2;
        int special = 0;
        switch (v)
        {
        case 0x5aa7:
            special = 1;
            break;
        case 0xfa6:
            special = 2;
            break;
        case 0x6a23:
            special = 3;
            break;
        case 0x5df0:
            special = 4;
            break;
        case 0x7270:
            special = 5;
            break;
        }

        // P236：ORIG 的 find 实参临时槽 -0xf0 比 git(-0xfc) 浅 ⇒ 内联实参形
        // （具名 grade 使 4.4 排到 -0xf8，git 到 -0xf4，与 ORIG 相反）。
        std::map<int, std::map<int, int> >::iterator git =
            out.find(ci->get_grade());
        // P222 正向包裹：ORIG `test;je <inner 块>` 即 git!=end 直落（up/k2 段）、
        // inner 构造走尾岛；且 insert 处重新调用 GetUpgrade（不用 up 槽）。
        if (git != out.end())
        {
            // P236 同族：ORIG 的 up 临时槽 -0xe8 比 it(-0xf8) 浅 ⇒ 内联实参。
            std::map<int, int>::iterator f =
                git->second.find(it->m_item.GetUpgrade());
            if (f != git->second.end())
            {
                f->second++;
            }
            else
            {
                git->second.insert(std::make_pair(it->m_item.GetUpgrade(), 1));
            }
            std::map<int, int>::iterator g =
                git->second.find(special + 1000);
            if (g != git->second.end())
            {
                g->second++;
            }
            else
            {
                git->second.insert(std::make_pair(special + 1000, 1));
            }
        }
        else
        {
            std::map<int, int> inner;
            inner.insert(std::make_pair(it->m_item.GetUpgrade(), 1));
            inner.insert(std::make_pair(special + 1000, 1));
            out.insert(std::make_pair(ci->get_grade(), inner));
        }
    }
