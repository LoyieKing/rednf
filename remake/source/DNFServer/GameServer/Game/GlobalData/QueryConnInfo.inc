bool CDBConnector::QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                                 DBInfoMap& outMap)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0,
                                                  (ENUM_SERVER_GROUP)0);
    if (!db->set_query("seLect db_name,db_ip,db_port,db_userid,db_passwd from "
                       "db_connect where db_server_group = %d and db_type = %d",
                       group, idx))
        return false;
    // ORIG 0x080ecb9d/0x080ecbd1：两条件各自 `xor $1; test; je <继续>; mov $0;
    // jmp <end>` 独立早退（P133 独立守卫，禁合并短路 `||`）。
    if (!db->exec(true))
        return false;
    if (!db->fetch())
        return false;
    if (db->get_n_rows() != 1)
        return false;

    // [DualMisc] ORIG 0x080eca6c：列索引是递增变量（`mov -0x1c,%eax; addl $1,-0x1c`
    // ×5，i 声明在 info 之前），非字面常量 ⇒ 改 i++。
    int i = 0;
    STDBConnInfo info;
    if (!db->get_str(i++, info.m_db, 0x1f))
        return false;
    if (!db->get_str(i++, info.m_host, 0x11))
        return false;
    if (!db->get_int(i++, info.m_port))
        return false;
    if (!db->get_str(i++, info.m_user, 0x15))
        return false;
    // ORIG 0x080ecc23..0x080ecc34：此处对 passCipher 做常量内联 rep stos
    // （0xf dword = 0x3c 字节）⇒ 源形是**在此处**声明 `char passCipher[0x3c] = {0};`
    // （P214③/P328⑨）；旧写 `memset(info.m_pass,0,60)` 既错目标（ORIG 清的是
    // 密文缓冲）又退化成 call（N78）。
    char passCipher[0x3c] = {0};
    if (!db->get_str(i++, passCipher, 0x3c))
        return false;
    // ORIG 0x080ecc6e..0x080ecc85：第二实参 = `lea -0x1c8(%ebp); add $0x2d`
    // = &info.m_pass（解密结果**回写结构**），非独立栈缓冲（旧写 passPlain[0x15]
    // 是实参来源错 + 多一个 0x15 数组致槽漂）。
    if (!G_CEnvironment()->DecryptValue(passCipher, info.m_pass))
        return false;
    // ORIG 0x80eccb8：rep movsd 0x5a dword（0x168 字节）拷入 outMap[idx]。
    // ORIG 0x080ecc98..0x080ecca4：键取形参值的具名副本（-0x24）后再取址，
    // 另有 group 的副本（-0x64，写后未读 ⇒ P171 照抄）。
    ENUM_SERVER_GROUP groupCopy = group;
    ENUM_DB_HANDLE_IDX idxCopy = idx;
    outMap[idxCopy] = info;
    return true;
