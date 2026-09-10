bool CEventCharacterHandler::_SetGrowType(CUser* user, int growTypeKind, int growType)
{
    int first = 0;
    int second = 0;
    if (growTypeKind == 1)
    {
        first = growType;
    }
    else if (growTypeKind == 2)
    {
        if (user->getCurCharFirstGrowType() == 0)
        {
            return false;
        }
        first = user->getCurCharFirstGrowType();
        second = growType;
    }
    else
    {
        return false;
    }

    if (user->ChangeGrowType_GM(first, second))
    {
        return true;
    }
    return false;
