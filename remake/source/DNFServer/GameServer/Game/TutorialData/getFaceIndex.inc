// ORIG _ZN12TutorialData12getFaceIndexEjj @0x08ab85de（参数语义同 getDialogStr）。
int TutorialData::getFaceIndex(unsigned int idx, unsigned int key)
{
    DialogMap::iterator it = m_dialogs.find(static_cast<int>(key));
    if (it == m_dialogs.end()) {
        return 0;
    }
    // 同 getDialogStr：ORIG 0x08ab8654 seta 物化 + 正极性布局。
    if (idx < it->second.size()) {
        return it->second.at(idx)->m_faceIndex;
    }
    return 0;
}