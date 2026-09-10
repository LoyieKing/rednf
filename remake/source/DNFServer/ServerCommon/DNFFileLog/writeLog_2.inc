void CFileLogWriter::writeLog(char const* fileName, char const* logMsg, ...) {
    CGuard<CMutex> auto1(&mutex);
    time_t currTime = time(NULL);
    tm currtm;
    // 原始：使用 localtime_r 返回的 tm*（非直接取 buffer）
    tm* result = localtime_r(&currTime, &currtm);
    // 原始：va_list 在 sprintf 之前建立
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
        log->writeLog(result, logMsg, (char*)args);
    } else {
        // 首次写今日日志：关闭并移除昨日句柄，再新建今日句柄
        time_t prevtime = currTime - 86400;
        tm prevtm;
        result2 = localtime_r(&prevtime, &prevtm);
        char prevTempFileName[256];
        sprintf(prevTempFileName, "%s%04d%02d%02d.log", fileName, result2->tm_year + 1900,
                result2->tm_mon + 1, result2->tm_mday);
        // 原始：昨日 key 为具名 string，生命周期延至函数末（非临时 std::string）
        std::string prevFileName(prevTempFileName);
        std::map<std::string, CFLog*>::iterator prev_itr = logs.find(prevFileName);
        if (prev_itr != logs.end()) {
            // 原始：delete 直接用迭代器 second（ptEv 一次，delete 复用 eax）
            delete prev_itr->second;
            logs.erase(prev_itr);
        }

        CFLog* newLog = new CFLog(newFileName);
        // 原始：insert(make_pair(...)) 并检查返回值，插入成功才写日志
        if (logs.insert(std::make_pair(newFileName, newLog)).second) {
            newLog->writeLog(result, logMsg, (char*)args);
        }
    }
