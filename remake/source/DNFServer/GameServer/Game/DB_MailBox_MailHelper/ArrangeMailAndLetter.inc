// ============================================================================
// ArrangeMailAndLetter —— ORIG 0x0841d2ef：vecNew 按 CompareLetter 排序后，
//   从 size-count 处起逐项：按 +0x125 找 postal 区更新的邮件
//   （FindLastOccTimeMail）并删除（RemoveMailByLatestLetter），同时从 vecOld
//   删同 letter_id(+0x5c) 项。恒返 1。
// ============================================================================
// [sweep] DIFF：ORIG 0x0841d2ef；p167 调用点差集 CLEAR（14=14）。
//   残差 3 处均 4.1/4.4 codegen 固有差：①序言 `mov count,%edx; sub %edx,%eax`
//   vs 4.4 折 `sub mem,%eax`；②`cmpl $0xffffffff,slot; setne; test; je`
//   （4.1 槽比物化，P197④）vs 4.4 直 `je`；③外层尾测 ORIG mem 直比 vs 4.4
//   载 %edx 比较（P209）。109 vs 111。
// [F2d-C05② 甄别] 无命中形：gap=+2（show_func_diff orig=111 our=109，我们**少**）⇒ F2
//   减指令方向相反；且③的比较对象含跨调用返回值 size()（非两具名局部）。本轮实测
//   ORIG-only={setne,test}（=②物化段）⇒ 残余归 P197④/N18b。
int ArrangeMailAndLetter(SIG_MAILBOX_LIST* list,
                         std::vector<SIG_LETTER_INFO>& vecNew, int count,
                         std::vector<SIG_LETTER_INFO>& vecOld)
{
    std::sort(vecNew.begin(), vecNew.end(), CompareLetter);
    int start = (int)vecNew.size() - count;
    while ((unsigned int)vecNew.size() > (unsigned int)start)
    {
        int mailIdx = -1;
        mailIdx = FindLastOccTimeMail(list,
                                      vecNew[(size_t)start].m_period);
        if (mailIdx != -1)
        {
            int letterIdx =
                (int)list->m_box.m_postals[mailIdx].m_tail.m_occ.m_letterId;
            if (RemoveMailByLatestLetter(list, mailIdx))
            {
                if (letterIdx != 0)
                {
                    std::vector<SIG_LETTER_INFO>::iterator it = vecOld.begin();
                    std::vector<SIG_LETTER_INFO>::iterator itEnd = vecOld.end();
                    for (; it != itEnd; ++it)
                    {
                        if (*(const int*)&*it == letterIdx)
                        {
                            vecOld.erase(it);
                            break;
                        }
                    }
                }
            }
        }
        ++start;
    }
    return 1;
}