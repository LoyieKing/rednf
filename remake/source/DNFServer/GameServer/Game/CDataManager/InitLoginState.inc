// ---------------------------------------------------------------------------
// CDataManager::InitLoginState —— ORIG 0x08360306（0xf9），报告
// docs/class_func_reports/CDataManager/InitLoginState.md 逐条转写：
// 取 DB_HANDLE_6 句柄；operation_type 0/2 → 按 channel 清 login_status；
// operation_type==1 且 env+0x1b0==1 → 不带 channel 清；否则直接成功。
// env+0x1b0 字段未建模，保留裸偏移（推断）。
// ---------------------------------------------------------------------------
// 残余 DIFF 归因（o=67 n=67，指令序列已同构）：hdb(-0xc)/result(-0xd) 与
// 4.4 (hdb -0x10/result -0x9) 的 1B/4B 槽排布方向差（P112：4.1 后声明更深、
// 4.4 后声明更浅），及 DBMgr 地址/字符串 reloc 显示差。
bool CDataManager::InitLoginState(ENUM_SERVER_GROUP serverGroup, int channelNo)
{
    // ORIG 0x08360306：get_operation_type 无缓存、多次重调（P48 双臂双调 =
    // 源码多处独立调用）；op!=1 时 result 无赋值（ORIG +0xa7 je 直落 +0xf4
    // 返未初始化槽，忠实保留 UB 形）；内层 channelNo!=1 → result=true。
    bool result;
    // result 声明前置（4.4 槽序随声明序）：hdb 落 -0xc、result 落 -0xd，
    // 与 ORIG 槽布局一致（原序 hdb 落 -0x10/result -0x9，N82 槽平移）。
    MySQL* hdb = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                   (ENUM_SERVER_GROUP)0);
    if (G_CEnvironment()->get_operation_type() == 0 ||
        G_CEnvironment()->get_operation_type() == 2)
    {
        hdb->set_query(
            "upDate login_account_%d set login_status=0 where m_channel_no=%d",
            serverGroup, channelNo);
        result = hdb->exec(true);
    }
    else if (G_CEnvironment()->get_operation_type() == 1)
    {
        if (G_CEnvironment()->m_serverEnvir.m_channelNo == 1)
        {
            hdb->set_query("upDate login_account_%d set login_status=0",
                           serverGroup);
            result = hdb->exec(true);
        }
        else
        {
            result = true;
        }
    }
    return result;
}