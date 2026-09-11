bool COnlinePreliminary::CheckSeat(int a)
{
    /* ORIG @085608c3 cmpl $2 / jne +0x10(mov $1)、fall-through mov $0、jmp +0x15(leave)
     * ——守卫早返 `if(a==2)return 0;`+尾 `return 1;` 精确复现（三目被折叠成 setne，goto 形
     * 多一 nop 中继；P133 独立守卫形）。 */
    if (a == 2)
        return 0;
    return 1;
}