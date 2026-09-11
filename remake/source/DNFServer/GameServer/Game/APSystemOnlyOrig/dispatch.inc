// dispatch —— 报告 dispatch.md 全文转写。ORIG T 0x08123b10。
bool DB_LoadActionPoint::dispatch(int a, int b, Stream* stream)
{
    if (!DBDispatcher::dispatch(a, b, stream))
    {
        return false;
    }
    _SIG_LOAD_ACTION_POINT* sig = stream->GetOutBuffer<_SIG_LOAD_ACTION_POINT>();
    if (sig == 0)
    {
        return false;
    }
    bool ok = Select(*sig);
    return SendResult(a, b, *sig, ok);
}