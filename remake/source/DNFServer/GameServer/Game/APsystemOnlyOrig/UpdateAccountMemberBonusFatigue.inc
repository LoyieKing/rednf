// ORIG 0x8418442：login_common 表按 m_id 更新会员奖励疲劳；无行受影响时
// 回退 inSert 新行（update_time=now()），返 !exec(inSert)；已更新路径返 0
//（ORIG +0x97 test %al/je +0xe5 汇合，eax=v=false 直落出口）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08418442
// [b28d-solo-2R] DIFF(6)：`!db->exec(true)` / `exec()==false` 两形 4.4 均产
// `xor$1; jmp; mov$0` 岛（+1 条），ORIG 4.1 为 `xor$1; test %al,%al` dead-test 直落
// epilogue ⇒ N122/跨版本 bool 返回封顶（P168a③）。ORIG 0x8418442
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
    // [r3 负结论] 尾部 `return false;` 改显式 `else { return false; }` 包裹（P222）
    // 实测**零变化**（62/63、ndiff 3、ORIG-only{test:1}/OURS-only{jmp:1,mov:1} 全同）
    // ⇒ 回滚，维持 N93附 封顶。
    // [P5S2W8LT r2 负结论·已回滚] v1 正极性正文形实测**劣化**：blocks 1→4
    // （新增两处 `jne/je` 镜像 + 尾块仍 `jmp/mov$0`）、ours 63→62 但差异块
    // 反增 ⇒ 维持原负极性形，N93附 封顶（ORIG 0x084184d9 `test %al,%al;
    // je +0xe5` 的 ver 值由 4.1 choose 汇聚，4.4 不可复现）。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query("inSert into login_common(m_id,member_bonus_fatigue,update_time) values(%s,%d,unix_timestamp(now()))",
                      NumberToString(accId, 0), memberBonusFatigue);
        // ORIG +0xe0-0xe3 `xor $1,%eax; test %al,%al`（dead test 直落 epilogue，
        // 0x08418522-0x08418525）⇒ P22/P168a③ `!exec` 直返（^true 多 jmp+mov$0）。
        return !db->exec(true);
    }
    return false;
}