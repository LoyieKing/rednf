// ===========================================================================
// CreatePvP — 报告 @0x084026ec
// ===========================================================================
bool DB_CreateCharac::CreatePvP(unsigned int characNo)
{
    // N93 翻案（P212 标量例外）：4.1 先声明占浅槽（db@-0xc，ok@-0xd 后声明更深）；
    // 4.4.7 相反——把 ok 先声明（无初始化、仅赋值使用），db 后声明，
    // 4.4 槽序翻为 ok 深 db 浅 = ORIG 位。register asm("al") 绑定（N96 违例）删除，
    // 裸 `if(!ok)` 形与 ORIG mov%al,-off;movzbl;xor$1;test;je 同形（P168a①）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("inSert into pvp_result (charac_no) values(%u)", characNo);
    ok = db->exec(true);
    if (!ok)
        return false;
    return true;
}