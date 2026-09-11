// ---------------------------------------------------------------------------
// 4. _postCheckForceChangeGrowType4AheadQuest(CUser*,Quest*) —— ORIG T 0x086686ed
//    objdump 0x086686ed..0x08668823 逐指令转写（无独立报告；调用方
//    _postCheckForceChangeGrowType 见 CUser.cpp:8029 区）。
//    quest->m_intDataList(+0x2c, vector<vector<int>>) 每组逐 questIdx：
//    find_quest 后递归本函数，并 getCurCharacQuestW()+4 处 CQuestClear
//    setClearedQuest(questIdx)。
// [b28d-62] N93/P50：ORIG 有 normal_iterator 默认构造 + vector<vector<int>>::
//   operator[]（先声明后赋值形），我方缺；85/82。ORIG 0x086686ed
// [b28d-63 本 TU] p167 三缺口实证（ORIG calls=15 / our=12）：
//   ① MISSING `normal_iterator::C1Ev` ⇒ `iterator it;` 先声明、内层 `it = begin()`（P50 复活）；
//   ② MISSING `vector<vector<int>>::operator[]` ×1 ⇒ 删 groups/group 引用缓存（P183），
//      内层 cond/body 各自重读 `quest->m_intDataList[gi]`；
//   ③ 删 cleared 具名指针（ORIG 无该槽，questIdx 经 %ebx 跨 getCurCharacQuestW 保活 ⇒ P0 内联）。
//   ⇒ 结果 **AE 85/85**（p167 仅剩同址别名 C1Ev/C2Ev 与自调用重定位伪差）；gi/questIdx
//   槽序按 P212 全头部声明（questIdx 先声明占深槽 -0x14、gi -0x10）复现。
void _postCheckForceChangeGrowType4AheadQuest(CUser* user, Quest* quest)
{
    if (quest->m_intDataList.empty())   // ORIG 0x086686f4：mov 0xc(%ebp);add $0x2c;call empty
        return;
    // ORIG 0x086870a：empty 检查后 `lea -0x1c; call normal_iterator::C1Ev`（P50 先声明）
    std::vector<int>::iterator it;
    // P212：ORIG 槽序 gi(-0x10) 浅于 questIdx(-0x14) ⇒ 全头部声明、questIdx 先声明（深）。
    int questIdx;
    for (unsigned int gi = 0; gi < quest->m_intDataList.size(); ++gi)
    {
        for (it = quest->m_intDataList[gi].begin();
             it != quest->m_intDataList[gi].end(); ++it)
        {
            questIdx = *it;
            Quest* q = G_CDataManager()->find_quest(questIdx);
            if (q == 0)
                continue;
            _postCheckForceChangeGrowType4AheadQuest(user, q);
            // ORIG 0x086878f：`mov -0x14,%ebx` 先物化 questIdx，再算 receiver（单语句内联）。
            reinterpret_cast<WongWork::CQuestClear*>(
                reinterpret_cast<char*>(user->getCurCharacQuestW()) + 4)
                ->setClearedQuest((unsigned int)questIdx);
        }
    }
}