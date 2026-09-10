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
