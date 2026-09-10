int getItemIndex(const std::vector<std::pair<int, int> >& items, int rand)
{
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    for (; it != items.end(); ++it)
    {
        if (it->second > rand)
            break;
    }
    if (it == items.end())
        return 0;
    return it->first;
