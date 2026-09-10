bool haveChampionFeature(std::vector<int>* features, CHAMPION_FEATURES feature)
{
    for (std::vector<int>::size_type i = 0; i < features->size(); ++i)
    {
        if ((*features)[i] == static_cast<int>(feature))
            return true;
    }
    return false;
