void tagRecommendNewbieOptionTable::clear()
{
    for (int i = 0; i <= 0xa; ++i)
        push_back(std::vector<tagRecommendNewbieOption>());
    for (int i = 0; i <= 0xa; ++i)
    {
        operator[](i).clear();
        operator[](i).reserve(6);
    }
