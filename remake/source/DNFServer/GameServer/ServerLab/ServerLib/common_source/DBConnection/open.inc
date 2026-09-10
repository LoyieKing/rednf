bool DBConnection::open(char* ip, char* dbname, char* id, char* pass, unsigned int port)
{
    strncpy(dbIp_, ip, 0x10);
    dbPort_ = port;
    strncpy(dbAcc_, id, 0x14);
    strncpy(dbPwd_, pass, 0x14);
    strncpy(dbName_, dbname, 0x1e);
    if (mysql_real_connect(h_db, ip, id, pass, dbname, port, 0, 0) == 0)
    {
        // ORIG: err 与 G_TraceLog() 均为内联临时（err 保存在 ebx 跨调用）。
        G_TraceLog()->errorLog("%s %u %s %s %s error=\'%d\' FAILED", ip, port, dbname, id, pass,
                               mysql_errno(h_db));
        printf("%s %u %s %s %s error=\'%d\' FAILED\n", ip, port, dbname, id, pass,
               mysql_errno(h_db));
        return false;
    }
    printf("%s %s SUCCESS\n", ip, dbname);
    return true;
