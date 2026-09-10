void IArea::notifyAllAreaMember()
{
    MAP_OBJECTS_ITER mapIter;
    mapIter = mMapObj.begin();
    while (mapIter != mMapObj.end())
    {
        ++mapIter;
    }
