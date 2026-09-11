// ORIG _ZN12TutorialData14addAvatarIndexEii @0x08ab868a：
// make_pair(p1,p2) 插入 this+0x1c。
void TutorialData::addAvatarIndex(int type, int index)
{
    m_avatarIndexMap.insert(std::make_pair(type, index));
}