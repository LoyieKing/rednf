bool DB_Load::dispatch(int uid, int uniqueId, Stream* stream)
{
    if (!DBDispatcher::dispatch(uid, uniqueId, stream))
        return false;
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    bool ok = Select(*data);
    return SendResult(uid, uniqueId, *data, ok);   // ORIG eax = SendResult 返回值
