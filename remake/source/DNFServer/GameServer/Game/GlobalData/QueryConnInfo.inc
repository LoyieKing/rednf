// ---- CDBConnector::QueryConnInfo ORIG @ 0x080eca6c（U3ShopDb 批次） ----
// 用主 DB 句柄(0,0) 查 db_connect 表，逐列填 STDBConnInfo 后放入 outMap[idx]。
// 列序：db_name(0x1f→m_db) / db_ip(0x11→m_host) / db_port(→m_port) /
//       db_userid(0x15→m_user) / db_passwd（先清零 60 字节，0x3c→+0x2d 起）。
// 密文经 CEnvironment::DecryptValue 解出（ORIG 解密结果未回写结构，仅校验）。
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080eca6c
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memset×1（N78 内联清零）；余 N82 槽位/STL 展开。
bool CDBConnector::QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                                 DBInfoMap& outMap)
{
    // [DataLane PoolE 负结论 2026-09-11] 指针局部→整型承载（`unsigned int dbAddr`）
    // 实测**反劣**：ndiff 20→24、件由 NEAR 落 DIFF（db 槽未归位且其它局部连锁漂移）
    // ⇒ 回滚为 `MySQL* db`；本件维持 N82/N126 槽图封顶。
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
    // [H7PartyGlobal 2026-09-11] P331 声明序旋钮负结论：把「写后未读」的 group 副本
    // （ORIG 0x080ecc9b 落最深槽 -0x64）前移到 i 之前 ⇒ 4.4 反把本件由 NEAR 打成 DIFF
    // （TU：near 11→10、diff 37→38），已回滚。⇒ 槽向旋钮在此无效，维持 N82/N126 封顶。
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
    // [G7-sub] 结论（N82/N126 封顶）：ORIG 169 / 我方 169；p167 仅 STDBConnInfo
    //   C1↔C2 别名。ORIG 帧 0x1cc / 我方 0x1dc(+0x10)，标量槽逐项同构但位置差：
    //   ORIG i(-0x1c)/db(-0x20)/键副本(-0x24)/passCipher(-0x60)/group 副本(-0x64)
    //   vs 我方 i(-0x20)/db(-0x24)/键(-0x28)/passCipher(-0x64)/group 副本(-0x1c)。
    //   ORIG 把「写后未读」的 group 副本放紧邻 passCipher 的深处
    //   （0x080ecc9b `mov %eax,-0x64(%ebp)`），4.4 恒放最浅槽；键副本取址在
    //   0x080ecca4 `lea -0x24(%ebp),%eax`。块级标量槽向跨版本不可控（N93），
    //   且该未读副本属 P171 必须照抄 ⇒ 封顶（不删具名、不试声明序）。
    outMap[idxCopy] = info;
    return true;
}