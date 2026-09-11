// ORIG _ZN12TutorialData13addDialogDataEiiPKc @0x08ab82dc
void TutorialData::addDialogData(int key, int faceIndex, char const* text)
{
    DialogMap::iterator it = m_dialogs.find(key);

    DialogData* data = new DialogData;
    data->m_faceIndex = faceIndex;
    data->m_str = text;

    if (it != m_dialogs.end()) {
        it->second.push_back(data);
        return;
    }

    std::vector<DialogData*> entries;
    entries.push_back(data);
    m_dialogs.insert(std::make_pair(key, entries));
}