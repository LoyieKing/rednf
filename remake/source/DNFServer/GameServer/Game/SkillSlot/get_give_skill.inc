// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a,N96）→ 试=nd 231/233 唯一差 = 2×
// `mov 0x14(%eax),%edx`（ORIG）vs `mov 0x14(%eax),%eax; mov %eax,%edx`（我方）；本轮新形
// `(*(CCharacter**)((char*)G_CDataManager()+0x14))` 实测变 `add $0x14,%eax; mov (%eax),%edx`
// 仍 2 条（未命中，已回滚）⇒ 维持（新=DIFF gap=-3, blocks=2）。
int SkillSlot::get_give_skill(
    int job, int firstGrow, int secondGrow,
    std::vector<std::pair<int, int> >& out)
{
    if (job < 0)
    {
        return -1;
    }
    // [b28d-solo-SkillSlot] 修法：ORIG 0x0860714a 形 = 正极性 `if (IsPvP())`
    // （`call; test %al,%al; je <普通分支>`，P292 bool callee 直判、无 xor）
    // ⇒ PvP 分支 fall-through 在前、普通分支在后；去 dm 具名缓存（每次内联
    // G_CDataManager()，P183）；result 局部 bool + `if (!result)`（P158 xor 物化）；
    // 两处 get_give_skill 第 2 实参传形参 secondGrow（非字面量 0）；
    // 迭代器 for + 循环后 eq 判尾（P7），禁用 while(true)+break。
    // [b28d-solo-SkillSlot] 判词（N18a/P168a 封顶，231=231，p167 CLEAR 39=39）：
    // 残余仅 2 类纯寄存器/物化形：① m_characList 装载 4.4 发
    // `mov 0x14(%eax),%eax; mov %eax,%edx`（ORIG 单条 `mov 0x14(%eax),%edx`，
    // 2 处；指针算术/CCharacter*+job/数组下标三形均同）；② result!=1 检查
    // ORIG 4.1 物化 `movzbl; xor $1; test %al; je`，4.4 对 char 局部恒
    // `cmpb $1; jne`（bool 化则存储段多 setne，两段不可兼得，P168a④/N96）。
    // 语义等价（get_give_skill 返回 char 无归一化，ORIG 0x08348838 movzbl 直返）。
    // [J10GameRest.J10SkillSlot 2026-09-11 复核] p167 calls 39 vs 39（仅 C1/C2 别名）；
    // 231/233 的 2 条差额 = 上述 ① 的 4.4 regmove（N18a，`mov 0x14(%eax),%eax;
    // mov %eax,%edx` 单段不可造），② 段本函数未复现（我们 231 vs 233 全落在 ① 两处）。
    // 本轮无新增旋钮可落地 ⇒ 维持 N18a 封顶。
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        out.clear();
        (&G_CDataManager()->m_pvpSkillTree)
            ->getGiveSkill(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                (int)((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType(),
                (int)((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType(),
                out);
        return 1;
    }
    out = *(std::vector<std::pair<int, int> >*)(
        (char*)G_CDataManager()->m_characList + job * 0x7dc + 0x240);
    std::vector<std::pair<int, int> > learnList;
    // 同上（D06 消费侧配对 bool 化）。
    bool result = true;
    if (secondGrow == 0)
    {
        result = (((CCharacter*)G_CDataManager()->m_characList) + job)
                     ->get_give_skill(firstGrow, secondGrow, learnList, 1);
    }
    else
    {
        result = (((CCharacter*)G_CDataManager()->m_characList) + job)
                     ->get_give_skill(firstGrow, secondGrow, learnList, 0);
    }
    if (!result)
    {
        return -1;
    }
    for (std::vector<std::pair<int, int> >::const_iterator git =
             learnList.begin(),
         gend = learnList.end();
         git != gend; ++git)
    {
        std::vector<std::pair<int, int> >::iterator lit = out.begin();
        std::vector<std::pair<int, int> >::iterator lend = out.end();
        for (; lit != lend; ++lit)
        {
            if (lit->first == git->first)
            {
                lit->second = std::max(lit->second, git->second);
                break;
            }
        }
        if (lit == lend)
        {
            out.push_back(std::make_pair(git->first, git->second));
        }
    }
    return 1;
}