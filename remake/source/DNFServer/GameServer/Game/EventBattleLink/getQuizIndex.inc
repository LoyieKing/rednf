int CBingo::getQuizIndex()
{
    int i = 0;
    while (i < static_cast<int>((reinterpret_cast<std::vector<int>*>(this))->size()))
    {
        if ((*(reinterpret_cast<std::vector<int>*>(this)))[i] == 0)
        {
            return i;
        }
        ++i;
    }
    return -1;
