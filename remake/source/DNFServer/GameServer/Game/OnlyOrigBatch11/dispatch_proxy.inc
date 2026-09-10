void* ThreadInterface::dispatch_proxy(void* p)
{
    ThreadInterface* self = static_cast<ThreadInterface*>(p);
    self->dispatch(p);
    return 0;
