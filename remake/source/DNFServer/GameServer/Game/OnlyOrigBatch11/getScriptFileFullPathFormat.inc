// ORIG T 0x088bbbc5：vsnprintf 到临时缓冲（0x104 上限），再拼到
// g_szScriptFileBaseDirectory 后（ss_strcpy + ss_strcat）。
void getScriptFileFullPathFormat(char* out, unsigned int size,
                                 const char* fmt, ...)
{
    char buf[0x104];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, 0x104, fmt, ap);
    va_end(ap);
    ss_strcpy(out, size, g_szScriptFileBaseDirectory);
    ss_strcat(out, size, buf);
}