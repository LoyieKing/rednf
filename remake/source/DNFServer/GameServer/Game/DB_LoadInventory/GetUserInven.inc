// ============================================================================
// DB_LoadInventory::GetUserInven @0x0840b0d8
// 列序 money(0)/coin(1)/inventory(2)/equipslot(3)/pay_coin(4)/event_coin(5)/
// creature(6)/creature_flag(7)/inventory_capacity(8)。三 blob 分别解压到
// +0x5949 / +0xa603 / +0x1102d，容量 0x473e/0x37a/0x1d36；尺寸校验
// equip 0x53e / inven 0x4a58 / creature 0x4a58 对 0x3d 对齐。随后
// member_avatar_coin 聚合查询入 +0x14780；tag charac（+0x8）非零走
// _GetTagCharacInven。blob 溢出/uncompress 失败 throw unsigned int
// （0x255a/0x255c/0x2563/0x2565/0x2567/0x256e），catch 后 logFormat
// 0x259c 并返 0。错误行号 0x24cb..0x25b2。
// b28d-solo-DB_LoadInventory：ok 变量（14 处共用槽）+ P227 临时链 +
// len/binLen 副本（P171，声明序对齐 ORIG 槽）已归位；残差 = 4.4 标量分区
// （ok -0x25 vs -0x31、binLen 副本整体深 4）⇒ N82 封顶；p167 MISSING=0。
// 本轮复核修复（真语义 bug）：creature blob 解压目标。ORIG 0840b8ef
// `add $0x1102d,%eax`（= 0xeed1+0x8c×0x3d = m_invenType3[0x8c] 首槽，
// 0x8c 正是 dispatch reset 循环 0..0x8b 之后第一个 creature 槽）；旧译
// `&m_invenType3[72].m_reserved` 编译成 +0x1002d，与 ORIG 差 0x1000 字节
// ⇒ blob 落到错误槽区。已改为 `&m_invenType3[0x8c]`，.o 立即数 0x1102d
// 与 ORIG 逐字节一致（四数不变、p167 81/81 CLEAR）。
// [SweepDBLoadEtcMain 本波] **gap 4→0（725/725）、p167 CLEAR、jcc 49/49 逐位核过**，
// 修 4 处真 bug + 2 处源形：
//   ① 三处「incorrect inventory size」日志第 3 参误用 len（解压后容量）——ORIG
//      0840b9a1/0840ba2b/0840baba 读的是 `-0x2c/-0x30/-0x28`（binLen，DB 压缩尺寸）
//      ⇒ 改 equip/inven/creatureBinLen（日志契约 original_size,db_size 才自洽）；
//   ② `char* m = NumberToString(...)` 具名局部：ORIG 0840bb7a `mov %eax,0x8(%esp)`
//      直压实参、无 m 槽（P0 槽-局部对应律）⇒ 内联进 set_query 实参位；
//   ③ col 具名局部（ORIG -0x20，`movl $0x6` + 后三调用 `col++`）：补后
//      get_int/get_uint 取址回到 ORIG 的 `lea 0x1287c(%eax),%edx` 形（旧字面量形
//      发 add 且少 col 的 8 条装载/自增）；
//   ④ 三个 binLen 与 len 同型化 unsigned（P211 档③/P209）：int 型时 4.4 走
//      `mov binLen,%edx; mov L,%eax; cmp %eax,%edx`（+1 条×3），unsigned 后复现
//      ORIG `mov L,%eax; cmp %eax,binLen; jbe` 两段 mem-dst 形
//      （探针 /tmp/p209p/q.cpp p7↔p9）。
// 残余 = N104（catch/unwind 块 29 条内容逐条全等、仅相对 % 检查块位置互换）+
// N82（ok -0x25 vs -0x31、binLen 组整体深 4）+ 3 处 `jbe↔jae`（cmp 操作数方向
// fold：ORIG `cmp %eax,B`=B−L 配 jbe，我方 `cmp B,%eax`=L−B 配 jae，落点同体）。
// [L3CUserPremDBInven 本波复核（ORIG nm 0x0840b0d8）] 复算 725/725、diff 段仅剩
//   上述 3 处 jbe↔jae + N104 的 29 条 landing-pad 位移（ORIG 0840ba4x 处 catch
//   体 `cmp $0x1,%edx; je; call _Unwind_Resume; call __cxa_begin_catch` 在我方
//   提前 29 条出现）⇒ 无新旋钮，维持 N104/N82 封顶。
// ============================================================================
bool DB_LoadInventory::GetUserInven(SIG_LOAD_INVENTORY* sig)
{
    static const int handleIdx = 3;
    // b28d-214③/N78：ORIG 对三缓冲区发 rep stos 内联清零（无 memset call）
    // ⇒ 用数组零初始化器复现。
    char equipBuf[0x37a] = {0};
    char invenBuf[0x473e] = {0};
    char creatureBuf[0x1d36] = {0};
    // ORIG 标量区（4.4 先声明深，故 ok 最先声明占最深 -0x31；4.1 的 len 组
    // creatureLen -0x148 / equipLen -0x14c /
    // invenLen -0x150；binLen 副本 -0x30/-0x2c/-0x28 成对初始化（P171）。
    bool ok;                            // ORIG -0x31（所有 db 检查共用槽）
    unsigned int creatureLen = 0x1d36;  // ORIG -0x148
    unsigned int equipLen = 0x37a;      // ORIG -0x14c（解压 in/out 容量）
    unsigned int invenLen = 0x473e;     // ORIG -0x150
    // 三个 binLen 必须与 len 同型（unsigned）：ORIG 比较形
    // `mov L,%eax; cmp %eax,binLen; jbe`（binLen 为内存 dst，0840b791/0840b7c8/
    // 0840b7ff）；int 型时 4.4 走 `mov binLen,%edx; mov L,%eax; cmp %eax,%edx`
    // 多一条中转（P209 同族，探针 /tmp/p209p/q.cpp p7↔p9 实证）。
    unsigned int invenBinLen = invenLen;      // ORIG -0x30
    unsigned int equipBinLen = equipLen;      // ORIG -0x2c
    unsigned int creatureBinLen = creatureLen;// ORIG -0x28

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect money,coin,inventory,equipslot,pay_coin,event_coin,"
                  "creature,creature_flag,inventory_capacity from inventory"
                  " where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24cb, 5)
            ("DB_LoadInventory::GetUserInven, exec ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24d2, 5)
            ("DB_LoadInventory::GetUserInven, fetch ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(0, *(unsigned int*)&sig->m_money);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24d9, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(0, pContext->money)"
             " ERROR charac_no=%u", sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(1, sig->m_coin);             // +0x4038（unsigned int 直引）
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24e0, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(1, pContext->coin)"
             " ERROR charac_no=%u", sig->m_characNo);
        return 0;
    }
    invenBinLen = db->get_binary_length(2);
    ok = db->get_binary(2, invenBuf, invenLen);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24e8, 5)
            ("DB_LoadInventory::GetUserInven, get_binary(2) ERROR"
             " charac_no=%u", sig->m_characNo);
        return 0;
    }
    equipBinLen = db->get_binary_length(3);
    ok = db->get_binary(3, equipBuf, equipLen);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24f0, 5)
            ("DB_LoadInventory::GetUserInven, get_binary(3) ERROR"
             " charac_no=%u", sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(4, sig->m_payCoin);          // +0x403c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24f7, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(4) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(5, sig->m_eventCoin);        // +0x4040
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24ff, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(5) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    // ORIG 具名 col 槽（-0x20）：`movl $0x6,-0x20` + 后三调用 col++ 形
    // （0840b5e3/0840b5ea/0840b607/0840b692/0840b713，P0 槽-局部对应律）。
    int col = 6;
    creatureBinLen = db->get_binary_length(col);
    ok = db->get_binary(col++, creatureBuf, creatureLen);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x251b, 5)
            ("DB_LoadCreature, get_binary(8) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_int(col++, sig->m_creatureCount);  // +0x1287c（int 直引）
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2522, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(9) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(col++, *(unsigned int*)&sig->m_inventoryCapacity);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x253a, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(13) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }

    try
    {
        // [D04] ORIG 三处 `cmp %eax,B; jbe`（B=binLen, L=len；B−L 折叠方向）——
        // 写 `B > L` 正向式复现 ORIG mem-dst cmp（`L < B` 反向式产 `cmp B,%eax; jae`）。
        if (equipBinLen > equipLen)
        {
            throw static_cast<unsigned int>(0x255a);
        }
        if (invenBinLen > invenLen)
        {
            throw static_cast<unsigned int>(0x255c);
        }
        if (creatureBinLen > creatureLen)
        {
            throw static_cast<unsigned int>(0x2563);
        }
        // equipslot 解压目标 = equip 镜像区第 10 槽起（GetUserInven.md ORIG
        // `add $0xa603` = 0xa3a1+10×0x3d）；inventory blob → m_invenType1 首
        // （ORIG +0x5949）；creature blob → +0x1102d = 0xeed1+0x8c×0x3d =
        // m_invenType3[0x8c] 首槽（ORIG 0840b8ef `add $0x1102d,%eax` 实证；
        // 0x8c 正是 dispatch reset 循环 0..0x8b 之后的第一个 creature 槽）。
        // 旧译写作 `m_invenType3[72].m_reserved`（= +0x1002d），与 ORIG 差
        // 0x1000 字节 ⇒ 真语义 bug（creature blob 落到错误槽区）。
        if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_equipMirror[10]),
                            equipLen, equipBuf,
                            equipBinLen))
        {
            throw static_cast<unsigned int>(0x2565);
        }
        if (!uncompress_zip(reinterpret_cast<char*>(sig->m_invenType1), invenLen,
                            invenBuf,
                            invenBinLen))
        {
            throw static_cast<unsigned int>(0x2567);
        }
        if (!uncompress_zip(
                reinterpret_cast<char*>(&sig->m_invenType3[0x8c]),
                creatureLen, creatureBuf,
                            creatureBinLen))
        {
            throw static_cast<unsigned int>(0x256e);
        }
    }
    catch (unsigned int err)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x259c, "Uncompress failed. charac_no(%d)(%d)",
                              sig->m_characNo, err);
        return 0;
    }

    // ORIG 0840b5xx/b6xx/b7xx 三处均为「`test; je <主流程>` + trace 块直落」
    // ⇒ 原码是三个独立早返守卫（非嵌套 if）。嵌套 if 形 4.4 把三块 trace 集中
    // 到函数尾并产 je/jne 反向（ndiff 362）；改早返后 ndiff 210、je/jne 消失
    // （残差 = col 变量 N + N107 lea/add，见下）。
    if ((equipLen % 0x3d) != 0 && equipLen != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2573, 5)
            ("incorrect inventory size charac_no(%d), equip,"
             " original_size(%d), db_size(%d)\n", sig->m_characNo, 0x53e,
             equipBinLen);
        return 0;
    }
    if ((invenLen % 0x3d) != 0 && invenLen != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2579, 5)
            ("incorrect inventory size charac_no(%d), inven,"
             " original_size(%d), db_size(%d)\n", sig->m_characNo, 0x4a58,
             invenBinLen);
        return 0;
    }
    if ((creatureLen % 0x3d) != 0 && creatureLen != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2588, 5)
            ("incorrect inventory size charac_no(%d), creature,"
             " original_size(%d), db_size(%d)\n", sig->m_characNo, 0x4a58,
             creatureBinLen);
        return 0;
    }
    db->set_query("seLect ifnull(min(avatar_coin),0) as avatar_coin"
                  " from member_avatar_coin where m_id = %s",
                  NumberToString(sig->m_accId, 0));
    // ORIG 尾部为 guard 形（fail 块直落、je 跳主流程）⇒ 逐个早返。
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x25a6, 5)
            ("DB_LoadInventory::GetUserInven, avatar_coin exec"
             " ERROR m_id=%s", NumberToString(sig->m_accId, 0));
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x25ac, 5)
            ("DB_LoadInventory::GetUserInven, avatar_coin fetch"
             " ERROR m_id=%s", NumberToString(sig->m_accId, 0));
        return 0;
    }
    ok = db->get_uint(0, sig->m_avatarCoin);   // +0x14780
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x25b2, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(%u,"
             " pContext->avatar_coin) ERROR m_id=%s",
             NumberToString(sig->m_accId, 0));
        return 0;
    }
    if (sig->m_field8 != 0)
    {
        _GetTagCharacInven(sig->m_field8, &sig->m_tagInven);
    }
    return 1;
    return 1;
}