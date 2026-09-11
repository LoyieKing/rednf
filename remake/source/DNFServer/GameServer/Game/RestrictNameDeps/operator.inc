bool IsNoneUnicodeRange::operator()(wchar_t ch) const
{
    bool notInRange = !isInRange(ch);
    return notInRange;
}