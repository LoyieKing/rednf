int Dispatcher_Buy_Charac_Status_Using_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_QUEST_SHOP& mref = reinterpret_cast<MSG_QUEST_SHOP&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {
        return LineFunc(0x3fc9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG GetCharacExpandData(param_1, 0xb)（扩展数据 0xb 槽）
    CQuestShop* shop = reinterpret_cast<CQuestShop*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xb)));
    if (shop == 0)
    {
        *(int*)((char*)&param + 8) = 1;
        return 0;
    }
    short result = 0;   // local_22
    int ok = shop->buyStatus(user,   // ORIG 返回码 0/1/0xe（param+8 需完整值）
                             mref.m_0d,
                             mref.m_0e,
                             result);
    if (ok)
    {
        *(unsigned char*)((char*)&param + 4) =
            mref.m_0d;   // param+4 ← msg+0xd
        *(short*)((char*)&param + 6) = result;      // param+6 ← local_22
        return 0;
    }
    *(int*)((char*)&param + 8) = ok;   // ORIG param+8 = buyStatus 返回码
    return 0;
