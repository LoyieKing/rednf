// ORIG T 0x08ac2d6c sz 0x78（isUtf8StrInUnicodeRange.md 全文）：
// WideString 构造 → convertFromUtf8(str,(int)strlen,str 的宽度) → c_str 判定 → 析构返回。
bool isUtf8StrInUnicodeRange(const char* str)
{
    WideString wide;
    convertFromUtf8(str, (int)strlen(str), &wide);
    return isWideStrInUnicodeRange(wide.c_str());
}