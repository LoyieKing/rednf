int isSyncActiveStatus(int type)
{
    switch (type)
    {
    case 2:
    case 6:
    case 7:
    case 9:
    case 11:
        return 1;
    }
    return 0;
