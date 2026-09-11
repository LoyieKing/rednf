bool DB_UpdateUserGiftFatigueQuantity::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 0x08441d1c：event_script_mng 空返 0；实体 0x65 为空或
    //   !CUsedFatigueGiveItem::getIsCharacEvent(charac) 返 0；>>u1 >>u2 >>u3 后按
    //   charac 选 DB_HANDLE_9/DB_HANDLE_2 与 m_id/charac_no 双查询，exec 失败返 0。
    // b28c-34 判词（N7 封顶，推断）：
    //   ①charac inSert 分支实参序修正为 values(charac_no=u2, fatigue_quantity=u3)：
    //     ORIG 0x0841f56 `mov -0x24(%ebp),%esi; mov -0x20(%ebp),%ebx` ⇒ 0xc(%esp)=u2、
    //     0x10(%esp)=u3，与列序 (charac_no, fatigue_quantity) 一致（旧码 u3,u2 为真反）。
    //   ②evt 声明为 CUsedFatigueGiveItem*（赋值处 static_cast）以复现 ORIG 0x0841d64
    //     调用点 `lea -0x15(%ebp),%eax; mov %eax,0x4(%esp); mov -0x10(%ebp),%eax` 的
    //     「&charac 先装载」实参序；表达式内 static_cast 形 4.4 会反转装载序（探针
    //     /tmp/gf_probe2.cpp t5/t7 反形 vs t2/t3/t6 正形）。
    //   ③残余 NEAR 仅 N7（结构性，不可复现）：ORIG 0x0841df1 虚目标 `mov (%eax),%edx`
    //     + this 重载 `mov -0x10(%ebp),%eax; call *%edx`；4.4 -O0 对间接 call 目标恒
    //     %eax、this 恒 %edx（探针 /tmp/gf_probe3.cpp t1 成员调用/t2 引用/t3 解引用/t4
    //     丢弃全为 `call *%eax`）。已试形：成员调用、(*evt) 解引用、引用形、丢弃形；
    //     槽位 +0x20 与 ORIG vtable slot 8 一致（非 P193 选错虚函数）。语义与 ORIG 等价。
    //   实测：TU 级 compare 由 DIFF→NEAR（190/190 insn、gap=0），ndiff 16→8（残余
    //   8 行全部为上述 N7 寄存器名差）；探针 TU /tmp/gf_before.cpp 复刻修改前体得
    //   ndiff_before=16（口径 audit_tu_ledger：AE 归一+<T>/<A>+尾 nop 豁免）。
    EventClassify::CEventScriptMng* mng =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    if (mng == 0)
        return 0;
    EventClassify::CUsedFatigueGiveItem* evt =
        static_cast<EventClassify::CUsedFatigueGiveItem*>(mng->get_event_entity(0x65));
    bool charac = false;
    if (evt == 0 || !evt->getIsCharacEvent(charac))
        return 0;
    unsigned int u1 = 0;
    unsigned int u2 = 0;
    unsigned int u3 = 0;
    stream->operator>>(u1);
    stream->operator>>(u2);
    stream->operator>>(u3);
    MySQL* db = 0;
    std::string table(evt->get_db_table_name());
    // ORIG 0x08441e0e-0x08441e13 `xor $1;test;je <DB_2块>`：charac==1 →
    // DB_2/charac_no 查询、charac==0 → DB_9/m_id（getIsCharacEvent 语义
    // 为“走角色维度”，前批分支写反，本次按 ORIG 汇编修正）。
    if (!charac)
    {
        db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate %s set fatigue_quantity=%u where m_id=%s",
            toTString(table), u3, NumberToString(u1, 0));
    }
    else
    {
        db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate %s set fatigue_quantity=%u where charac_no=%u",
            toTString(table), u3, u2);
    }
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        if (!charac)
        {
            db->set_query(
                "inSert into %s(m_id, fatigue_quantity) values(%s, %u)",
                toTString(table), NumberToString(u1, 0), u3);
        }
        else
        {
            db->set_query(
                "inSert into %s(charac_no, fatigue_quantity) values(%u, %u)",
                toTString(table), u2, u3);
        }
        if (!db->exec(true))
            return 0;
    }
    return 1;
}