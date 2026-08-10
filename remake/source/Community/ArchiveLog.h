//
// Created by Loyie King on 2024/3/10.
//
#pragma once

#include <cstdio>

#include "ReverseEngineerLib.h"


// 原始：DateTime 位于 ArchiveLog.cpp TU（_GLOBAL__I 桩以 ArchiveLog ctor 命名；
// DateTime ctor/DiffDay 与 CArchiveLog 同 TU，地址紧邻 ArchiveLog ctor 之后）。
class DateTime {
public:
    short year;
    char month;
    char day;
    char hour;
    char min;
    char sec;

    DateTime();

    bool DiffDay(DateTime &another);
};

TEST_CLASS_SIZE(DateTime, 8);

class ArchiveLog {
public:
    ArchiveLog(const char *log, ...);
};

class CArchiveLog {
    FILE *logFile;
    DateTime time;
    const char *logPrefix;

private:
    int CloseFile();

    bool OpenFile(const char *file);

public:
    CArchiveLog();

    bool CreateLogFile(DateTime &date);  // 原始：bool 返回（WriteLog 调用处 xor 惯用法）

    bool CreateLogFile(const char *prefix);

    int WriteLog(const char *log);

    ~CArchiveLog();
};
