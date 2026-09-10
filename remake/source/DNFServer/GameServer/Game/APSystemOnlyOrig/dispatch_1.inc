bool DB_UpdateActionPoint::dispatch(int a, int b, Stream* stream)
{
    (void)a;
    (void)b;
    bool expire = false;
    (*stream) >> expire;
    _SIG_LOAD_ACTION_POINT* sig = stream->GetOutBuffer<_SIG_LOAD_ACTION_POINT>();
    if (sig == 0)
    {
        return false;
    }
    Update(expire, *sig);
    return true;
