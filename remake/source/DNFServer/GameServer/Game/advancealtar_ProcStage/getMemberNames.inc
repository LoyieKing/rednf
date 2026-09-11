void getMemberNames(CUser* user, char* buf)
{
    if (user == 0)
    {
        return;
    }
    if (buf == 0)
    {
        return;
    }
    sprintf(buf, "\"%s\",\"%s\",\"N/A\",\"N/A\",\"N/A\"",
            user->getCurCharacName(), user->getCurCharacName());
}