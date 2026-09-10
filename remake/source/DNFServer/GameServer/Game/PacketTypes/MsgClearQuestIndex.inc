MsgClearQuestIndex::MsgClearQuestIndex() : m_questIdxs() {}  // ORIG 0x8231a3a：仅 vector C1
// ORIG 0x08235266（W）：逐标量拷贝 +0x00..+0x08 / +0x0c / +0x10，跳过 +0x09..+0x0b
// 与 +0x0d..+0x0f（编译器逐成员拷贝语义，padding 不拷贝），m_questIdxs 走
// std::vector<int>::operator=。
