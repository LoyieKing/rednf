// ---- getGameStartTime(...)  ORIG 0x086031f6：薄转发 _getGameStartTime
// [b23S3C] bool 透传：ORIG 体末 `call _getGameStartTime; leave; ret`（%eax 直带
// al 结果，无槽），调用点 0x0818f7a8 xor$1/test%al 实证返回承载成败。
bool WongWork::CSimpleSSO::getGameStartTime(unsigned int mId, long& loginTime,
                                            long& expireTime, stOutputData_t& out)
{
    return _getGameStartTime(mId, loginTime, expireTime, out);
}