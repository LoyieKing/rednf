// [report dispatch.md / objdump 0x813ec24]：不调基类前置，直接取 OutBuffer → update。
bool DB_Update::dispatch(int /*uid*/, int /*uniqueId*/, Stream* stream)
{
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    update(*data);
    return true;
}