// UseSelectBooster(unsigned int,int,int,std::vector<Inven_Item>&)：ORIG 0x8686b52。
// 把 items 中每个元素 +0x02（Inven_Item::m_addInfo）拼成 "%d," 串（上限 0x400 字节缓冲，
// 每项至少占 0xd 字节），经 CUser::LogHistory 记录 "UseSelectBooster,%u,%d,%d,%s"。
// b28d-65：while(i<size)+体内 `if(strlen+0xd>0x3fe) break` + sprintf 目标内联（去 p 槽）
//   ⇒ 与 ORIG 0x08686b52 逐条全等（60=60）。
void cUserHistoryLog::UseSelectBooster(unsigned int a, int b, int c,
                                       std::vector<Inven_Item>& items)
{
    char buffer[0x400];
    memset(buffer, 0, sizeof(buffer));
    unsigned int i = 0;
    while (i < items.size())
    {
        if (strlen(buffer) + 0xd > 0x3fe)
            break;
        sprintf(buffer + strlen(buffer), "%d,", items[i].m_addInfo);
        ++i;
    }
    m_user->LogHistory("UseSelectBooster,%u,%d,%d,%s", a, b, c, buffer);
}