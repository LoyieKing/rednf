void CStreamBuffer::AppendString(char* dst, int maxlen, char* src)  // 0x086269ea
{
    unsigned int slen;
    unsigned int n;
    n = maxlen - strlen(dst) - 1;
    slen = strlen(src);
    strncat(dst, src, std::min(slen, n));
}