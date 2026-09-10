bool DB_MailBox_Req_List::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MailBoxListReqView* req = (MailBoxListReqView*)stream->GetOutBuffer<SIG_MAILBOX_LIST>();
    SIG_MAILBOX_LIST localSig;
    memset(&localSig, 0, sizeof(localSig));
    SIG_MAILBOX_LIST* sig = &localSig;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    // 首段：无增量游标时清理过期未限时 postal
    if (req->m_maxPostalId == 0)
    {
        db->set_query(
            "upDate postal set delete_flag=%d where receive_charac_no=%d and unlimit_flag=0 and occ_time<subdate(now(),interval %d day) and delete_flag=0",
            2, req->m_characNo, 0xf);
        if (!db->exec(true))
            return 0;
    }

    // 增量游标后缀（char sql[0x80] rep stos 0x20 dword 实证）
    char sql[0x80] = { 0 };
    if (req->m_maxPostalId != 0)
        sprintf(sql, " and postal_id > %d", req->m_maxPostalId);

    db->set_query(
        "seLect postal_id,unix_timestamp(occ_time),send_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,unlimit_flag,avata_flag,creature_flag,letter_id,extend_info,item_guid,amplify_option,amplify_value,random_option,seperate_upgrade,type from postal where receive_charac_no=%d and delete_flag=0 %s and (unlimit_flag=1 or occ_time>subdate(now(),interval 15 day)) order by postal_id limit %d",
        req->m_characNo, sql, 0x14);
    if (!db->exec(true))
        return 0;

    // 请求附加 postal_id 列表 → map（make_pair<unsigned int&, int> 实证）
    std::map<unsigned int, int> extraMap;
    // ORIG 0x0841e061 `cmpl $0x13,-0x34; setbe` ⇒ 无符号尾测（setle 是 signed）
    unsigned int mi;
    for (mi = 0; mi < 0x14; ++mi)
    {
        if (req->m_postalExtra[mi] != 0)
        {
            extraMap.insert(std::make_pair((unsigned int&)req->m_postalExtra[mi],
                                           (int)req->m_postalExtra[mi]));
        }
    }

    SIG_I32(sig, SIG_OFF_LETTER_USED) = (int)req->m_maxLetter;   // +0x3326
    SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID) = 0;                     // +0x79c
    int nRows = db->get_n_rows();
    SIG_I32(sig, SIG_OFF_POSTAL_COUNT) = std::min((int)req->m_maxLetter, nRows);
    SIG_I32(sig, SIG_OFF_REMAIN_COUNT) = 0;
    if ((int)req->m_maxLetter <= SIG_I32(sig, SIG_OFF_POSTAL_COUNT))
    {
        SIG_I32(sig, SIG_OFF_REMAIN_COUNT) =
            db->get_n_rows() - SIG_I32(sig, SIG_OFF_POSTAL_COUNT);
    }

    // avata / creature ui_id 收集表（首元素为条数；rep stos 0x15 dword 实证）
    unsigned int avataList[0x15] = { 0 };
    unsigned int creatureList[0x15] = { 0 };

    int rowIdx = 0;    // +0x794 最终值
    int loopIdx = 0;
    // ORIG 0x0841e13d 入口直跳条件、0x0841e8e3 循环尾 `addl $1,-0x28` 后再测
    // （for 形；while + 尾部 ++loopIdx 无法承载 continue 的落点）——失败列走
    // 0x0841e8a0..0x0841e8e2 的 nop 岛跳到 ++loopIdx ⇒ **continue**，非 return 0。
    for (; loopIdx < SIG_I32(sig, SIG_OFF_POSTAL_COUNT) && loopIdx < 0x14; ++loopIdx)
    {
        if (!db->fetch())
            return 0;

        // ORIG 0x0841e15e 循环体开头只有 `movl $0,-0xf0`/`movl $0,-0xf4`
        // 两条（b16/s17），p0..s12 与 name9 均**无**初始化（各列由 get_*
        // 成功路径写入，失败路径 continue 不读）⇒ 原码是裸声明（P171 照抄）。
        int p0, p1, p2;
        short s3;
        unsigned long u4;
        int i5;
        short s6;
        char b7;
        int i8;
        char name9[0x1d];
        short s10, s11, s12;
        // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841e8d7 `lea -0x106(%ebp),%eax;
        // call RandomOption::reset` ⇒ 原码是 RandomOption 局部 + reset()（不是
        // memset 等价物）；p167 MISSING `_ZN12RandomOption5resetEv` 由此归零。
        MailExtSource extOpt;
        extOpt.m_ro.reset();

        // ORIG 各列取数失败跳到循环尾（0x0841e8a0..0x0841e8e2 共 23 岛）⇒ continue
        if (!db->get_int(0, p0)) continue;

        if (!db->get_int(1, p1)) continue;
        if (!db->get_int(2, p2)) continue;
        if (!db->get_short(3, s3)) continue;
        if (!db->get_ulong(4, u4)) continue;
        if (!db->get_int(5, i5)) continue;
        if (!db->get_short(6, s6)) continue;
        if (!db->get_byte(7, b7)) continue;
        if (!db->get_int(8, i8)) continue;
        if (!db->get_str(9, name9, 0x1d)) continue;
        if (!db->get_short(10, s10)) continue;
        if (!db->get_short(11, s11)) continue;
        if (!db->get_short(12, s12)) continue;

        // avata==1 && creature==1 的行跳过（ORIG 跳过条件）
        if (s11 != 1 || s12 != 1)
        {
            // ORIG 0x0841e3ae `movl $0xd,-0x24` 后每处 `mov -0x24,%eax;
            // addl $1,-0x24; ...call get_*(eax,..)` ⇒ 原码是 idx 初值 0xd 的
            // **后置自增实参**（我方旧形用常量 13 + 前置 ++idx，每处少 2 条）。
            int idx = 0xd;
            unsigned int u13 = 0;
            unsigned long u14 = 0;
            int b16 = 0;
            int s17 = 0;
            SIG_MAILBOX_POSTAL_AMP ampBuf;
            char b19 = 0, b20 = 0;

            // [b28d] ORIG 0x0841e3ae..0x0841e8e2 有 23 个「失败 → 循环尾」跳转岛
            // （每处 `test;jne <岛>` + 岛 `jmp +0xa99; nop`；O[340]/O[343]/O[346]
            // 三段独立判断实证）⇒ 原码是**独立早退**（嵌套 if 形 4.4 只产 13 岛 ⇒
            // 少 10 条 jmp+10 条 nop）。故展平为 continue 链（落点=循环尾，同 ORIG）。
            if (!db->get_uint(idx++, u13))
                continue;
            if (u4 == 0 && i8 == 0)
                continue;
            if (!db->get_ulong(idx++, u14))
                continue;
            if (!db->get_binary(idx++, &ampBuf, 10))           // amplify 区（-0x116）
                continue;
            if (!db->get_byte(idx++, b16))
                continue;
            if (!db->get_short(idx++, s17))
                continue;
            if (!db->get_binary(idx++, &extOpt.m_ro, 0xe)) // extend_info 区（-0x102）
                continue;
            if (!db->get_byte(idx++, b19))
                continue;
            if (!db->get_byte(idx++, b20))
                continue;
            {
                                            if (u13 != 0)
                                            {
                                                extraMap.insert(
                                                    std::make_pair((unsigned int&)u13,
                                                                   (int)u13));
                                            }

                                            // P183：ORIG 每字段重发完整 imul/lea/add 链
                                            // （0x0841e5ae..0x0841e640），禁缓存 item 指针。
                                            // [b28d] 视图 cast → 权威头 SIG_MAILBOX_POSTAL 具名成员
                                            // 直访问（SigTypes_D.h 字段化），各字段附 ORIG 地址链。
                                            sig->m_box.m_postals[rowIdx].m_postalId = p0;
                                            // ORIG 0x0841e5c2 `add $0x50,%eax; mov %edx,0x5(%eax)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_field55 = p1 + 0x13c680;
                                            // ORIG 0x0841e5f5 `mov %ecx,0x4(%edx,%eax,1)`
                                            sig->m_box.m_postals[rowIdx].m_sendCharacNo = p2;
                                            // ORIG 0x0841e5f8 `cmp $0x1,%dx; sete %dl; add $0x29,%eax`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_sealFlag = (char)(s3 == 1);
                                            // ORIG 0x0841e614 `add $0x20,%edx; mov %eax,0xa(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occTime = (unsigned int)u4;
                                            sig->m_box.m_postals[rowIdx].m_tail.m_addInfo = (unsigned int)i5;
                                            // ORIG 0x0841e644 `add $0x20,%edx; mov %ax,0x13(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_upgrade = s6;
                                            // ORIG 0x0841e65e `add $0x32,%edx; mov %al,(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_endurance = b7;
                                            // ORIG 0x0841e676 `add $0x20,%edx; mov %eax,0x15(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_itemGuid = (unsigned int)u14;
                                            // ORIG 0x0841e691..0x0841e6b6：一次 `add $0x30` 基址 +
                                            // 0x9/0xd/0x11 连续 store = amp 组**整体赋值**形。
                                            sig->m_box.m_postals[rowIdx].m_tail.m_amp = ampBuf;
                                            // ORIG 0x0841e6ba `add $0x20,%edx; mov %eax,0x5(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_itemId = i8;
                                            // ORIG 0x0841e6d2 `add $0x5b,%eax; mov %dl,(%eax)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_unlimit = (s10 == 1);
                                            if (s11 == 1)
                                            {
                                                // ORIG 0x0841e6fe `add $0x59,%eax; movb $0x1,(%eax)`
                                                sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_avataFlag = 1;
                                                avataList[0] = avataList[0] + 1;
                                                avataList[avataList[0]] = (unsigned int)i5;
                                            }
                                            else
                                            {
                                                // ORIG 0x0841e734 `add $0x59,%eax; movb $0x0,(%eax)`
                                                sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_avataFlag = 0;
                                            }
                                            if (s12 == 1)
                                            {
                                                // ORIG 0x0841e753 `add $0x5a,%eax; movb $0x1,(%eax)`
                                                sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_creatureFlag = 1;
                                                creatureList[0] = creatureList[0] + 1;
                                                creatureList[creatureList[0]] = (unsigned int)i5;
                                            }
                                            else
                                            {
                                                // ORIG 0x0841e789 `add $0x5a,%eax; movb $0x0,(%eax)`
                                                sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_creatureFlag = 0;
                                            }
                                            // ORIG 0x0841e79b `add $0x50,%edx; mov %eax,0xc(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_letterId = u13;
                                            // ORIG 0x0841e7b3 `add $0x43,%edx; mov %al,(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_field43 = (char)b16;
                                            // ORIG 0x0841e7ca `add $0x30,%edx; mov %ax,0x14(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_field44 = (short)s17;
                                            // ORIG 0x0841e7e3 `add $0x60,%edx; mov %al,(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_field60 = b20;
                                            // ORIG 0x0841e7fb..0x0841e82c：一次 `add $0x30` 基址 +
                                            // 0x16/0x1a/0x1e/0x22 连续 store（extend_info 14 字节）。
                                            // ext2（abs 0x4e..0x51）跨 m_occ 边界（abs 0x50）⇒ 按
                                            // 2+2 字节具名写（权威头 SIG_MAILBOX_POSTAL_OCC.m_ext2Hi），
                                            // 源地址每次内联重算（P183，勿缓存指针局部）。
                                            sig->m_box.m_postals[rowIdx].m_tail.m_ext0 = extOpt.m_ext.m_e0;
                                            sig->m_box.m_postals[rowIdx].m_tail.m_ext1 = extOpt.m_ext.m_e1;
                                            sig->m_box.m_postals[rowIdx].m_tail.m_ext2Lo = extOpt.m_ext.m_e2Lo;
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_ext2Hi = extOpt.m_ext.m_e2Hi;
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_ext3 = extOpt.m_ext.m_e3;
                                            // ORIG 0x0841e830 `add $0x54,%edx; mov %al,(%edx)`
                                            sig->m_box.m_postals[rowIdx].m_tail.m_occ.m_field54 = b19;
                                            // ORIG 0x0841e848 `imul $0x61; add base; lea 0x8; call strncpy`
                                            strncpy(sig->m_box.m_postals[rowIdx].m_sendName, name9, 0x1d);


                                            SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID) =
                                                std::max(SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID),
                                                         (unsigned int)p0);
                                            ++rowIdx;
            }
        }
    }
    SIG_I32(sig, SIG_OFF_POSTAL_COUNT) = rowIdx;
    SIG_I32(sig, SIG_OFF_LETTER_ADD) = 0;

    // 第二循环：剩余行只读 letter_id，统计 map 频次与 +0x332e 增量
    int k;
    for (k = 0; k < SIG_I32(sig, SIG_OFF_REMAIN_COUNT); ++k)
    {
        if (!db->fetch())
            return 0;
        unsigned int u13b = 0;
        if (db->get_uint(13, u13b) && u13b != 0)
        {
            std::map<unsigned int, int>::iterator it = extraMap.find(u13b);
            if (it != extraMap.end())
                // ORIG 0x0841e9e5 `call ptEv; mov 0x4(%eax),%edx; add $1,%edx;
                // mov %edx,0x4(%eax)` ⇒ `+= 1`（`= it->second + 1` 恒发两次 ptEv，
                // 探针 /tmp/p2/mp.cpp a1/a2）。
                it->second += 1;
            SIG_I32(sig, SIG_OFF_LETTER_ADD) += 1;
        }
    }

    // map 键回写 +0x333a（stride 4，最多 0x14 项）
    if (!extraMap.empty())
    {
        // ORIG 0x0841ea78 `cmpl $0x13,-0x34; ja` ⇒ 无符号（jg 是 signed）
        unsigned int idx = 0;
        // ORIG 0x0841ea9e：begin() 直构进 it 槽（-0x124）+ 循环内每轮重调 end()
        // （0x0841eab8 call end）⇒ 直接初始化、非「默认构造 + 赋值」（P314/P50，
        // 旧形多发 _Rb_tree_iterator::C1Ev，p167 EXTRA）。
        std::map<unsigned int, int>::iterator it = extraMap.begin();
        for (; it != extraMap.end(); ++it)
        {
            // ORIG 0x0841ea63 `test %eax,%eax; jle` ⇒ 判据是 `> 0`（`>= 1` 产 cmp $1）。
            if (it->second > 0 && idx <= 0x13)
            {
                SIG_U32(sig, SIG_OFF_EXTRA_IDS + idx * 4) = it->first;
                ++idx;
            }
        }
    }

    int oldCount = SIG_I32(sig, SIG_OFF_POSTAL_COUNT);
    SIG_I32(sig, SIG_OFF_LETTER_COUNT) = (int)req->m_maxLetter;   // +0x1018
    if (!SelectLetter(db, slot, (int)req->m_characNo, req->m_letterIdx, sig))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x5438,
                              "SelectLetter() error");
        SIG_I32(sig, SIG_OFF_LETTER_COUNT) = 0;
    }
    // ORIG 0x0841eb8e `je 0x0841ebb4`（相等跳「postal==0」块）⇒ else 直落、
    // then 后置：反写条件才复现该块序（P272）。
    if (SIG_I32(sig, SIG_OFF_POSTAL_COUNT) != oldCount)
    {
        DoChangeMailList(sig, avataList, (int)req->m_maxPostalId);
    }
    else
    {
        if (SIG_I32(sig, SIG_OFF_POSTAL_COUNT) == 0)
            SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID) = req->m_maxPostalId;
    }

    SIG_I32(sig, SIG_OFF_USER_ITEM_CNT) = 0;   // +0x7a0
    if (avataList[0] != 0)
    {
        char sql2[0x1000] = { 0 };
        // ORIG 0x0841ec01..0x0841ec65：agency 的 0x19 字节对齐清零在
        // getList2inQuery 之前、ui 循环**之外** ⇒ 原码声明在块头（循环内声明
        // 会把整段清零搬进循环体，与 ORIG 顺序不符）。
        char agency[0x19] = { 0 };
        getList2inQuery(avataList, sql2);
        db->set_query(
            "seLect ui_id,unix_timestamp(reg_date),unix_timestamp(expire_date),ipg_agency_no,jewel_socket from user_items where charac_no=%d and ui_id %s",
            req->m_characNo, sql2);
        db->exec(true);
        sig->m_user.m_items[0].m_count = db->get_n_rows();

        // ORIG 0x0841eeeb `cmp -0x1c(%ebp),%eax; seta` ⇒ ui 无符号（setg 是 signed）
        unsigned int ui;
        for (ui = 0; ui < (unsigned int)db->get_n_rows(); ++ui)
        {
            if (!db->fetch())
                return 0;
            // [b28d-solo-DB_MailBox_Req_List] 真 bug 修正：ORIG 明细基址
            // sig+0x7a0+ui*0x47 的字段在 **+0x04 ui_id / +0x08 expire_date /
            // +0x0c reg_date / +0x10 agency / +0x29 jewel**（0x0841ee1a/ee35/ee50/
            // ee61 实证）；旧 MailUserItemView 缺前导 4 字节 pad ⇒ ui_id/reg_date/
            // agency/jewel 全部低移 4 字节。改用权威头 SIG_MAILBOX_USER_ITEM 视图。
            int uiId = 0;
            if (!db->get_int(0, uiId))
                return 0;
            unsigned int regDate = 0, expireDate = 0;
            if (!db->get_uint(1, regDate))
                return 0;
            if (!db->get_uint(2, expireDate))
                return 0;
            if (!db->get_str(3, agency, 0x18))
                return 0;
            stAvatarEmblemInfo_t emblem;
            emblem.init();
            if (!db->get_binary(4, &emblem, 0x1e))
                return 0;

            sig->m_user.m_items[ui].m_expireDate = expireDate;
            sig->m_user.m_items[ui].m_uiId = uiId;
            sig->m_user.m_items[ui].m_regDate = regDate;
            strncpy(sig->m_user.m_items[ui].m_ipgAgency, agency, 0x18);
            // ORIG 0x0841ee95..0x0841eed5 段为 7×dword + 1×word 结构体整体拷贝
            // （非 memcpy 调用；P167 EXTRA memcpy 由此归零）。
            *(stAvatarEmblemInfo_t*)sig->m_user.m_items[ui].m_jewel = emblem;
        }
    }

    sig->m_creature.m_items[0].m_count = 0;   // +0xd30
    // ORIG 0x0841ef03 `mov -0x214(%ebp),%eax; test; je body` + 调用后
    // `xor $1,%eax; test %al; je body` ⇒ 原码是短路 `||`（if/else + bool
    // 局部恒多发 movb/cmpb 两条）。
    if (creatureList[0] == 0 ||
        setCreatureData(db, req->m_characNo, creatureList, sig))
    {
        SIG_U32(sig, SIG_OFF_CHARAC_NO) = req->m_characNo;
        SIG_U32(sig, SIG_OFF_FIELD3336) = req->m_field10;

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5492), true);
        guard.operator*()->operator<<(0x7f);
        guard.operator*()->operator<<(slot);
        guard->put_binary(sig, 0x338a);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    return 0;
