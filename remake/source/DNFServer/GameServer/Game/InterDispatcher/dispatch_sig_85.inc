// ORIG 0x084d0648 T dispatch_sig —— 校验码区间比对：对所有登录用户执行校验并计数作弊。
// 报告 docs/class_func_reports/Inter_CodeChecksumRange/dispatch_sig.md 全文转写。
// [b28d-PoolInterDisp] 收口：getUserByAccIdMap 发明 getter 已删（ORIG 无该符号）；
// 本类经 CGameManager.h `friend class Inter_CodeChecksumRange` 直访 m_userByAccId，
// 复现 ORIG 0x084d069c `call G_CGameManager; lea 0x22c(%eax)` 无 call 形。
int Inter_CodeChecksumRange::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    CodeHackCheckRangeEx range;
    CodeHackChecksumEx checksum;

    const char* next = range.setData(data);
    checksum.setData(next);

    // ORIG 0x084d069c：`call G_CGameManager; lea 0x22c(%eax),%edx`（无具名槽）。
    for (std::map<unsigned int, CUser*>::iterator it =
             G_CGameManager()->m_userByAccId.begin();
         it != G_CGameManager()->m_userByAccId.end(); ++it)
    {
        CUser* target = it->second;                    // pair second
        if (!target->checkCodeChecksumRange(checksum, range))   // xor $1 取反
        {
            if (target->get_state() > 1)               // cmp $1,setg
            {
                // ORIG 0x084d0715 `mov %eax,-0xc`：结果落槽但从未使用（技巧8 死槽）。
                int answerCnt = target->GetChecksumAnswerCount();
                (void)answerCnt;
                target->getHackAnalyzer()->addServerHackCnt(
                    target, (WongWork::ENUM_HACKTYPE)0x3b7, 1, 0, 0);
            }
        }
    }
    return 0;
}