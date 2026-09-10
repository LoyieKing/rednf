bool script_tag::isTypeString(std::string& str)
{
    // [b28c-lane17] ORIG 0x08a0c8fa：同 isCloseString（size()>1 && at(0)=='['）。
    bool result = false;
    if (str.size() > 1)
    {
        if (str.at(0) == '[')
            result = true;
    }
    return result;
