Thread::~Thread()
{
    if (states_.isStarted() && !isTerminated())
    {
        throw Exception("[Thread::~Thread] abnormal destruction. you should terminate the thread,first.");
    }
