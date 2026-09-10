bool Script::get_sections()
{
    const char* p = 0;
    unsigned int i = 0;
    for (; i < mParentNames.size(); i = i + 1)
    {
        unsigned int j = 0;
        for (; j < mChildNames[i].size(); j = j + 1)
        {
            p = mChildValues[i][j].c_str();
            if (p == 0)
            {
                return false;
            }
        }
    }
    return true;
