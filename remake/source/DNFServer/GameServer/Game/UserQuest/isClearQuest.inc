// [b28c-L9] SIGFIX:UserQuest::isClearQuest —— ORIG 0x086ab920 体为
// `call _ZNK8WongWork11CQuestClear14isClearedQuestEj; leave; ret` 直返无归一化；
// callee 权威型 unsigned char（QuestClear.h:41）⇒ caller 同型化（P9/P174）。
bool UserQuest::isClearQuest(int questIdx) const
{
    return m_cleared.isClearedQuest(questIdx);
}