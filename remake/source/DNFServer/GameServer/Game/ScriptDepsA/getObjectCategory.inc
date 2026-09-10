int getObjectCategory(std::string name)
{
    std::map<std::string, ENUM_CATEGORY, IgnoreCaseCompare>::iterator it =
        s_objCategoryString.find(name);
    if (it != s_objCategoryString.end())
        return (*it).second;
    return 0x2c;
