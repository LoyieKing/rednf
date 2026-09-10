void CCompoundAvatar::ProcCompound(CUser* user, const MSG_COMPOUND_AVATAR* msg)
{
    try
    {
        tagCompoundInfo info = _MakeCompoundInfo(user, msg);
        int error = 0;
        error = _CheckCondition(user, info);
        while (error != 0)
        {
            throw (ENUM_ERROR)error;
        }
        tagCompoundAvatarTable* table = &m_tables[info.m_field00Job];
        error = _ProcCompoundCore(user, info, table);
        while (error != 0)
        {
            throw (ENUM_ERROR)error;
        }
        _SendResult(user, info);
        _SendDisjointAvatarStatistic(info);
    }
    catch (ENUM_ERROR e)
    {
        _SendError(user, e);
    }
