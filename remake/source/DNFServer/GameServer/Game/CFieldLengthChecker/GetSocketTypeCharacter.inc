int GetSocketTypeCharacter(unsigned short type)
{
    switch (type)
    {
    case 1:
        return '1';
    case 2:
        return '2';
    case 4:
        return '3';
    case 8:
        return '4';
    case 0x10:
        return '5';
    case 0xffef:
        return '6';
    case 0xffff:
        return '7';
    default:
        return 0;
    }
