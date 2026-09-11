// ============================================================================
// DB_LoadEtc::LoadFairPvPScore @0x08404290 —— fair_pvp_score 14 列；
// n_rows==0 时补插并返回；give_item 压缩列（cap 0x650）→ +0xcf4b；
// daily 重置比较（last_play_time < getDailyInitializeTime(6,0,0) → +0xd59b=0）。
// ============================================================================
// b28d-31: N 封顶（N82）——ORIG 0x08404290；474 vs 476（帧/寄存器分配），
// p167 待核。
// [b28d-solo-DB_LoadEtc] N封顶 ndiff=516 (orig=474/ours=476)：N82 帧/槽错位（14 列 + 13 处
// trace 的复用 bool ok 槽）；p167 CLEAR 0/0 ⇒ 语义完成、byte 层封顶（§6.2）。
//   [b28d-solo 复测] 474=474 vs 476、p167 CLEAR 20/20；P221/P227 临时链试形对 trace 槽
//   （ORIG -0xe8 深 vs 4.4 -0x28）**零效**（4.1 不复用 13 个 trace 槽，N82 真因）⇒ 封顶。
// [b29-Lane21 复核] 「trace 槽零效」系 fndiff 对齐错觉：两侧 lea -0xe8..-0x28 分布实测全等。
//   真修法三条已落：①`bool ok;` 前置声明（P212，ok=-0x15/db=-0x14 就位）；②`int col = 0;`
//   移到 fetch 检查之后（ORIG +0x1d1 col/i 连发）；③13 处具名 cMyTrace 改全表达式临时链
//   （P227：ctor 前 vararg 装 %ebx 跨 ctor 保活）。读数 ndiff 516→14（orig 474/our 476）。
// [b29-Lane21] 残差 11 行 = N1 常量拆分：ORIG `shl $2; add $0xcf10; add sig; lea 0x13`
//   vs 4.4 `add $0x33c8; shl $2; add sig; lea 3`（0xcf23 分解 mod16+0x13 vs mod4+3，同址
//   sig+0xcf23）；槽集两侧逐值全等 ⇒ 判据⑪ (a) 不成立，语义等价。
//   残差（N1 常量拆分，5 形实测）：ORIG `shl $2; add $0xcf10; add sig; lea 0x13` vs 4.4
//   `add $0x33c8; shl $2; add sig; lea 3`（0xcf23 非 4 对齐的拆分点不同）；&arr[K] /
//   arr+K / 视图指针 / 两段常量 / 行视图五形均折 add。
bool DB_LoadEtc::LoadFairPvPScore(SIG_LOAD_ETC* sig)
{
    static const int handleIdx = 3;    // ORIG _ZZN10DB_LoadEtc16LoadFairPvPScore…E9handleIdx
    // ORIG 槽序：ok(-0x15) 深于 db(-0x14) ⇒ 原码 ok 先声明（P212 全头部声明「先声明深」）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect private_win,private_lose,private_draw,relay_battle_win,"
                  " relay_battle_lose, relay_battle_draw,relay_battle_2kill,"
                  " relay_battle_3kill, successive_win, max_successive_win,"
                  " job_score,daily_play_count, unix_timestamp(last_play_time),"
                  " give_item from fair_pvp_score where charac_no=%u",
                  sig->m_characNo);
    // ORIG local_19（-0x15）为复用的 bool 结果槽：每步 call 后 `mov %al,-0x15;`
    // `movzbl; xor$1; test; je` —— 命名 bool ok 赋值再判触发同形。
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1618, 5)(
            "DB_LoadSkill::LoadFairPvPScore, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into fair_pvp_score (charac_no) values(%u)",
                      sig->m_characNo);
        ok = db->exec(true);                                // ORIG local_19 复用同槽
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1625, 5)(
                "DB_LoadEtc::LoadFairPvPScore insert, exec() ERROR"
                  " charac_no=%u", sig->m_characNo);
            return false;
        }
        return true;
    }

    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x162f, 5)(
            "DB_LoadEtc::LoadFairPvPScore, fetch() ERROR charac_no=%u",
              sig->m_characNo);
        return false;
    }

    int col = 0;                                            // 8904461 movl $0,-0x10
    // 前 6 列：i∈[0,1] 外层循环内 3 个显式 get（ORIG 8904474..8904636 无内层循环，
    // 字段地址 = sig + i*0xc + 0xcf23/27/2b；行号 0x1638/0x163f/0x1646）。
    for (int i = 0; i <= 1; ++i)
    {
        ok = db->get_uint(col++, sig->m_fairPvpScore[i * 3 + 0]);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1638, 5)(
                "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
        ok = db->get_uint(col++, sig->m_fairPvpScore[i * 3 + 1]);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x163f, 5)(
                "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
        ok = db->get_uint(col++, sig->m_fairPvpScore[i * 3 + 2]);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1646, 5)(
                "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
    }
    // 4 个独立 uint 列（8904649.. 平铺 je 前进/失败落 log 岛；行号 1667/6e/75/7c）
    ok = db->get_uint(col++, sig->m_fairPvpScore[6]);        // successive_win +0xcf3b
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1667, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpScore[7]);        // max_successive_win +0xcf3f
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x166e, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpScore[8]);        // job_score +0xcf43
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1675, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpScore[9]);        // daily_play_count +0xcf47
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x167c, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }

    // give_item 压缩列：fail 无 log（ORIG else 直落 return 0 岛 @89053xx）
    if (!get_compressed_blob_data(db, col++, sig->m_fairPvpGiveItem, 0x650, 0))
    {
        return false;
    }
    ok = db->get_uint(col++, sig->m_fairPvpLastPlayTime);    // +0xd59b
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1689, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    // get_int 直写 sig+0xd59f（ORIG 无中间局部；行号 0x1690）
    ok = db->get_int(col++, sig->m_fairPvpLastPlaySec);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1690, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    if (sig->m_fairPvpLastPlaySec < getDailyInitializeTime(6, 0, 0))
    {
        sig->m_fairPvpLastPlayTime = 0;                  // ORIG movl $0x0,+0xd59b
    }
    // 末列 get_ubyte 直写 +0xd5a3（行号 0x16a5）
    ok = db->get_ubyte(col++, sig->m_fairPvpGiveFlag);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x16a5, 5)(
            "DB_LoadEtc::LoadFairPvPScore, get(%d) ERROR charac_no=%u",
            col, sig->m_characNo);
        return false;
    }
    return true;
}