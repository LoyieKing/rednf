IInterHandler* IHandlers::getInterHandler(int category)
{
    std::map<int, IInterHandler*>::iterator iter =
        mInterHandlers.find(category);
    if (iter != mInterHandlers.end())
    {
        return mInterHandlers[category];
    }
    return NULL;
