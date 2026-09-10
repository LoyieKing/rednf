int Dispatcher_UseRightOfChangeGrowType::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_RIGHT_OF_CHANGE_GROW_TYPE& mref = reinterpret_cast<MSG_RIGHT_OF_CHANGE_GROW_TYPE&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    *(int*)((char*)&param + 4) = check_error(user, msg);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x93, "Dispatcher Check_Error No(%d)", err);
        return LineFunc(0x94,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2a);
    if (secu != 0)
    {
        *(int*)((char*)&param + 4) = secu;
        return 0;
    }
    // ORIG：(int)(char)msg+0xf（movzbl+movsbl），(short)msg+0xd（movzwl+movswl）
    *(int*)((char*)&param + 4) =
        user->ChangeGrowType_Item(
            static_cast<signed char>(mref.m_0f),
            static_cast<short>(mref.m_0d));
    user->_SaveCharacData();
    return 0;
