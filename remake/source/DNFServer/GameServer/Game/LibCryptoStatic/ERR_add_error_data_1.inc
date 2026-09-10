void ERR_add_error_data(int count, ...)              // ORIG T 0x087ee240→vdata
{
    va_list ap;
    va_start(ap, count);
    char buf[256];
    buf[0] = '\0';
    for (int i = 0; i < count; ++i)
    {
        const char* piece = va_arg(ap, const char*);
        strncat(buf, piece ? piece : "", sizeof(buf) - strlen(buf) - 1);
    }
    va_end(ap);
    const int slot = (g_errTop + 15) & 0xf;          // 最近写入槽（补回退一位）
    strncat(g_errRing[slot].data, buf,
            sizeof(g_errRing[0].data) - strlen(g_errRing[slot].data) - 1);
