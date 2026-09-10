void FunctionGuildMember::LogMakeGuildMembersInfo(unsigned int guildKey,
                                                  int count,
                                                  ST_Guild_Mem_Info* members)
{
    char buf[0x1000] = { '\0' };   // P4：O0 内联 rep stos，与 ORIG 同形
    sprintf(buf, "gKey(%d) Cnt(%d)", guildKey, count);
    while (count-- != 0)
    {
        sprintf(buf, "%s %s", buf, members->f.m_name);
        sprintf(buf, "%s l(%d)", buf, (int)members->f.m_level);
        sprintf(buf, "%s j(%d)", buf, (int)members->f.m_job);
        sprintf(buf, "%s g(%d)", buf, (int)members->f.m_growthType);
        sprintf(buf, "%s Ch(%d)", buf, (int)(unsigned char)members->f.m_channel);
        sprintf(buf, "%s S(%d)", buf, (int)members->f.m_sex);
        sprintf(buf, "%s B(%d)", buf, (int)members->f.m_blackUser);
        sprintf(buf, "%s MG(%d)", buf, (int)(unsigned char)members->f.m_grade);
        sprintf(buf, "%s\n", buf);
        if (strlen(buf) > 0xfff)
            break;
        members = (members + 1);
    }
    char path[0x100] = { '\0' };   // P4：O0 内联 rep stos，与 ORIG 同形
    sprintf(path, "./log/%s/GuildModi", G_CEnvironment()->get_file_name());
    CMyFileLog log(
        __PRETTY_FUNCTION__,
        0x24ac);
    log(path, "%s", buf);
