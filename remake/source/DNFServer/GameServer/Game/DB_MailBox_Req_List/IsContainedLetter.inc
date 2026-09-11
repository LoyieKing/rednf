// [round5-A29] 原本 TU 的 pack(1) cast 视图（LetterPostalEntry/LetterListView/
//   LetterCountView）已全部删除：postal 区三处取值统一改走 **权威头
//   SIG_MAILBOX_LIST::m_box.m_postals[i].m_tail.m_occ**（参数类型自身成员，
//   P235；ORIG 「下标先、基址后」+ `imul $0x61; lea; add $0x50` 装载序复现），
//   不再有 cast 视图 ⇒ 无死代码、无第二套偏移定义。
// ============================================================================
// IsContainedLetter —— ORIG 0x0841ccb6：postal 区（stride 0x61）扫 +0x5c
//   letter_id 是否含目标。
bool IsContainedLetter(SIG_MAILBOX_LIST* list, int letterId)
// [round5-A29 破 N] 旧判词 N18b + P30/P235「cast 视图 base 先」两项一并作废：
//   正解 = **权威头 SIG_MAILBOX_LIST 具名嵌套视图**（参数类型自身成员 ⇒ 4.4 出
//   「下标先、基址后」+ `imul $0x61; lea; add $0x50`）+ **字段在左**的比较
//   （m_tail.m_occ.m_letterId 为 unsigned int、形参与 int 异型 ⇒ P274 两段装载
//   `mov 0xc(%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`）。
//   同族首证：CMailBoxHelper.cpp `isUnlimitMail` 0x0855254c 用同一配方 DIFF→AE
//   （SigTypes_D.h 的 m_box.m_postals[i].m_tail.m_occ.* 视图，b28d 已广播落地）。
//   ORIG 0x0841ccb6 逐条同形；p167 不适用（无 call）。
{
    for (int i = 0; i < list->m_box.m_count; ++i)
    {
        if (list->m_box.m_postals[i].m_tail.m_occ.m_letterId == letterId)
            return 1;
    }
    return 0;
}