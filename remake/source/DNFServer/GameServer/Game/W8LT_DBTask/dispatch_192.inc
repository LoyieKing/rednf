// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08440f3e
bool DB_UpdateItemMakingSkill::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_ITEM_MAKING_SKILL* sig = stream->GetOutBuffer<SIG_ITEM_MAKING_SKILL>();
    // [FL3W8LT 2026-09-10 P183/P328①] 删两处具名缓存（原 `characNo` -0x20 槽、
    // `skills` 指针 -0x1c 槽）：ORIG 0x08440f3e 全函数只留 sig 槽（-0x20），字段
    // 访问一律基址重读 + disp 折叠（`mov -0x20(%ebp),%eax; mov (%eax),%edi`
    // charac_no@+0x00；`movzwl 0x18(%eax)` = skills[10]）；我方旧码每个
    // skills[i] 各多一条 `add $K,%eax`（11 处）⇒ 全部改内联视图表达式。
    // [P101/P328⑩] exec 收尾按 ORIG 早返形：`call exec; **xor $1,%eax**; test %al;
    // je <insert 分支>`（0x0844109f 段）⇒ 写 `if (!db->exec(true))`；两处失败
    // trace 改单语句临时链（ORIG ctor 前 `mov -0x1c(%ebp),%eax; mov (%eax),%ebx`
    // 重读 charac_no 并跨 ctor 保活）。
    // [DNF-FIX 2026-10-08 c22] ORIG 0x08440fc2..0x0844112e 各级 `movzwl; movswl`：
    //   按 short 符号扩展传 %d（字段为 unsigned short ⇒ 使用点 (short) 收窄）。
    db->set_query(
        "upDate item_making_skill_info set weapon=%d, cloth=%d, leather=%d, light_armor=%d, heavy_armor=%d, plate=%d, amulet=%d, wrist=%d, ring=%d, support=%d, magic_stone=%d where charac_no =%u",
        (int)(short)sig->m_skillLevels[0], (int)(short)sig->m_skillLevels[1],
        (int)(short)sig->m_skillLevels[2], (int)(short)sig->m_skillLevels[3],
        (int)(short)sig->m_skillLevels[4], (int)(short)sig->m_skillLevels[5],
        (int)(short)sig->m_skillLevels[6], (int)(short)sig->m_skillLevels[7],
        (int)(short)sig->m_skillLevels[8], (int)(short)sig->m_skillLevels[9],
        (int)(short)sig->m_skillLevels[10], sig->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x9fde, 5)(
            "DB_UpdateItemMakingSkill::UpdateItemMakingSkill update, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into item_making_skill_info (charac_no, weapon, cloth, leather, light_armor, heavy_armor, plate, amulet, wrist, ring, support, magic_stone) values(%u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d )",
            sig->m_characNo, (int)(short)sig->m_skillLevels[0],
            (int)(short)sig->m_skillLevels[1], (int)(short)sig->m_skillLevels[2],
            (int)(short)sig->m_skillLevels[3], (int)(short)sig->m_skillLevels[4],
            (int)(short)sig->m_skillLevels[5], (int)(short)sig->m_skillLevels[6],
            (int)(short)sig->m_skillLevels[7], (int)(short)sig->m_skillLevels[8],
            (int)(short)sig->m_skillLevels[9], (int)(short)sig->m_skillLevels[10]);
        if (!db->exec(true))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x9ff5, 5)(
                "DB_UpdateItemMakingSkill::InsertItemMakingSkill insert, exec() ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
    }
    return 1;
}