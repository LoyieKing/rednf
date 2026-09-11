// ORIG 0x0815a8f2 T LoadServiceRestrictPolicy::dispatch（报告 dispatch.md）：
// GetDBHandle(1,0) null→throw "handle null"；set_query 失败 throw "set_query"；
// exec 失败 throw "exec"；逐行 get_int(++idx,&v)/get_char(&c) 失败 throw
// "category"/"restrict"/"values"；sig.restrictValue==0 或 ==v1 时 sig.set(v2,v1,c)，
// 否则先 sendRestrictData+init 再 set；末尾 sendRestrictData。catch 0xd2 返 0。
// [b28d-solo-TaiwanDBDispatcher] DIFF·N 封顶（201/201，仅 3 处实参求值序）：①真 bug
// 修正 set(v2,v1,c)→set(v1,v2,c)（ORIG 0x0815a8f2 两处压栈序 0x4=v1(-0x1c)/0x8=v2
// (-0x20)/0xc=c 实证；旧写使 m_restrictValue 与 m_extra 索引互换）；②TaiwanQueries.h
// SigLoadRestrictPolicy 加 pack(1)（P250：ORIG 槽基 -0x1221 非 4 对齐）⇒ 槽基/块序归零。
// 残余：三处 get_int/get_char 实参求值序——ORIG 左→右（idx 先入 %eax、&v 后入 %edx），
// 4.4 恒右→左（先 lea &v）；实测语句表达式/逗号/(int)/(unsigned)/引用 cast/内联 ++idx/
// idx+=1 七形均不可翻（(short)idx 可对齐求值序但多 3 条 cwtl 且引入截断，弃用）；
// 实参值/压栈序/调用目标全同 ⇒ P75 跨版本实参装载时刻，语义等价封 N。
bool LoadServiceRestrictPolicy::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        if (!db->set_query(
                "seLect category, restrict_code, restrict_value from dnf_restrict_state where server_group=%d",
                serverGroup))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        Taiwan::SigLoadRestrictPolicy sig;
        sig.init();
        while (db->fetch())
        {
            int idx = 0;
            int v1 = 0;
            int v2 = 0;
            char c = 0;
            // [P2S5] ORIG 0x0815aa1e/0x0815aa6e/0x0815aab6 三处实参为**后置**自增：
            // `mov -0x10(%ebp),%eax; addl $0x1,-0x10(%ebp); …; mov %eax,0x4(%esp)`
            // （加载旧值→内存自增→压旧值；4.4 实参左→右求值）。列索引 0-based
            // （MySQL::is_valid_col 允许 col>=0），故 ORIG 逐列传 0/1/2 ⇒ 用 idx++。
            if (db->get_int(idx++, v1) != 1)
                throw "category";
            if (db->get_int(idx++, v2) != 1)
                throw "restrict";
            if (db->get_char(idx++, c) != 1)
                throw "values";
            if (sig.m_restrictValue == 0 || sig.m_restrictValue == v1)
            {
                // [b28d-solo-TaiwanDBDispatcher] 真 bug 修正：ORIG 0x0815a8f2 两处
                // set 压栈序 (0x4=v1,-0x1c / 0x8=v2,-0x20 / 0xc=c) ⇒ 原码 set(v1,v2,c)；
                // 旧写 set(v2,v1,c) 使 m_restrictValue 与 m_extra 索引互换（可观察行为错）。
                sig.set(v1, v2, c);
            }
            else
            {
                sendRestrictData(sig);
                sig.init();
                sig.set(v1, v2, c);
            }
        }
        sendRestrictData(sig);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0xd2,
            "[Taiwan, Restrict] select dnf_restrict_state. (error:%s)", e);
        return 0;
    }
}
