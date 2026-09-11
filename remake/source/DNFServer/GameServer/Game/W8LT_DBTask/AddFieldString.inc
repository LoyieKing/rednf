// ============================================================================
// ORIG 实证（0x0841fff8 反汇编）：tmp[100] 先于检查零初始化（P165 物理展开序），
// va_start 在 (mask&flag) 检查前；isFirst==true 仅清标志，isFirst==false 时
// 前置 ","（ORIG 以 2 字节 local_e 存 "," 并 strlen 之，movzwl 物化→N 残差），
// 两分支均落入共同尾：strncat(buf,tmp,strlen(tmp))。
// ============================================================================
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 0841fff8
void AddFieldString(bool& isFirst, char* buf, int mask, int flag,
                    const char* fmt, ...)
{
    char tmp[100] = { '\0' };
    // ORIG 0x08420013 `movzwl <A>,%eax; mov %ax,-0xa(%ebp)` = 局部数组
    // `char szComma[] = ","`（P244 两跳：字面量先落槽再入栈实参）；直接
    // 传字面量会被 4.4 折成 memcpy（P167 EXTRA）。
    char szComma[] = ",";
    va_list ap;
    va_start(ap, fmt);
    // P257 正向包裹：早退 return/goto 使 4.4 在函数尾产 `jmp <epilogue>` 岛
    // （ORIG 0x0842009b 后直落 0x84200a0），正向 if 包裹后 je 直达 epilogue。
    if (mask & flag)
    {
        vsprintf(tmp, fmt, ap);
        va_end(ap);
        // ORIG 0x0842004f `xor $1,%eax; test %al,%al; je <else>`（P292：bool
        // 取反物化）⇒ 源码为 `if (!isFirst) strncat(...); else isFirst=false;`。
        if (!isFirst)
            strncat(buf, szComma, strlen(szComma));
        else
            isFirst = false;
        strncat(buf, tmp, strlen(tmp));
    }
}