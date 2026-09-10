int HIP_GetHackType(unsigned char type)
{
    switch (type)
    {
        case 1:  return 0x5dd;
        case 2:  return 0x5de;
        case 3:  return 0x5df;
        case 4:  return 0x5e0;
        case 6:  return 0x5e2;
        case 7:  return 0x5e3;
        case 8:  return 0x5e4;
        case 9:  return 0x641;
        case 10: return 0x642;
        case 11: return 0x643;
        case 13: return 0x646;
        case 14: return 0x647;
        case 15: return 0x648;
        case 16: return 0x679;
        default: return 0x5dc;
    }
