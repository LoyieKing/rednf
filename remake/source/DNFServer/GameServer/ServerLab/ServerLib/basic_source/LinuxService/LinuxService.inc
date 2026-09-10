LinuxService::LinuxService()
{
    theInstance = this;
    isTerminated_ = false;
    mbStopRecevied = false;
