bool CharString::endsWith(const char* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(strlen(pat));
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad6e02）：早退 `if (length() < patLength)
    // return false;` 保持；残差仅 `length() - patLength` 物化段（同 right 族），
    // 改左结合 `c_str() + length() - patLength` 复现 ORIG 六段
    // （P205 拆形族；旧 N-86b 判词作废）。
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return strncasecmp(c_str() + length() - patLength, pat, patLength) == 0;
    return strncmp(c_str() + length() - patLength, pat, patLength) == 0;
}