// ORIG T _ZN27CInGameAdvertisementManager17queryADExposeInfoEP25stInGameADExposeCondition
// @0x080fd19e（报告 queryADExposeInfo.md 全转写）：DB_HANDLE_9 取连接，
// "seLect * from in_game_ad where visible = 1"；行 → 0x84c 条件记录逐列填充
//（失败即返 0）；循环后写哨兵（cond[i]+0x210 = -1）并双 trace，返 1。
// ORIG 形态：char 暂存槽 b(-0x15) 先于 db(-0x14) 声明；cMyTrace 临时链式
//（P37，__PRETTY_FUNCTION__）；无 rec 命名局部（每列 cond+i 内联重算）。
bool CInGameAdvertisementManager::queryADExposeInfo(stInGameADExposeCondition* cond)
{
    bool b;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("seLect * from in_game_ad where visible = 1");
    b = db->exec(true);
    if (!b)
    {
        return false;
    }
    int nRows = db->get_n_rows();
    int i = 0;
    while (i < nRows)
    {
        b = db->fetch();
        if (!b)
        {
            break;
        }
        if (199 < i)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xeb, 5)(
                "[IN_GAME_ADVERTISEMENT_ERROR] count(%d)", nRows);
            break;
        }
        b = db->get_uint(0, reinterpret_cast<unsigned int&>(cond[i].m_sentinel));
        if (!b) return false;
        b = db->get_str(1, cond[i].m_title, 0x32);
        if (!b) return false;
        b = db->get_byte(2, cond[i].m_pad0[0]);
        if (!b) return false;
        b = db->get_byte(3, cond[i].m_pad0[1]);
        if (!b) return false;
        b = db->get_byte(4, cond[i].m_pad0[2]);
        if (!b) return false;
        b = db->get_str(5, cond[i].m_pad0 + 3, 0x200);
        if (!b) return false;
        b = db->get_str(6, cond[i].m_field246, 0xff);
        if (!b) return false;
        b = db->get_uint(7, cond[i].m_field444);
        if (!b) return false;
        b = db->get_str(8, cond[i].m_field345, 0xff);
        if (!b) return false;
        b = db->get_str(9, cond[i].m_field54a, 0xff);
        if (!b) return false;
        b = db->get_str(10, cond[i].m_field649, 0xff);
        if (!b) return false;
        b = db->get_uint(0xb, cond[i].m_field748);
        if (!b) return false;
        b = db->get_str(0xc, cond[i].m_field74c, 0xff);
        if (!b) return false;
        b = db->get_uint(0xd, cond[i].m_minCera);
        if (!b) return false;
        b = db->get_uint(0xe, cond[i].m_maxCera);
        if (!b) return false;
        b = db->get_byte(0xf, cond[i].m_field547);
        if (!b) return false;
        b = db->get_ubyte(0x11,
            reinterpret_cast<unsigned char&>(cond[i].m_pad0[0x203]));
        if (!b) return false;
        b = db->get_short(0x12, cond[i].m_key);
        if (!b) return false;
        b = db->get_str(0x14, cond[i].m_field448, 0xff);
        if (!b) return false;
        b = db->get_byte(0x15, cond[i].m_field548);
        if (!b) return false;
        ++i;
    }
    cMyTrace(__PRETTY_FUNCTION__, 300, 9, true, true)(
        "[!] Load In Game Advertisement Expose Info From DB(%d) Complete", nRows);
    cond[i].m_sentinel = -1;
    return true;
}