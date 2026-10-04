// ORIG 0x0818c0e0：member_mousepass（GoblinPad）加载。
int LOGIN_QUERY::arad_sp_convert_pass_pad_info(SIG_LOGIN_DATA* data)
{
    // b28d-50 AE：ORIG 0x0818c0e0。槽位：ORIG db=-0xc、cMyTrace=-0x1c..-0xd、
    // enableFlag=-0x1d（4.1 按首次使用序分槽）；4.4 先给函数作用域标量分槽
    // （db -0xc、enableFlag -0xd），再给块内聚合 cMyTrace 分槽 ⇒ 对象被挤到
    // -0x20。修法：enableFlag 改为**后置块内局部**（声明在 cMyTrace 块之后的
    // 复合块内），使其分槽晚于该块展开 ⇒ cMyTrace 落 -0x1c、enableFlag 落
    // -0x1d，与 ORIG 逐条全等（探针 /tmp/arad_probe/s5.cpp f7 实证）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2b0, "%s", "Database null!!");
        return -1;
    }

    if (!db->set_query(
            "seLect mousepass, fail_cnt, cancel_cnt, version_info, validity_time, "
            "enable_flag from member_mousepass where m_id=%d",
            data->m_accId))
    {
        return -1;
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            700, "%s", "arad_sp_convert_pass_pad_info fail");
        return -1;
    }

    if (db->get_n_rows() == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x2c2, 0);
        trace("Goblin Pad Exist");
        data->m_passPadVersion = 0;      // version_info 置无
        return 0;
    }

    if (!db->fetch())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2c9, "%s", "arad_sp_convert_pass_pad_info fetch fail");
        return -1;
    }

    if (!db->get_str(0, data->m_passPadPassword, 0x24))   // +0x38e1
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2cf, "%s", "Mouse Pass Encrypted Parsing error");
        return -1;
    }
    // [b21 N·P198-②/P197-④] 以下 get_str/get_int/get_byte/get_uint 五连判定——
    // ORIG(4.1) 对 call 结果 `xor $0x1,%eax; test; je` 寄存器直测（无槽）;4.4-O0 对
    // 该类 call 结果 bool 判定恒 materialize 到栈槽 `mov %al,-off; movzbl; xor; test`
    // （P198-②/P197-④：源形不可复现，内联也出同样栈槽，禁改比较=双调用真 bug）。
    // 逐 getter 列/目标偏移/错误日志行号已与 ORIG 逐一核对，语义闭合，封 N。
    if (!db->get_int(1, LOGIN_FIELD(int, data, 0x3914)))     // fail_cnt
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2d5, "%s", "Mouse PassPad Fail Count Parsing error");
        return -1;
    }
    if (!db->get_int(2, LOGIN_FIELD(int, data, 0x3918)))     // cancel_cnt
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2db, "%s", "PassPad Cancel Count Parsing error");
        return -1;
    }
    if (!db->get_byte(3, data->m_passPadVersion))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2e0, "%s", "VersionInfo Parsing error");
        return -1;
    }
    if (!db->get_uint(4, LOGIN_FIELD(unsigned int, data, 0x3910))) // validity_time
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2e6, "%s", "Goblin Validity Time Parsing error");
        return -1;
    }
    {
        char enableFlag;  // b28d-50：后置块内局部，见函数头判词
        enableFlag = 0;   // ORIG movb $0x0,-0x1d 预清（P198-①两句式）
        if (!db->get_byte(5, enableFlag))                   // enable_flag
        {
            LogManager::logFormat(
                1, "localjapan/Arad_DB_Query.cpp",
                __PRETTY_FUNCTION__,
                0x2ed, "%s", "Goblin enable flage Parsing error");
            return -1;
        }
        if (enableFlag == 1)
        {
            // ORIG：movb 直写 0x390e——即 SIG_LOGIN_DATA::m_passPadCertified
            // （bool，具名化替代 login_field 外联 call）。
            data->m_passPadCertified = true;
        }
    }
    return 0;
}