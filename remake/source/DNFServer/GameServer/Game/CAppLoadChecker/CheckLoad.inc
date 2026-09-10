bool CAppLoadChecker::CheckLoad(int dbq, int logdbq, int timerq, int packetq)
{
    bool dbqOver = checkLoad_dbq(dbq);
    bool logdbqOver = checkLoad_logdbq(logdbq);
    bool timerqOver = checkLoad_timerq(timerq);
    bool packetqOver = checkLoad_packetq(packetq);

    if (dbqOver || logdbqOver || timerqOver || packetqOver)
    {
        setQueue(dbq, logdbq, timerq, packetq);
        return true;
    }
    return false;
