bool DB_ForceSaveSimpleSSO::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    unsigned int mId;    // -0xc（ORIG lea 首参解包位）
    int errCnt;          // -0x10
    stream->operator>>(mId);
    stream->operator>>(errCnt);
    if (!GlobalData::s_psimpleSSO->updateHumanCertify(mId, errCnt))
        return 0;
    return 1;
