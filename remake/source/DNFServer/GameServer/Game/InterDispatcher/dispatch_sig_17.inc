int Inter_Terminate::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    puts("Game Server is Terminated");
    exit(0);
}