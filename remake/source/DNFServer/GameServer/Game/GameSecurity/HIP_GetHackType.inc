// ===========================================================================
// HIP_GetHackType  ORIG T 0x827585c（jump table .rodata 0x8bff070，17 项）
// type<=0x10 查表：idx 0→0x5dc；1..4→0x5dd..0x5e0；5→0x5dc；6..8→0x5e2..0x5e4；
// 9..11→0x641..0x643；12→0x5dc；13..15→0x646..0x648；16→0x679；越界→0x5dc。
// ===========================================================================
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
}