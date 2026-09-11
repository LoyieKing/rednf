// ORIG _ZN12TutorialData14TutorialNoticeC1Ev @0x08ab985e：
// 仅默认构造 this+4 的 std::string；其余 int 成员不初始化。
TutorialData::TutorialNotice::TutorialNotice()
    : m_aniPath()
{
}