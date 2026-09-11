// ===========================================================================
// _updateCreateEventCharacter — 报告 @0x08401428
// 同事件窗内置 member_login.event_charac_flag=1 并回写角色标志。
// 52/52 逐条对齐：窗口右值内联（P0/P183）、GetDBHandle(6,0)+
// NumberToString(m_accId)+set_query+exec(true)、失败 rc4=2@0x5344、
// 成功 flag=1@0x24 返 1、窗外 rc4=4 块置函数尾（与 ORIG 同序）。
// 残余文本差均为 call/jmp 重定位与数据地址占位（§6.3），relink 后转 AE。
// ===========================================================================
bool DB_CreateCharac::_updateCreateEventCharacter(SIG_CREATE_CHARAC* sig)
{
    // P0/P183：窗口计算右值内联（ORIG 寄存器持有，无 nowSec/inWindow 槽）。
    if (GlobalData::s_systemTime_.getCurSec() > (int)0x4b055d6f &&
        GlobalData::s_systemTime_.getCurSec() <= (int)0x4b3bbf50)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                      E_SERVER_GROUP_NONE);
        db->set_query("upDate member_login set event_charac_flag = 1 where m_id = %s",
                      NumberToString(sig->m_accId, 0));
        if (!db->exec(true))
        {
            sig->m_resultCode = 2;
            return false;
        }
        sig->m_eventCharacFlag = 1;
        return true;
    }
    sig->m_resultCode = 4;
    return false;
}