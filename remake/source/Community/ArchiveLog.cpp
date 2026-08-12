//
// Created by Loyie King on 2024/3/10.
//



#include "ArchiveLog.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"

#include <cstdarg>
#include <cstring>
#include <ctime>

// 原始：文件级 static const LINE_SIZE = 0x1000（_ZL9LINE_SIZE），ArchiveLog 缓冲大小
static const int LINE_SIZE = 0x1000;

ArchiveLog::ArchiveLog(const char *log, ...) {
    char buffer[LINE_SIZE];
    static CArchiveLog archive_log;
    static bool bArchiveLog;

    if (!bArchiveLog) {
        archive_log.CreateLogFile("log/Log");
        bArchiveLog = true;
    }

    memset(buffer, 0, 4096);

    va_list args;
    va_start(args, log);
    vsprintf(buffer, log, args);
    va_end(args);

    archive_log.WriteLog(buffer);
}

CArchiveLog::CArchiveLog() : logFile(NULL), time() {
}

int CArchiveLog::CloseFile() {
    if (logFile != NULL) {
        fclose(logFile);
        logFile = NULL;
    }
    return 1;
}

bool CArchiveLog::OpenFile(const char *file) {
    // 原始：非空提前返回 0；fopen 赋值在条件内（sete al; test al,al; je 形态）
    if (logFile != NULL) {
        return 0;
    }
    if ((logFile = fopen(file, "ab")) == NULL) {
        return 0;
    }
    return 1;
}

bool CArchiveLog::CreateLogFile(DateTime &date) {
    char buffer[1024] = {0};  // 原始：rep stosl 内联清 0
    sprintf((char *)buffer, "%s%04d%02d%02d.log", logPrefix, (int)date.year, (int)date.month, (int)date.day);
    return OpenFile((char *)buffer);
}

bool CArchiveLog::CreateLogFile(const char *prefix) {
    DateTime time;
    this->logPrefix = prefix;
    return CreateLogFile(time);
}

int CArchiveLog::WriteLog(const char *log) {
    if (logFile == NULL) {
        return 0;
    }

    DateTime now;
    if (now.DiffDay(this->time)) {
        // not today
        CloseFile();
        if (CreateLogFile(now) == false) {
            return 0;
        }
    }
    // 与 ORIG 完全一致的形态（2026-08-12 实测）：DateTime 有用户声明构造，
    // GCC 4.4 为隐式拷贝赋值生成自赋值守卫（cmp/je）+ memcpy(...,7)，
    // 与 ORIG 的寄存器复用块逐条一致；显式 if+memcpy 反而会重算地址。
    this->time = now;
    fprintf(this->logFile, "[%02d:%02d:%02d] %s\r\n", (int)now.hour, (int)now.min, (int)now.sec, log);
    printf("[%02d:%02d:%02d] %s\r\n", (int)now.hour, (int)now.min, (int)now.sec, log);
    fflush(this->logFile);

    return 1;
}

CArchiveLog::~CArchiveLog() {
    CloseFile();
}

// ===== DateTime（原始位于 ArchiveLog.cpp TU，紧邻 ArchiveLog ctor）=====

DateTime::DateTime() {
    tm *pt;
    tm time;
    time_t time_size;
    time_size = ::time(NULL);
    pt = localtime_r(&time_size, &time);
    this->year = (short)(pt->tm_year + 1900);
    this->month = pt->tm_mon + 1;
    this->day = pt->tm_mday;
    this->hour = pt->tm_hour;
    this->min = pt->tm_min;
    this->sec = pt->tm_sec;
}

bool DateTime::DiffDay(DateTime &another) {
    return this->day != another.day;
}
