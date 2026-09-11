// ===========================================================================
// HP_GetHackType  ORIG T 0x82756cf（jump table .rodata 0x8bfefc8，41 项）
// type<=0x29 查表；表项 idx 0/越界 → 0x384；idx 1..9 → 0x385..0x38d；
// idx 10..40 → 0x578..0x596。
// ===========================================================================
int HP_GetHackType(unsigned char type)
{
    switch (type)
    {
        // case 0 与 default 合并（ORIG 跳转表槽 0 直指 default 标签，单处 0x384）
        case 1:   return 0x385;
        case 2:   return 0x386;
        case 3:   return 0x387;
        case 4:   return 0x388;
        case 5:   return 0x389;
        case 6:   return 0x38a;
        case 7:   return 0x38b;
        case 8:   return 0x38c;
        case 9:   return 0x38d;
        case 10:  return 0x578;
        case 11:  return 0x579;
        case 12:  return 0x57a;
        case 13:  return 0x57b;
        case 14:  return 0x57c;
        case 15:  return 0x57d;
        case 16:  return 0x57e;
        case 17:  return 0x57f;
        case 18:  return 0x580;
        case 19:  return 0x581;
        case 20:  return 0x582;
        case 21:  return 0x583;
        case 22:  return 0x584;
        case 23:  return 0x585;
        case 24:  return 0x586;
        case 25:  return 0x587;
        case 26:  return 0x588;
        case 27:  return 0x589;
        case 28:  return 0x58a;
        case 29:  return 0x58b;
        case 30:  return 0x58c;
        case 31:  return 0x58d;
        case 32:  return 0x58e;
        case 33:  return 0x58f;
        case 34:  return 0x590;
        case 35:  return 0x591;
        case 36:  return 0x592;
        case 37:  return 0x593;
        case 38:  return 0x594;
        case 39:  return 0x595;
        case 40:  return 0x596;
        case 41:  return 0x597;
        default:  return 0x384;
    }
}