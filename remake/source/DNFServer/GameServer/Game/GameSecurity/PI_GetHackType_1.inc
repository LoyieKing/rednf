// ===========================================================================
// PI_GetHackType  ORIG W 0x82758e3（符号 _Z14PI_GetHackTypes = 本名自然 mangling）
// jump table（.rodata 0x8bff0b4，55 项）逐项转写：idx 0→0x514；
// 1..9→0x515..0x51d；10→0x54a；11..52→0x51e..0x547；53→0x548；54→0x549；
// 越界/负值（movswl 符号扩展后 ja）→ 0x514。
// ===========================================================================
int PI_GetHackType(short hackType)
{
    // ORIG @0x082758e3 入口仅 `mov %ax,-0x4(%ebp); movswl; cmp $0x36; ja`——
    // 无显式边界检查（P140），range 由 switch 自身产生。补 `case 0`（与
    // default 共享 0x514 块）把 4.4 的判定 range 撑成 0..0x36=55 项，与 ORIG
    // 跳转表项数一致（P163）。
    switch (hackType)
    {
        case 1:  return 0x515;
        case 2:  return 0x516;
        case 3:  return 0x517;
        case 4:  return 0x518;
        case 5:  return 0x519;
        case 6:  return 0x51a;
        case 7:  return 0x51b;
        case 8:  return 0x51c;
        case 9:  return 0x51d;
        case 10: return 0x54a;
        case 11: return 0x51e;
        case 12: return 0x51f;
        case 13: return 0x520;
        case 14: return 0x521;
        case 15: return 0x522;
        case 16: return 0x523;
        case 17: return 0x524;
        case 18: return 0x525;
        case 19: return 0x526;
        case 20: return 0x527;
        case 21: return 0x528;
        case 22: return 0x529;
        case 23: return 0x52a;
        case 24: return 0x52b;
        case 25: return 0x52c;
        case 26: return 0x52d;
        case 27: return 0x52e;
        case 28: return 0x52f;
        case 29: return 0x530;
        case 30: return 0x531;
        case 31: return 0x532;
        case 32: return 0x533;
        case 33: return 0x534;
        case 34: return 0x535;
        case 35: return 0x536;
        case 36: return 0x537;
        case 37: return 0x538;
        case 38: return 0x539;
        case 39: return 0x53a;
        case 40: return 0x53b;
        case 41: return 0x53c;
        case 42: return 0x53d;
        case 43: return 0x53e;
        case 44: return 0x53f;
        case 45: return 0x540;
        case 46: return 0x541;
        case 47: return 0x542;
        case 48: return 0x543;
        case 49: return 0x544;
        case 50: return 0x545;
        case 51: return 0x546;
        case 52: return 0x547;
        case 53: return 0x548;
        case 54: return 0x549;
        case 0:
        default: return 0x514;
    }
}