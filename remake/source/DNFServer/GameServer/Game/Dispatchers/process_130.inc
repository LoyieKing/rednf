// ORIG 0x081df42c：state>2 → param+4=0/param+8=-1；等级 27..69 且
// getCurCharacQuestR()->m_authenData.m_urgentQuest==-1 才继续；
// hasEpicQuest → err=1；否则静态 selected_quest 清空后
// getQuestNPCList(8)+allowable_questlist_as_npc(…,true) 取候选：
// 空 → err=4；非空 → 静态 CMTRand randInt(size-1) 抽一个写回
// m_urgentQuest 与 param+8，param+4=1。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081df42c orig/ours=214/206 ndiff-blocks=38
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x28(%ebp) | mov    -0x28(%ebp),%eax
int Dispatcher_Req_Urgent_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }

    // [G1Dproc 2026-09-10] ORIG 0x081df4xx：param→res 槽的 `mov 0x14(%ebp),%eax; mov %eax,-0x28(%ebp)`
    //   出现在 `get_state()<=2` 早退**之后**（ORIG 第 15-18 条），⇒ res 声明下移一个 guard。
    ReqResult& res = static_cast<ReqResult&>(param);
    res.m_found = 0;
    res.m_questIdx = -1;

    // [G1Dproc 2026-09-10] ORIG 0x081df4xx（`mov %eax,-0x24(%ebp); cmpl $0x1a…; jle…;
    //   cmpl $0x45…; jg…` + 直接读 questR->m_authenData.m_urgentQuest）⇒ P0：
    //   只有 `level` 一个具名槽；`questR` 与 `eligible` 均**无槽**（原源码 bool eligible 会物化
    //   `mov %al,-0x25; movzbl; xor $0x1,%eax` 三条 + 多两槽 ⇒ 帧漂移）。
    int level = user->get_charac_level();
    if (!(level > 0x1a && level <= 0x45 &&
          user->getCurCharacQuestR()->m_authenData.m_urgentQuest == -1))
    {
        res.m_errorCode = 1;
        return 0;
    }
    if (user->getCurCharacQuestW()->hasEpicQuest())
    {
        res.m_errorCode = 1;
        return 0;
    }

    // 函数局部静态（ORIG __cxa_guard 保护）：候选任务列表 + 随机骰。
    static std::list<int> selected_quest;
    selected_quest.clear();

    // [b28d-solo-Dispatchers] ORIG local_7589 = 0x7531 字节快照（memcpy 直填，
    //   0x081df57c mov $0x7531 / 0x081df58c call memcpy；全函数**无** CQuestClear
    //   构造调用）。ORIG 有 D1Ev ×2（0x081df71f 正常 + 0x081df739 EH）。
    // [Q4DispatchBig 2026-09-11 · 缺对象型缺口修复] 旧探针试的是**默认构造形**
    //   （`WongWork::CQuestClear cleared;`）⇒ 必发 C1Ev（ORIG 无）故回滚。
    //   正解 = 旧注释漏看的**拷贝构造形**：`CQuestClear cleared(src)` 的隐式拷贝
    //   构造内联为 memcpy（不发 C1Ev），而 ~CQuestClear(){} 非平凡 ⇒ 正常尾 + EH
    //   各发 D1（正是 ORIG 的 0x081df71f/0x081df739）。同仓 UserQuest.cpp:930
    //   已有同形成功范例；QuestClear.h 记 sizeof(CQuestClear)=0x7531（= ORIG memcpy 长度）。
    WongWork::CQuestClear cleared(user->getCurCharacQuestR()->m_cleared);

    stSelectQuestParam selectParam(user);
    // ORIG：getQuestNPCList/allowable_questlist_as_npc 各自写完整表达式
    //（两次 G_CDataManagerv 调用，不缓存 questList 指针）
    std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_8);
    G_CDataManager()->m_questList->allowable_questlist_as_npc(
        selected_quest, *npcList, selectParam, cleared, true);

    if (selected_quest.empty())
    {
        res.m_errorCode = 4;
    }
    else
    {
        static CMTRand urgentRandomDice;  // ORIG __cxa_guard 静态
        unsigned long range = (unsigned long)(selected_quest.size() - 1);
        unsigned int dice = urgentRandomDice.randInt(range);
        if (dice < selected_quest.size())
        {
            std::list<int>::iterator it = selected_quest.begin();
            std::advance(it, dice);
            int selectedIdx = *it;
            user->getCurCharacQuestW()->m_authenData.m_urgentQuest = selectedIdx;
            res.m_questIdx = selectedIdx;
            res.m_found = 1;
        }
    }
    return 0;
}