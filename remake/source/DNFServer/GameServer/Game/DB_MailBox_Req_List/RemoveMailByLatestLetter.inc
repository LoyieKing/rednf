bool RemoveMailByLatestLetter(SIG_MAILBOX_LIST* list, int mailIdx);

// [round5-A29] 原本 TU 的 pack(1) cast 视图（LetterPostalEntry/LetterListView/
//   LetterCountView）已全部删除：postal 区三处取值统一改走 **权威头
//   SIG_MAILBOX_LIST::m_box.m_postals[i].m_tail.m_occ**（参数类型自身成员，
//   P235；ORIG 「下标先、基址后」+ `imul $0x61; lea; add $0x50` 装载序复现），
//   不再有 cast 视图 ⇒ 无死代码、无第二套偏移定义。
// ============================================================================
// IsContainedLetter —— ORIG 0x0841ccb6：postal 区（stride 0x61）扫 +0x5c
//   letter_id 是否含目标。
