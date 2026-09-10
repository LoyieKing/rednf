TutorialData* GlobalScriptTutorial::getData(int index)
{
    if (!(index < 11 && index >= 0))
        return 0;
    {
        if (({ m_data[index].getDungeonIndex() == 0; }))
            return 0;
        return &m_data[index];
    }
