int getActiveStatusDamageIndex(ENUM_ACTIVESTATUS status)
{
    int result = -1;
    switch (static_cast<int>(status))
    {
    case 2:  result = 0; break;
    case 6:  result = 1; break;
    case 7:  result = 2; break;
    case 9:  result = 3; break;
    case 11: result = 4; break;
    }
    return result;
