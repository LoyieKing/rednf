// ============================================================================
// DB_ReqItemUpgradeLog::dispatch —— ORIG 0x08425dbe（报告
// docs/class_func_reports/DB_ReqItemUpgradeLog/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_4（LOGDB）。流解包 4×uint（a,b,c,d）+ bool e + char str[29]；
// inSert into log_item_develop(occ_time,it_no,server_id,charac_no,charac_name,
// develop,sucess_flag) values(now(),%d,%d,%d,'%s',%d,%d) =
// (b,a,c,str,d,(uint)e)；exec(true)。Ghidra 标 void：函数尾无置值——ORIG 源码
// 为 bool 覆盖且体末无显式 return（eax 残留 exec 返回值，UB 与原版一致，
// 同 DB_UpdateAvatarJewelSlot::dispatch 模式）。故此处不写 return。
bool DB_ReqItemUpgradeLog::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序 db,a,b,c,d,str,e 与 ORIG 栈槽对应（-0xc/-0x10/-0x14/-0x18/
    // -0x1c/-0x39/-0x3a；gcc O0 先声明→高地址）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int a = 0;
    unsigned int b = 0;
    unsigned int c = 0;
    unsigned int d = 0;
    char str[29];
    bool e = true;
    *stream >> a;
    *stream >> b;
    *stream >> c;
    *stream >> d;
    *stream >> e;
    *stream >> str;
    db->set_query("inSert into log_item_develop(occ_time,it_no,server_id,charac_no,charac_name,develop,sucess_flag) values(now(),%d,%d,%d,'%s',%d,%d)",
                  b, a, c, str, d, (unsigned int)e);
#if !remake_verify
    // [bughunt15] ORIG 0x08425ec1 `call MySQL::exec` 后直接 epilogue ⇒ 返回 exec 结果。
    return db->exec(true);
#else
    db->exec(true);
#endif
}