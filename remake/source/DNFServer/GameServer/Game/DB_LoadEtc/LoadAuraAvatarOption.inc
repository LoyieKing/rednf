bool DB_LoadEtc::LoadAuraAvatarOption(SIG_LOAD_ETC* sig)
{
    // ORIG 0x08408394：无 aura 具名局部（add $0xd730 直传 this，P0 槽律）。
    reinterpret_cast<stAuraAvatarOption*>(sig->m_auraAvatarOption)->reset();

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect option_type, value_1 from aura_avatar_option"
                  " where charac_no=%u", sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }
    // ORIG 0x08408423：fetchOk 预清两句式（P198①）+ while 前置 jmp cond（P25）；
    // fetch 失败 break 出循环（非 return）。
    bool fetchOk = false;
    int i = 0;
    while (i < nRows)
    {
        fetchOk = db->fetch();
        if (!fetchOk)
        {
            break;
        }
        int optionType;                 // ORIG 无预清（get_int 直写）
        unsigned int value;             // [b28d-solo] 与 union 视角元素同型，消 4.4 的
                                        // int→unsigned 装载中转（探针 /tmp/dble_probe/p9.cpp m2）
        if (!db->get_int(0, optionType))
        {
            return false;
        }
        if (!db->get_int(1, *reinterpret_cast<int*>(&value)))
        {
            return false;
        }
        if (-1 < optionType && optionType < 3)
        {
            // ORIG 0x084084af：mov -0x1c,%ecx; mov -0x20,%edx; mov 0xc(%ebp),%eax;
            //   add $0x35cc,%ecx; mov %edx,(%eax,%ecx,4)
            // ⇒ sig 的 uint 数组成员（下标含常量 0x35cc），P30 两步寻址。
            // [b28d-solo-DB_LoadEtc] P235：必须是 SIG_LOAD_ETC 的**完整类型成员**直访问
            //   （SigTypes_C.h +0xd730 匿名 union 视角 m_auraOptionWords，Main 代落）；
            //   跨类型 cast 视图恒产 base-first（探针 /tmp/dble_probe/p8.cpp k4），
            //   成员直访问逐条复现 ORIG 的 index→value→base（同探针 k1）。
            sig->m_auraOptionWords[optionType] = value;
        }
        ++i;
    }
    return true;
