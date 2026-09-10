void IHandler::init()
{
    setSendThread(pApp->super_Threads.getTCPSendThread());
    pTimeHandler = pApp->super_IHandlers.getTimeHandler(0);
