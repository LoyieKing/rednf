// ============================================================================
// CompareLetterIndex —— ORIG 0x0841d12f：比较首字段（letter_id）严格小于。
// ============================================================================
bool CompareLetterIndex(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b)
{
    // ORIG 0x0841d12f：`return uint_a < uint_b;`（GCC4.1 生成 a<b→1; a==b→setb; a>b→0
    // 三分支）。`if(x)return 1; return 0` 隔成两分支（缺失 a==b 的 setb 路径）。
    if (*(const unsigned int*)&a < *(const unsigned int*)&b)
        return 1;
    if (*(const unsigned int*)&a == *(const unsigned int*)&b)
        return *(const unsigned int*)&a < *(const unsigned int*)&b;
    return 0;
}