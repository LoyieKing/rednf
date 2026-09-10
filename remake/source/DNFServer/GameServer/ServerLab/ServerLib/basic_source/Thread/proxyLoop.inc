void* Thread::proxyLoop(void* temp)
{
    Thread* inst = (Thread*)temp;
    inst->loop(temp);
    return 0;
