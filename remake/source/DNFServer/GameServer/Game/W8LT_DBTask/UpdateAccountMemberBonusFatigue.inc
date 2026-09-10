bool DB_UpdateLogout::UpdateAccountMemberBonusFatigue(unsigned int accId,
                                                      short memberBonusFatigue)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("upDate login_common set member_bonus_fatigue=%d where m_id=%s",
                  memberBonusFatigue, NumberToString(accId, 0));
    // ORIG +0x97 `test %al,%al; je +0xe5`：v=!exec||rows==0 choose 汇聚于
    // eax（mov$1/mov$0 无槽），false 路径（更新成功）先落 mov$0 块，
    // body 尾 `xor$1;test` 直落 epilogue。
    // [b28c-L18 spec，L19 代落并实测] P222 正向包裹形（if(exec==1&&rows!=0)return false）
    // 反劣：缺 ORIG 的 `xor $1` 段且仍产 mov$0 岛 ⇒ 维持原形，记 N3/N93附。
    // [b28d-solo-4] 早返形（if(ok)return false; body; return !exec）实测同 DIFF：
    // 4.4 恒把 return false 的 mov$0 块放 body 之后（body 尾多 jmp），ORIG 把它
    // 并入 choose 汇聚 +0x92（body 尾 xor$1;test 直落）⇒ N93附 尾块发射序封顶。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query("inSert into login_common(m_id,member_bonus_fatigue,update_time) values(%s,%d,unix_timestamp(now()))",
                      NumberToString(accId, 0), memberBonusFatigue);
        // ORIG +0xe0-0xe3 `xor $1,%eax; test %al,%al`（dead test 直落 epilogue，
        // 0x08418522-0x08418525）⇒ P22/P168a③ `!exec` 直返（^true 多 jmp+mov$0）。
        return !db->exec(true);
    }
    return false;
