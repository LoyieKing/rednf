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
                                      ((LetterOccView*)&vecNew[(size_t)start])->m_occ);
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
