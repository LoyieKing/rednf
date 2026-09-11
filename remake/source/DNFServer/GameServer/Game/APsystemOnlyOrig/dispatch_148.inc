// ============================================================================
// DB_ForceSaveSimpleSSO::dispatch —— ORIG 0x08428154（报告 dispatch.md 全文）
// ============================================================================
// 解包 mId(uint)/errCnt(int) → GlobalData::s_psimpleSSO->updateHumanCertify(
// mId, errCnt)；非 1 返 0，否则返 1（ORIG xor $1/test/je + mov $0/$1 分支）。
bool DB_ForceSaveSimpleSSO::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    unsigned int mId;    // -0xc（ORIG lea 首参解包位）
    int errCnt;          // -0x10
    stream->operator>>(mId);
    stream->operator>>(errCnt);
    if (!GlobalData::s_psimpleSSO->updateHumanCertify(mId, errCnt))
        return 0;
    return 1;
}