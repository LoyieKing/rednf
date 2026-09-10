void WriteLog(char const* fileName, char const* fmt, ...)
{
    FILE* fp = fopen(fileName, "ab");
    // [b28c-34] ORIG 0x0858c7fe `je <+0x74 nop 岛>` + body 尾 `jmp +0x75`：
    // 4.1 if-join 空块占位岛；4.4 对正向 if 块恒 jump-thread ⇒ 用负条件
    // goto 共享尾空标签复现（P223 定案）。
    if (fp == 0) goto end;
    {
        char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        va_end(ap);
        fputs(buf, fp);
        fflush(fp);
        fclose(fp);
    }
end:;
