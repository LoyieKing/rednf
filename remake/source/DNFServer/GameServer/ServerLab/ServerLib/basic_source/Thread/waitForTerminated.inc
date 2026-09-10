void Thread::waitForTerminated(int nTimeSleep)
{
    setTerminate();
    while (!isTerminated())
    {
        TSystem<LinuxSystem>::sleep(nTimeSleep);
    }
