void CFileLogWriter::writeRawLog(char const* fileName, char const* logMsg, ...) {
    CGuard<CMutex> auto1(&mutex);
    time_t currTime = time(NULL);
    tm currtm;
    tm* result = localtime_r(&currTime, &currtm);
    va_list args;
    va_start(args, logMsg);
    tm* result2;

    char newTempFileName[256];
    sprintf(newTempFileName, "%s%04d%02d%02d.log", fileName, result->tm_year + 1900,
            result->tm_mon + 1, result->tm_mday);
    std::string newFileName(newTempFileName);
    std::map<std::string, CFLog*>::iterator itr = logs.find(newFileName);
    if (itr != logs.end()) {
        CFLog* log = itr->second;
        log->writeLog(logMsg, (char*)args);
    } else {
        time_t prevtime = currTime - 86400;
        tm prevtm;
        result2 = localtime_r(&prevtime, &prevtm);
        char prevTempFileName[256];
        sprintf(prevTempFileName, "%s%04d%02d%02d.log", fileName, result2->tm_year + 1900,
                result2->tm_mon + 1, result2->tm_mday);
        std::string prevFileName(prevTempFileName);
        std::map<std::string, CFLog*>::iterator prev_itr = logs.find(prevFileName);
        if (prev_itr != logs.end()) {
            delete prev_itr->second;
            logs.erase(prev_itr);
        }

        CFLog* newLog = new CFLog(newFileName);
        if (logs.insert(std::make_pair(newFileName, newLog)).second) {
            newLog->writeLog(logMsg, (char*)args);
        }
    }
