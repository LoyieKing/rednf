int ContentsType::getType(const char* name)
{
    for (int i = 0; i <= 12; ++i)
    {
        std::string expected(getString((ContentsType::T)i));
        std::string current(name);
        if (current == expected)
            return i;
    }
    return 0;
