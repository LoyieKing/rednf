// ORIG 0x081c98c8 T _ZN17DisPatcher_SetPos7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_SetPos::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {   // LineFunc(0x1cb8, pretty, 0, acc_id)——第三参恒 0、第四参 acc_id（ORIG 实证）
        return LineFunc(0x1cb8,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    MSG_USER_SET_POS& req = *reinterpret_cast<MSG_USER_SET_POS*>(&msg);
    G_GameWorld()->move_position(user,
                                 (int)req.m_0d,          // x（ushort 零扩展）
                                 (int)req.m_0f,          // y
                                 (int)req.m_11,          // z（sbyte 符号扩展）
                                 (unsigned short)req.m_12);  // dir
    return err;   // 该分支 err==0，与 ORIG `mov -0x20(%ebp),%eax` 一致
}
