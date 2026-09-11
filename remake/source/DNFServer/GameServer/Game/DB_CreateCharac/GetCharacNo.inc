// ===========================================================================
// GetCharacNo — 报告 @0x0840275c：@@identity → sig->m_characNo。
// ===========================================================================
bool DB_CreateCharac::GetCharacNo(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // P211 异型化翻法（探针 /tmp/bl10/p9.cpp w1/w2）：局部声明 **int** 而字段
    // unsigned ⇒ 4.4 store 翻为 ORIG 的 RHS 值先载（`mov -0xc,%edx; mov p,%eax`），
    // 无额外中转条数；同型 unsigned 恒 LHS 地址先载。
    int identity = (int)WongWork::DBCommon::GetIdentity(db);
    if (identity == 0)
    {
        return false;
    }
    sig->m_characNo = identity;
    return true;
}