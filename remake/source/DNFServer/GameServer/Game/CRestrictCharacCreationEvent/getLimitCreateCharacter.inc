int CRestrictCharacCreationEvent::getLimitCreateCharacter(int characCount) const
{
    switch (m_limitType)
    {
    case 0:
        if (characCount == 0)
        {
            return 7;
        }
        break;
    case 1:
        return 8;
    }
    return 0;
