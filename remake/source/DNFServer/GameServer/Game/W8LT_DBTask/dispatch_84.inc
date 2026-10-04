// ============================================================================
// DB_UpdateCreatureItem::dispatch —— ORIG 0x084203e8（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_CREATURE_ITEM> → 按 m_field64(+0x64) 分支：
//   1: m_mapSize(+0x68)==0 → upDate inventory set creature_flag=1 where
//      charac_no=m_characNo(+0x48)；m_field70(+0x70)==1 → 再 creature_flag=0
//      where charac_no=m_field6c(+0x6c)
//   3/5: m_mapSize==1 → creature_flag=0 where charac_no=m_field6c
//   4: m_mapSize==0 → creature_flag=1 where charac_no=m_characNo
// 各 exec 失败返 0。GetField 生成 SET 子句 → upDate creature_items set %s
// where ui_id=m_addInfo2(+0x30)；exec 失败返 0。m_field64==4||1 时组回包：
// Acquire("DBThread.cpp",0x5710) → guard<<0x8b → guard<<slot →
// GetInBuffer<SIG_CREATURE_ITEM> → memcpy 0x74 → seLect 9 列
// （slot,it_id,name,stomach,exp,endurance,creature_type,no_charge,
// unix_timestamp(expire_date)）逐列读入回包体；任一失败
// cMyTrace(0x5729..0x5794,5) + Free(guard) 返 0；全成 put(DISPATCH_Q,guard)
// 返 1。其余分支返回 1。
// [SweepW8LT b28d] DIFF：gap 14（our 594 / ORIG 608）封顶。p167 72/72 [0缺0多]
// （cMyTrace C1/C2、CStreamGuard C1/C2/D1/D2 同址别名）。残差：① P0 具名 bool——
// ORIG 前 6 处检查（exec/fetch/get_int0/get_ulong1/get_int3/get_int4）产
// `movzbl %al,%eax; mov %eax,-0x1c; cmpl $0,-0x1c; jne`（共用 -0x1c 槽），后 5 处
// 与 4.4 全部为 `xor $1; test %al,%al; je`（各 -1）；② P231 ORIG 错误路径用 %ebx
// 跨 ctor 保活 sig->m_characNo（`mov 0x48(%eax),%ebx`），4.4 每处重载；③ N78
// rep movsl 段 %esi 多一条中转。ORIG 084203e8
bool DB_UpdateCreatureItem::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_CREATURE_ITEM* sig = stream->GetOutBuffer<SIG_CREATURE_ITEM>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    switch (sig->m_field64)
    {
    case 1:
        if (sig->m_mapSize == 0)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                1, sig->m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
        if (sig->m_field70 == 1)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                0, sig->m_field6c);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    case 3:
        if (sig->m_mapSize == 1)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                0, sig->m_field6c);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    case 4:
        if (sig->m_mapSize == 0)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                1, sig->m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    // [b28d-solo] P167：ORIG case 3 与 case 5 各自独立一份 set_query+exec
    // （差集 -1 set_query/-1 exec；ORIG 0x08420481 与 0x084205e4 两处），
    // 合并 case 标签只发一份 ⇒ 拆开；case 序按 P46 升序（1,3,4,5）。
    case 5:
        if (sig->m_mapSize == 1)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                0, sig->m_field6c);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    default:
        break;
    }
    char setClause[0x400] = { '\0' };
    GetField(db, setClause, sig);
    db->set_query(
        "upDate creature_items set %s where ui_id=%d",
        setClause, sig->m_addInfo2);
    if (db->exec(true) != 1)
        return 0;
    if (sig->m_field64 != 4 && sig->m_field64 != 1)
        return 1;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5710), true);
        guard.operator*()->operator<<((int)0x8b);
        guard.operator*()->operator<<((int)slot);
        // [b28d-solo] P103/P167：ORIG 调 operator->（ptEv）；N84：结构整体
        // 赋值（ORIG 内联 rep movs 0x74），非 memcpy call（p167 EXTRA）。
        SIG_CREATURE_ITEM* out =
            guard.operator->()->GetInBuffer<SIG_CREATURE_ITEM>();
        *out = *sig;
        // ORIG 0x08420759 movl $0x0,-0x1c(%ebp)：具名 int 结果局部，各检查
        // 逐次赋值后 `movzbl %al,%eax; mov %eax,-0x1c; cmpl $0,-0x1c; jne`
        // （P102 同一局部保持失败出口；旧内联 `!call()` 少 1 条/处）
        int ok = 0;
        db->set_query(
            "seLect slot,it_id,name,stomach,exp,endurance,creature_type,no_charge,unix_timestamp(expire_date) from creature_items where ui_id=%d",
            sig->m_addInfo2);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5729, 5);
            trace("DB_UpdateCreatureItem::dispatch, exec() ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5731, 5);
            trace("DB_UpdateCreatureItem::dispatch, fetch() ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->get_int(0, out->m_slot);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5739, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(0) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // it_id 列回填 +0x08（DB_CreatureItemInserView::m_itId；32 位
        // unsigned long==unsigned int 同宽，ORIG 传槽地址）。
        ok = db->get_ulong(
            1, *reinterpret_cast<unsigned long*>((void*)&out->m_itemView.m_itemIdx));
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5741, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_ushort(1) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // [A04-r5] ORIG 0x084209d5-0x084209df 为 `call get_str; xor $0x1,%eax;
        // test %al,%al; je`（**内联 bool 直判**，不经 ok 槽）⇒ 后 4 个检查
        // （get_str/get_ushort/get_int(6)/(7)/(8)）必须内联，与 0x08420790 等
        // 前 6 处 `movzbl %al,%eax; mov %eax,-0x1c; cmpl $0,-0x1c; jne`（ok 槽形）区分。
        if (!db->get_str(2, out->m_name, 0xd))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5756, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(2) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->get_int(3, out->m_stomachValue);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x575e, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(3) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->get_int(4, out->m_exp);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5766, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(4) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_ushort(5, out->m_itemView.m_endurance))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x576d, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_ushort(5) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // [R21a15 §27.5] ORIG 0x08420b8e 段：`lea -0xe4(%ebp),%eax` 前**无**
        // `movl $0x0` 清零 ⇒ 原码未初始化（`int creatureType;`）；旧版多写
        // `= 0` 多发 1 条 `movl $0x0,-N(%ebp)`（用户裁定：照抄 ORIG，含未初始化读）。
        int creatureType;
        // ORIG 0x08420b8e：`call get_int; xor $0x1,%eax; test %al,%al; je`（内联）。
        if (!db->get_int(6, creatureType))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5775, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(6) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        out->m_flag50 = (unsigned char)creatureType;
        // [R21a15 §27.5] 同上：ORIG 0x08420c93 `lea -0xe8(%ebp),%eax` 前无清零。
        int noCharge;
        if (!db->get_int(7, noCharge))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x577e, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(7) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        out->m_keepRight = (unsigned char)noCharge;
        if (!db->get_int(8, out->m_days))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5794, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(9) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}