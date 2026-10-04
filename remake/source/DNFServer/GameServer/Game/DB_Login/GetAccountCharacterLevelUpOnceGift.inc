// [S414_DB_Login] verdict=DIFF ndiff=10（15→10）：同 0x67 件，`bool execOk;` 提前
// 到 db 之前声明 ⇒ 槽位翻正（db -0x18→-0x14、execOk -0x11→-0x15，ORIG 槽
// -0x14/-0x15）。余 5 行 = N1 `(i+0x866)*8+8` 折 `(i+0x867)*8`（probe5 六形均
// `mov sig,%eax` 预载 vs ORIG `add 0xc(%ebp),%eax` 内存操作数）⇒ 维持封顶。
// 前史：P0/P183 具名 int* 局部修复 32→15。
bool DB_Login::GetAccountCharacterLevelUpOnceGift(SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P0/P140：同 0x67 件（无 evt 局部/判空）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x68)->IsEventing(0))
    {
        return 1;
    }
    bool execOk;                            // [solo 试验] 先声明，翻转 4.4 槽分配序
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("seLect job_type, level from event_1208_level_achieve where m_id = %d",
                  sig->m_accId);
    execOk = db->exec(true);                // P158：ORIG bool 槽（-0x15）
    if (!execOk)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xb4e8,
                              "LoadAccountCharacterLevelUpOnceGift Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if (db->get_n_rows() != 0)              // P222 正极性包裹
    {
        for (unsigned int i = 0; i < db->get_n_rows(); ++i)
        {
            if (!db->fetch())
            {
                return 0;
            }
            if (i > 3)
            {
                return 0;
            }
            // [b28d-solo-DB_Login] P0/P183：ORIG 有**具名 int* 局部**（-0xc 槽，
            // 两个 getter 各回读一次：`mov -0xc(%ebp),%eax; mov %eax,0x8(%esp)` /
            // `mov -0xc(%ebp),%eax; add $0x4,%eax`）⇒ 原码是「指针局部 + 下标直读」，
            // 非 jobType/level 两个 int 局部（多 2 槽 + 2 次字段写回）。
            // 地址链 N1：ORIG `add $0x866; shl $3; add sig; add $0x8` 被 4.4 折成
            // `(i+0x867)*8`（探针 probe_gift f5）⇒ 该 3 条常量折叠差封顶。
            // [S216] N1 复核：P205 显式拆形 `(i+0x866)*8+8` 实测仍被 4.4 折成
            // `(i+0x867)*8`（ndiff 30 不变）⇒ 维持 OFF_ONCE_GIFT 常量形。
            // [Lane26 复核] 探针再证：h1/h2 `(char*)(arr+i+0x866)+8`、`&arr[i+0x866]+8`
            // 均被 4.4 折 `add $0x867; shl $3`（/tmp/l26p/p3.cpp）；i6
            // `(int*)((char*)&arr[i+0x866].a)+2` 可保 `add $0x866; shl $3; add sig;
            // add $8` 五条但恒 sig-first（ORIG i-first）⇒ 常量拆分不可全复现，N1 封顶。
            // [K2ADBLogin 2026-09-11] P235 真实成员数组取址：ORIG 0x084489xx 为
            //   `mov i,%eax; add $0x866,%eax; shl $0x3,%eax; add 0xc(%ebp),%eax;
            //    add $0x8,%eax`（index 先、base 内存操作数后置、常量两段 0x866/8）；
            //   cast 指针形 4.4 恒 base-first + 折叠 `add $0x867`（旧 5 行失配）。
            int* giftSlot = reinterpret_cast<int*>(&sig->m_levelupGiftAccountOnce.m_entries[i]);
            if (!db->get_short(0, giftSlot[0])) return 0;
            if (!db->get_int(1, giftSlot[1])) return 0;
        }
    }
    else
    {
        memset(reinterpret_cast<char*>(&sig->m_levelupGiftAccountOnce), 0, 0x20);
    }
    return 1;
}