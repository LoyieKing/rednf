bool Thread::begin()
{
    states_.setStarted();
    id_ = pthread_create(&handle_, NULL, proxyLoop, this);
    if (id_ < 0)
    {
        throw Exception("[Thread::begin] can't begin thread");
    }
    return true;
