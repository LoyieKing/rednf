// ============================================================================
// ORIG 0x088bbc80 getDirectoryPath(std::string&, char const*, ...)：
// vsnprintf 格式化到 512 字节栈缓冲（上限 0x200），从串尾向前回扫第一个
// '\\' 或 '/'，将其后内容截断（保留分隔符本身）后赋给 out —— 即取路径的
// 目录部分。反编译见 docs/class_func_reports/_lt_global_gt_/getDirectoryPath.md。
// ============================================================================
void getDirectoryPath(std::string& out, const char* fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, 0x200, fmt, ap);
    va_end(ap);

    int i = 0;
    i = static_cast<int>(std::strlen(buf)) - 1;
    while (i >= 0)
    {
        if (buf[i] == '\\')
            break;
        if (buf[i] == '/')
            break;
        --i;
    }
    buf[i + 1] = '\0';
    out = buf;
}