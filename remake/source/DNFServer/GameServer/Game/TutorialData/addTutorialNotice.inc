// ORIG _ZN12TutorialData17addTutorialNoticeEiiiPKc @0x08ab8a86：
// TutorialNotice{+0:p1,+4:p4 文本,+8:p2,+c:p3}；make_pair(p1, vec) 插入 m_notices。
void TutorialData::addTutorialNotice(int key, int field8, int fieldc,
                                     char const* path)
{
    NoticeMap::iterator it = m_notices.find(key);

    TutorialNotice* notice = new TutorialNotice;
    notice->m_field0 = key;
    notice->m_aniPath = path;
    notice->m_field8 = field8;
    notice->m_fieldc = fieldc;

    if (it != m_notices.end()) {
        it->second.push_back(notice);
        return;
    }

    std::vector<TutorialNotice*> entries;
    entries.push_back(notice);
    m_notices.insert(std::make_pair(key, entries));
}