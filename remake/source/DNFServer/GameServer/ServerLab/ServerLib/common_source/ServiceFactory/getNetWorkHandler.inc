INetWorkHandler* IHandlers::getNetWorkHandler(int category)
{
    std::map<int, INetWorkHandler*>::iterator iter =
        mNetWorkHandlers.find(category);
    if (iter != mNetWorkHandlers.end())
    {
        return mNetWorkHandlers[category];
    }
    return NULL;
