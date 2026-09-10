CExceptLog::CExceptLog(std::string& str)
{
    memset((char*)this, 0, 2000);

    char buf[128];
    Set_Time_String(buf);
    fPrintTextFile("Except.txt", "%s\n%s\n", buf, str.c_str());
