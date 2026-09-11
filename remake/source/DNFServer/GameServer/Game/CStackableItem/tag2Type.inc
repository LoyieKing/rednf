// ORIG T 0x089c2832：if-return 链 + strcmp（每分支 `mov $N,%eax; jmp` 统一出口，
// 尾项默认 `mov $0x9,%eax` 直落；P17/P47 形态）。字符串映射（.rodata 0x8e19e71..）：
//   "[avatar]"→0 "[special avatar]"→1 "[cera]"→2 "[creature]"→3 "[equipment]"→4
//   "[stackable]"→5 "[etc]"→6 "[special creature]"→7 "[emblem]"→8，未知→9。
int stStackableBooster_t::tag2Type(const char* tag)
{
    if (strcmp(tag, "[avatar]") == 0) return 0;
    if (strcmp(tag, "[special avatar]") == 0) return 1;
    if (strcmp(tag, "[cera]") == 0) return 2;
    if (strcmp(tag, "[creature]") == 0) return 3;
    if (strcmp(tag, "[equipment]") == 0) return 4;
    if (strcmp(tag, "[stackable]") == 0) return 5;
    if (strcmp(tag, "[etc]") == 0) return 6;
    if (strcmp(tag, "[special creature]") == 0) return 7;
    if (strcmp(tag, "[emblem]") == 0) return 8;
    return 9;
}