bool StreamPool::Free(CStreamGuard& guard)
{
    return Free(guard.Get());
