ISession* IArea::getMemberAt(unsigned int id)
{
    MAP_OBJECTS_ITER mapIter;
    mapIter = mMapObj.find(id);
    if (mapIter != mMapObj.end())
    {
        return mapIter->second;
    }
    return NULL;
