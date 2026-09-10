bool DB_Update::dispatch(int /*uid*/, int /*uniqueId*/, Stream* stream)
{
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    update(*data);
    return true;
