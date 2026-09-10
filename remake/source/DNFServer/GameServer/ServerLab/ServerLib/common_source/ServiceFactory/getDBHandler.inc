IDBHandler* IHandlers::getDBHandler(int category)
{
    std::map<int, IDBHandler*>::iterator iter =
        mDBHandlers.find(category);
    if (iter != mDBHandlers.end())
    {
        return mDBHandlers[category];
    }
    return NULL;
