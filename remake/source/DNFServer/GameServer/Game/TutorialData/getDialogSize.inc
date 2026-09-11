// ORIG _ZN12TutorialData13getDialogSizeEj @0x08ab8486
unsigned int TutorialData::getDialogSize(unsigned int key)
{
    DialogMap::iterator it = m_dialogs.find(static_cast<int>(key));
    if (it == m_dialogs.end()) {
        return 0;
    }
    return static_cast<unsigned int>(it->second.size());
}