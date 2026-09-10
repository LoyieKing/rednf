bool WongWork::CSimpleSSO::getGameStartTime(unsigned int mId, long& loginTime,
                                            long& expireTime, stOutputData_t& out)
{
    return _getGameStartTime(mId, loginTime, expireTime, out);
