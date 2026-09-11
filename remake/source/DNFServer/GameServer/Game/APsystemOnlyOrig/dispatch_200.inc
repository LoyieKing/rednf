// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 084317b2
// [F2e-Singles2 C05② 2026-09-10] 旋钮①删 `(unsigned char)i` 收窄（我方 8 位
//   `cmp %al,%dl; seta` vs ORIG 32 位 `movzbl; movzbl; cmp -N(%ebp),%eax; setg`）
//   ⇒ 翻正为 ORIG 逐条形；旋钮②mId/schoolId 具名局部内联（P0，实参直传）⇒
//   实测 our 78→76（ORIG 74，gap **−4→−2**），TU 四数 141/1743/9/67 两态逐位一致。
//   残余 = 我方多 sig/v 缓存槽 + 帧 `sub $0x34` vs ORIG `$0x20` + 缺 push %esi（P210/N82）。
bool DB_UpdatePvPLastPlayTime::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10,
                                                  E_SERVER_GROUP_NONE);
    // [AttackC 2026-09-11 P340 负结论] sig 改 `unsigned int sig = (unsigned
    //   int)stream->GetOutBuffer<...>()` 整型承载 ⇒ 编译产物**逐位不变**
    //   （still 74/75 ndiff 4，ours 仍 `mov -0x14(%ebp),%ecx` 先于 j/i 两条）
    //   ⇒ 本件槽/装载序非 4.4 指针分区所致（4 个槽位本来与 ORIG 全等），
    //   纯 4.1「索引先、base 后」vs 4.4「base 先」装载序 ⇒ N7/N82 封顶。
    Packet_PvPResult* sig = stream->GetOutBuffer<Packet_PvPResult>();
    // [L5 K5W8LTDBTask] P0：去 `v` 具名缓存（ORIG 0x084317b2 段无第二槽往返，
    // 全程 `((DB_PvPResultSchoolView*)sig)->...` 就地 cast 直读）。
    for (int i = 0; (unsigned char)((DB_PvPResultSchoolView*)sig)->m_count > i; ++i)
    {
        for (int j = 0; j <= 1; ++j)
        {
            db->set_query(
                "upDate school_member set last_play_time=NOW()  where m_id=%s and school_id=%d",
                NumberToString(((DB_PvPResultMIdView*)sig)->m_mId[j][i], 0),
                ((DB_PvPResultSchoolView*)sig)->m_schoolId[j][i]);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}