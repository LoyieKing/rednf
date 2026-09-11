bool DB_LoadEtc::LoadCharacOption(int userIdx, int slot, SIG_LOAD_ETC* sig)
{
    (void)userIdx;   // ORIG 仅透传第 3 参起作用（in_stack_00000010）
    (void)slot;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect options , best_clear_time, blue_marble_enter_count,"
                  " charac_inform_notice from charac_option where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    if (!db->fetch())
    {
        return false;
    }

    // [b28d-solo-DB_LoadEtc] ORIG 用自增列号局部（-0x18，`mov -0x18,%eax; addl $1,-0x18`
    //   取后自增）；原字面量列号使 4.4 直填常量（`movl $0x1,0x4(%esp)`）。
    int col = 0;
    // P212 全头部声明 + ORIG 槽深序（db-0x1c→col-0x18→count-0x14→i-0x10→idx-0xc）。
    int count;
    int i;
    int idx;
    if (!db->get_binary(col++, reinterpret_cast<SigLoadEtcOptionsView*>(sig)->m_options,
                        0x200))                                 // col0 options
    {
        return false;
    }
    if (!get_compressed_blob_data(db, col++, sig->m_bestClearTime, 24000, 0))
    {                                                        // col1 best_clear_time
        // ORIG 0x0840859f：ctor 前 `mov 0x4(%eax),%ebx` 缓存 characNo（P227 临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1f5d, 5)(
            "DB_LoadEtc::LoadCharacOption best_clear_time, get(%d) ERROR"
            " charac_no=%u", col /*local_18 已自增*/, sig->m_characNo);
        return false;
    }

    for (i = 0; i <= 4; ++i)                                 // base sig+0xC+(i+0x3b34)*4
    {
        // ORIG `mov 0xc(%eax,%edx,4)`（edx=i+0x3b34）= sig+0xECDC+i*4：具名数组成员
        // 视图（P291）产 scaled 索引；P0：无指针局部槽往返。
        if (reinterpret_cast<SigLoadEtcNegClampView*>(sig)->m_clamp[i + 0x3b34] < 0)
        {
            reinterpret_cast<SigLoadEtcNegClampView*>(sig)->m_clamp[i + 0x3b34] = 0;
        }
    }

    sig->m_blueMarbleEnterCount = 0;                          // 先置 0 再回填
    if (!db->get_ubyte(col++, sig->m_blueMarbleEnterCount))
    {
        return false;
    }
    // charac_inform_notice @+0x14a9a = attendanceEvent(+0x14a99)+1 重叠区。
    if (!db->get_str(col++, reinterpret_cast<SigLoadEtcNoticeView*>(sig)->m_notice, 16))
    {
        return false;
    }

    count = 0;
    idx = 0;
    // ORIG 0x08408718..0x0840873c：地址 = sig + idx*0x18 + 0xecd0 + 8 = +0xecd8 + idx*24
    // （`lea (%ecx,%eax,1); add $0xecd0; mov 0x8(%eax)`，P185 直读）——读**元素首 int**；
    // 旧式 `m_bestClearTime + idx*0x18 + 8` 多 8 字节（真 bug：读元素内第 3 个 int）。
    // 语义：首条 0 即 break（`test %eax,%eax; je`），非零则 count++/idx++。
    while (idx <= 0x3e7)
    {
        // [b29-Lane21] 成员偏移两种分解实测：①`m_rows[idx].m_first`（本形）⇒ 4.4
        // `add $0xecd8; mov (%eax)`（8 折进基址常量，ORIG 为 `add $0xecd0; mov 0x8(%eax)`）；
        // ②`*(int*)((char*)&m_rows[idx] + 8)` ⇒ `add $0xecd0; add $0x8; mov (%eax)`（多 1 条）。
        // 两形各含对方缺的一半 ⇒ N1 两步常量折叠族封顶。
        if (reinterpret_cast<SigLoadEtcBestClearTimeView*>(sig)->m_rows[idx].m_first == 0)
            break;
        ++count;
        ++idx;
    }
    sig->m_bestClearTimeCount = count;
    return true;
}