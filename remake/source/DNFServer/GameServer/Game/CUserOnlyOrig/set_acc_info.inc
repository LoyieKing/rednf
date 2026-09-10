void CUser::set_acc_info(unsigned int accId, const char* accName)
{
    // set_acc_info.md @0x8649bfc：m_accId(+0x704ac)=accId；
    // strncpy(m_accName(+0x704b0), name, 0x14)；min(strlen,0x14) 处补 '\0'。
    m_accId = accId;
    // ORIG 0x08649c0e-0x08649c37：`movl $0x14,-0x14`（先于 strlen 写）+ len 占更近槽
    // -0x10、std::min<int> call + `mov (%eax),%eax` 解引用落 -0xc、strncpy 在 min 之后。
    // 4.4 槽按声明序（len 先声明占近槽）而写值序按语句位置（技巧13），故分离声明。
    int len;
    int maxLen = 0x14;
    len = strlen(accName);
    int term = std::min<int>(len, maxLen);
    strncpy(m_accName, accName, 0x14);
    m_accName[term] = '\0';
