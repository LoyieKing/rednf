CSyncSlangFilter* CSyncSlangFilter::GetInstance()
{
    if (instance == 0)
        instance = new CSyncSlangFilter;
    return instance;
