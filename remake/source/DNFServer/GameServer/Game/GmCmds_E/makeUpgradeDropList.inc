// ORIG makeUpgradeDropList 0x084ae14c：非堆叠掉落按 grade 分桶；
// 桶内记录 [强化等级]=次数 与 [特殊类型+1000]=次数（特殊类型由
// m_addInfo2 映射：0x5aa7→1 / 0xfa6→2 / 0x6a23→3 / 0x5df0→4 / 0x7270→5）。
// b28d-solo-GmCmds_E：P167 CLEAR（58/58）——make_pair 实例修正
// （make_pair<uchar,int>、make_pair<int,map&>）+ get_grade 二次调用复现；
// switch(v) case 升序复现决策树。
// b28d-solo-GmCmds_E-2：376=375；grade/up/k2 改内联进 find 实参
// （P236 同族：ORIG 的 find 实参临时槽 -0xf0/-0xe8 比 git(-0xfc)/it(-0xf8) 浅）
// ⇒ ndiff 555→427、it 槽 -0xec→-0xf0（ORIG -0xf8）。仍 DIFF，残差：
// ① N86——ORIG find 返回经 sret 临时再拷贝到具名迭代器（-2 mov），4.4
//    copy-elision 直构，无源码解；② N98 槽序（4.4 按首次使用序）+ N82；
// ③ EH 析构路径差（p167 EXTRA pair/map D1；正常路径构造/析构次数一致）。
// 语义完成、byte 层封顶。
// [round1-L29 2026-09-10 复核] 判据① CLEAR（86 vs 86）。残差 = ORIG `push esi/ebx +
//   sub $0x130` vs 我方（P332 帧/push 族）⇒ N（ORIG 0x084ae14c）。
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
}