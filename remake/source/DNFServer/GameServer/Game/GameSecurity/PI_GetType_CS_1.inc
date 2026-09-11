// ===========================================================================
// PI_GetType_CS  ORIG T 0x808c5d8（报告 _lt_global_gt_/PI_GetType_CS.md）
// type!=1 → 0；type==1 时按 cmd 查表（未命中 0）。
// ===========================================================================
char PI_GetType_CS(char type, short cmd)
{
    switch (type)
    {
        case 1:
            switch (cmd)
            {
        case 0x11b: return 1;
        case 0x11c: return 2;
        case 0x11e: return 3;
        case 0x91:  return 0x21;
        case 0x29:  return 0x23;
        case 0x108: return 0x24;
        case 0x1e:  return 0x25;
        case 0x127: return 0x26;
        case 0x126: return 0x28;
        case 0x13f: return 0x29;
        case 0x140: return 0x2a;
        case 0x2a:  return 0x2b;
        case 0x31:  return 0x2d;
        case 0x152: return 0x2e;
        case 0x16f: return 0x31;
        case 0x14e: return 0x32;
                case 0x1f:  return 10;
            }
            break;
    }
    return 0;
}