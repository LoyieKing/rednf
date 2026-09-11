// ---------------------------------------------------------------------------
// ProcCompound —— T 0x083338c2。
// Make → CheckCondition(错误抛 ENUM_ERROR) → Core(同) → Result + 统计；
// 全程捕获 ENUM_ERROR 转 _SendError。
// [b28d-solo-WongWorkMiscB_A] ORIG 0x083338c2 逐点对应：①error 预清 0 后赋值
// （P198①：`movl $0x0,-0x14` 在 _CheckCondition 之前）；②错误判定用
// `while (error != 0) { throw; }` 复现 4.1 `setne %al; test %al,%al; je`
// 物化（P225；4.4 对槽比较恒直跳）；③table 具名局部（ORIG 有 -0x10 槽往返，
// P0 槽-局部律）。
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
}