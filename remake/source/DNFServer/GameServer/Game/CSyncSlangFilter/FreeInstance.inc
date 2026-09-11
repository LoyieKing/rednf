void CSyncSlangFilter::FreeInstance()
{
    if (instance)
    {
        delete instance;
        instance = 0;
    }
}