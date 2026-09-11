// ORIG 0x0858c71e _ZN6OS_API14WriteLoginInfoEPKcS1_z：
// fopen(path,"wb") 成功则 vsprintf 到 1024 栈缓冲 → fputs → fclose。
void WriteLoginInfo(char const* path, char const* fmt, ...)
{
    char buf[1024];
    va_list ap;
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        va_end(ap);
        fputs(buf, f);
        fclose(f);
    }
    return;
}