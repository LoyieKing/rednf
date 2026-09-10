int getModuleTypeFromTag(std::string& tag)
{
    for (int i = 0; i < 17; ++i)
    {
        if (tag == s_moduleNameTag.entries[i].open)
            return i;
    }
    return -1;
