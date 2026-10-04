// ORIG 0x0818ca69：member_broadcast 单行广播信息；到期未过→-1；
// channelType==7 时附 addUserDBInfo。
int LOGIN_QUERY::arad_sp_select_member_brodcast(
    SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry, int userIdx)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x3d6, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect m_id,charac_no,server_id,unix_timestamp(start_time),charac_name "
            "from member_broadcast where event_id=%d and m_id=%d and end_time>now() limit 1",
            data->m_accId,
            entry.m_eventId) != 0)
    {
        return -1;
    }

    // b28d-solo-ARAD_Database：ORIG 0x818cb10 起逐条独立守卫（每条
    //   `call; xor $1,%eax; test %al,%al; je <成功继续>`，P168a bool callee 裸形）；
    //   仅 startTime 有零初始化槽（-0x1c）。ORIG 把 col0/1/2 直接写 userInfo 三个
    //   连续成员（-0x18/-0x14/-0x10），无独立 characNo/serverId 槽。
    // verdict=NEAR 132/132（gap 2→0）：P167 12/12 CLEAR。已修两处：①DBMgr.h
    //   stUserDBInfo_t 三成员 int→unsigned int（同宽/偏移/mangled 不变；ORIG
    //   0x0818cc51/0x0818cc5d 两处 store 无 eax→edx 中转 ⇒ P211 档③同型直存；
    //   探针实证 unsigned→unsigned 单段）消 2 条中转；②比较改
    //   `(int)startTime > getCurSec()`（P243/P23 书写序=求值序）⇒ 复现 ORIG
    //   `mov -N,%eax; mov %eax,%ebx; …call…; cmp %eax,%ebx; setg` 的 %ebx 保活。
    //   残余仅槽号（ORIG startTime -0x1c/userInfo -0x18 vs 4.4 -0x10/-0x1c，
    //   4.4 按首次使用序，声明序调整无效）与数据地址。语义闭合。
    // 判据⑩（b28d 槽序律）：4.4 恒「后声明者占深槽」；ORIG 布局 startTime@-0x1c（深）/
    //   userInfo@-0x18..-0x10（浅）⇒ 源码须 userInfo 先声明、startTime 后声明
    //   （create_equip 同法已 AE）。
    DBMgr::stUserDBInfo_t userInfo;             // ORIG -0x18：col0/1/2 复用 +0/+4/+8
    unsigned int startTime = 0;                 // ORIG -0x1c
    if (!db->get_uint(0, reinterpret_cast<unsigned int&>(userInfo.m_userDBIdx)))
    {
        return -1;
    }
    if (!db->get_uint(1, reinterpret_cast<unsigned int&>(userInfo.m_userDBType)))
    {
        return -1;
    }
    if (!db->get_uint(2, reinterpret_cast<unsigned int&>(userInfo.m_serverGroup)))
    {
        return -1;
    }
    if (!db->get_uint(3, startTime))
    {
        return -1;
    }
    if (!db->get_str(4, data->m_broadcastCharacName, 0x1e))
    {
        return -1;
    }

    // P243/P23：ORIG 0x0818cbfe 先把 startTime 载入 %ebx 跨 getCurSec call 保活
    //   （`mov -0x1c(%ebp),%eax; mov %eax,%ebx; …call…; cmp %eax,%ebx; setg`）
    //   ⇒ 左操作数写 startTime、右操作数写 call 表达式（求值序=书写序）。
    if (static_cast<int>(startTime) > GlobalData::s_systemTime_.getCurSec())
    {
        return -1;                                           // 广播尚未生效
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        GlobalData::s_db_mgr->addUserDBInfo(userIdx, userInfo);
    }
    // P0：Ghidra C 直接偏移写 —— 0x39ac/0x39a8 对应具名字段（login_field 外联 call 需免）
    // P211 档③：ORIG 两处 store 无 eax→edx 中转 ⇒ 两侧同型；stUserDBInfo_t 成员是 int，
    //   显式 static_cast<unsigned int> 使 RHS 与字段同型（同宽零指令），消 2 条中转。
    data->m_tournamentAccountFlag =
        static_cast<unsigned int>(userInfo.m_userDBType);
    data->m_tournamentServerGroup =
        static_cast<unsigned int>(userInfo.m_serverGroup);
    return 0;
}