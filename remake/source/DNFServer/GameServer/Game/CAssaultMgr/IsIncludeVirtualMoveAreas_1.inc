bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y)
{
    for (unsigned int i = 0; i < areas->size(); ++i)
    {
        if (!areas->at(i).include(x, y))
            return false;
    }
    return true;
