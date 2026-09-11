// ORIG 0x081df91c：isGMUser 否 → LineFunc(0x51f3) 返回其值；
// res.m_index = msg（MsgClearQuestIndex::operator=，ORIG W 0x08235266）；
// 遍历 m_questIdxs：find_quest 失败 → return 1；exposeQuest &&
// check_possible → getCurCharacQuestW()->m_cleared.setClearedQuest(idx)、
// m_field138>0 → incQuestPoint(值,idx,true)、m_field13a>0 →
// incQuestPiece(值,idx,true)，计数；循环后 res.m_index.m_count=实际清空数，
// sendCharacQp() + sendCharacQuestPiece()，return 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081df91c orig/ours=151/127 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN28Dispatcher_Debug_Clear_Quest7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x14(%ebp),%eax | mov    %ea
int Dispatcher_Debug_Clear_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (!user->isGMUser())
    {
        return LineFunc(0x51f3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    MsgClearQuestIndex& req = static_cast<MsgClearQuestIndex&>(msg);
    ReqDebugClearQuestResult& res = static_cast<ReqDebugClearQuestResult&>(param);

    // ORIG 0x08235266：res.m_index = msg（逐标量 + vector operator=）。
    res.m_index = req;

    int clearedCount = 0;
    for (int i = 0; i < req.m_count; ++i)
    {
        // [b28d-solo-Dispatchers] P183/P48：ORIG 0x081df9a2/0x081dfa36/0x081dfa78/
        //   0x081dfac4 共 4 处各自完整重发 `vector<int>::operator[]`（p167 MISSING
        //   `_ZNSt6vectorIiSaIiEEixEj` ×3 即此项）；旧码用 questIdx 具名局部缓存
        //   只发 1 次 ⇒ 删除缓存，各使用点写完整下标表达式。
        Quest* quest = G_CDataManager()->find_quest(req.m_questIdxs[i]);
        if (quest == 0)
        {
            return 1;
        }
        stSelectQuestParam selectParam(user);
        if (quest->exposeQuest() && quest->check_possible(selectParam))
        {
            user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                (unsigned int)req.m_questIdxs[i]);
            if (quest->m_field138 > 0)
            {
                user->incQuestPoint((int)quest->m_field138, req.m_questIdxs[i], true);
            }
            if (quest->m_field13a > 0)
            {
                user->incQuestPiece((int)quest->m_field13a, req.m_questIdxs[i], true);
            }
            ++clearedCount;
        }
    }
    res.m_index.m_count = clearedCount;   // ORIG 写 *(int*)(param+0x14)
    user->sendCharacQp();
    user->sendCharacQuestPiece();
    return 0;
}