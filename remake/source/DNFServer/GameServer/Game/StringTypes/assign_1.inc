void CharString::assign(const char* src)
{
    assign(src, static_cast<int>(strlen(src)));
}