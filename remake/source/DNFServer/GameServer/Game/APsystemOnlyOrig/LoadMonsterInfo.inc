// ORIG T 0x0844159a（LoadMonsterInfo.md 全文）：GetDBHandle(DB_HANDLE_2,0)；
// charac_kill_monster_info 按 charac_no 查 3 个 blob 列（boss/named/apc_boss）：
// select exec 失败/insert exec 失败/fetch 失败分别 cMyTrace 返 0；无行则
// inSert 新行返 1；各列 get_binary_length 判空，非空时 get_compressed_blob_data
// 解压至 sig 载荷 +0x000/0x1d4c/0x927c（0x1d4c/0x7530/0x1d4c），失败 trace 返 0。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0844159a
// [L4-r1] 6 处 cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 275→116（余 idx/col 槽序
//   + ok 的 P292 归一化，属 N18a/N82 族封顶）。
bool DB_LoadKillMonsterInfo::LoadMonsterInfo(SIG_LOAD_KILL_MONSTER_INFO* sig)
{
    static const int handleIdx = 2;
    // [SweepIntervalSet] ORIG 0x084415a4 `movb $0x0,-0x19(%ebp)`：ok 声明先于 db
    // 并初始化 false（原声明在 db 之后且未初始化 ⇒ 少一条 movb）。
    bool ok = false;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    // [SweepIntervalSet 实测回滚] ORIG 0x084415c8 `movl $0x5,-0x14(%ebp)` 疑为
    // dead 具名局部；补 `int unknown5=5;` 后 gap 7→6 但 ndiff 275→350 反劣
    // （4.4 槽序整体漂移）⇒ 该 5 非源级 dead 局部，回滚，残差归 N82/P210。
    db->set_query(
        "seLect boss_info, named_info, apc_boss_info from charac_kill_monster_info where charac_no=%u",
        sig->m_characNo);
    // [SweepIntervalSet] ORIG 0x084415a4 `movb $0x0,-0x19(%ebp)` = ok 在函数头
    // 初始化为 false（原声明未初始化）。
    ok = db->exec(true);
    if (!ok)
    {
        // [L4-r1] P227 全表达式临时链（ORIG 在 cMyTrace C1 前取 varargs 并跨 ctor
        // 保活 callee-saved；两语句形 4.4 只在 ctor 后求值 ⇒ 少 push）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa063, 5)(
            "DB_LoadKillMonsterInfo::LoadMonsterInfo select, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }

    if (db->get_n_rows() == 0)
    {
        db->set_query(
            "inSert into charac_kill_monster_info (charac_no) values(%u)",
            sig->m_characNo);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa070, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo insert, exec() ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
        return 1;
    }

    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa07a, 5)(
            "DB_LoadKillMonsterInfo::LoadMonsterInfo select, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }

    int idx = 0;
    int len;
    int col;
    // 列 0：boss_info → sig+0x000，容量 0x1d4c
    len = db->get_binary_length(idx);
    // [SweepIntervalSet] P222 极性翻转：ORIG 0x08441793 `je <++idx 尾块>`（len==0
    // 落 0x0844182a），4.4 对 `if(len==0){++idx;}else{…}` 恒产 `jne else` 反向布局。
    if (len != 0)
    {
        // [J4S4-⑩a 回滚 2026-09-11] 删 col 改 `idx++` 实测反劣
        // （ndiff 66→75、our 236→233、D −4→−7）⇒ 恢复具名 col。
        col = idx;
        ++idx;
        // [J4S4-⑩b P0] ORIG 0x084417c8 `mov %al,-0x19(%ebp)` + `movzbl -0x19` ⇒
        // 返回值经既有 bool ok 承接（我方原直判返回寄存器，少 2 条/处）。
        ok = get_compressed_blob_data(db, col, reinterpret_cast<char*>(sig->m_boss), 0x1d4c, 0);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa087, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo uncompress_zip(%u) ERROR charac_no=%u",
                idx, sig->m_characNo);
            return 0;
        }
    }
    else
    {
        ++idx;
    }
    // 列 1：named_info → sig+0x1d4c，容量 0x7530
    len = db->get_binary_length(idx);
    // [SweepIntervalSet] P222 极性翻转：ORIG 0x08441793 `je <++idx 尾块>`（len==0
    // 落 0x0844182a），4.4 对 `if(len==0){++idx;}else{…}` 恒产 `jne else` 反向布局。
    if (len != 0)
    {
        col = idx;
        ++idx;
        ok = get_compressed_blob_data(db, col, reinterpret_cast<char*>(sig->m_named), 0x7530, 0);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa09b, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo uncompress_zip(%u) ERROR charac_no=%u",
                idx, sig->m_characNo);
            return 0;
        }
    }
    else
    {
        ++idx;
    }
    // 列 2：apc_boss_info → sig+0x927c，容量 0x1d4c
    len = db->get_binary_length(idx);
    // [SweepIntervalSet] P222 极性翻转：ORIG 0x08441793 `je <++idx 尾块>`（len==0
    // 落 0x0844182a），4.4 对 `if(len==0){++idx;}else{…}` 恒产 `jne else` 反向布局。
    if (len != 0)
    {
        col = idx;
        ++idx;
        ok = get_compressed_blob_data(db, col, reinterpret_cast<char*>(sig->m_apc), 0x1d4c, 0);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa0af, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo uncompress_zip(%u) ERROR charac_no=%u",
                idx, sig->m_characNo);
            return 0;
        }
    }
    else
    {
        ++idx;
    }
    return 1;
}