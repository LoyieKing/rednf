int StrikerScript::getAddCommand(short firstGrowType, int command)
{
    for (unsigned int i = 0; i < m_macroCommands.size(); ++i)
    {
        if (firstGrowType == m_macroCommands[i].m_field00 &&
            command == m_macroCommands[i].m_field04)
        {
            return m_macroCommands[i].m_type;
        }
    }
    return 0;
