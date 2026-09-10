ISession* IArea::getMemberFirst()
{
    MAP_OBJECTS_ITER mapIter;
    if (mMapObj.begin() == mMapObj.end())
    {
        return NULL;
    }
    return mMapObj.begin()->second;
