CCreatureScript::~CCreatureScript()
{
    if (m_artifactSlotFlags != 0)
    {
        delete[] m_artifactSlotFlags;
        m_artifactSlotFlags = 0;
    }
