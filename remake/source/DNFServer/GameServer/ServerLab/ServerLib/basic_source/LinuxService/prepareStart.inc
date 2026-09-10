void nsl::LinuxService::prepareStart()
{
    Neof_setCoreLimit();
    Neof_registerSignalHandlers();
