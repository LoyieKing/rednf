bool isWideStrInUnicodeRange(const wchar_t* str)
{
    IsNoneUnicodeRange pred;
    size_t len = wcslen(str);
    // P0：ORIG 把 any_of 结果物化到 bool 槽 -0x9（mov %al,-0x9(%ebp)），
    // 再以 cmpb $0,-0x9 / je 两支各 mov $0/$1 返回 → 源码为 if 展开形态
    // （`return !found;` 会被 4.4.7 编成 movzbl+xor，与 ORIG 分支形态不符）。
    bool found = std::any_of(str, str + len, pred);
    if (found)
        return false;
    return true;
