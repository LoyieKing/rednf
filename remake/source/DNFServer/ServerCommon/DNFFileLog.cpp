#include "DNFFileLog.h"


#include <iostream>
#include "DNFFunctionLib.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

CFileLogWriter* CFileLogWriterInstance() {
    static CFileLogWriter instance;
    return &instance;
}


// ---- CFLog ----

CFLog::CFLog(std::string& filename) : name() {
    // 原始：无 file 初始化（fopen 直接赋值）；c_str() 作用于参数而非成员
    name = filename;
    // 原始：fopen 赋值在条件内（mov [edx+4],eax; test; sete al; test al,al; je 形态）
    if ((file = fopen(filename.c_str(), "ab")) == NULL) {
        file = stdout;
    }
}

CFLog::~CFLog() {
    fclose(file);  // 原始：无条件 fclose（与原始一致）
}

void CFLog::writeLog(tm* t, char const* fmt, char* va) {
    if (file == NULL) {
        return;
    }
    char message[0x7000];
    vsprintf(message, fmt, va);
    char line[0x7000];
    sprintf(line, "[%02d:%02d:%02d] %s", t->tm_hour, t->tm_min, t->tm_sec, message);
    fprintf(file, "%s\r\n", line);
    fflush(file);
}

void CFLog::writeLog(char const* fmt, char* va) {
    if (file == NULL) {
        return;
    }
    char message[0x7000];
    vsprintf(message, fmt, va);
    fprintf(file, "%s\r\n", message);
    fflush(file);
}

// ---- CFileLogWriter（构造/析构已移至 CFileLogWriterBase.cpp，-fno-exceptions） ----

void CFileLogWriter::writeLog(char const* file, char const* fmt, ...) {
    CGuard<CMutex> guard(&mutex);
    time_t now = time(NULL);
    tm nowTm;
    localtime_r(&now, &nowTm);

    char filename[256];
    sprintf(filename, "%s%04d%02d%02d.log", file, nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday);
    std::string key(filename);
    std::map<std::string, CFLog*>::iterator it = logs.find(key);
    if (it != logs.end()) {
        va_list ap;
        va_start(ap, fmt);
        it->second->writeLog(&nowTm, fmt, (char*)ap);
        va_end(ap);
        return;
    }

    // 首次写今日日志：关闭并移除昨日句柄，再新建今日句柄
    time_t yesterday = now - 86400;
    tm yTm;
    localtime_r(&yesterday, &yTm);
    char yfile[256];
    sprintf(yfile, "%s%04d%02d%02d.log", file, yTm.tm_year + 1900, yTm.tm_mon + 1, yTm.tm_mday);
    std::map<std::string, CFLog*>::iterator yit = logs.find(std::string(yfile));
    if (yit != logs.end()) {
        // 原始：CFLog* 临时指针（ptEv 一次，delete 复用 eax）
        CFLog* oldLog = yit->second;
        delete oldLog;  // delete 自带 NULL 检查（原始单次 test eax,eax）
        logs.erase(yit);
    }

    CFLog* log = new CFLog(key);
    // 原始：insert(make_pair(...)) 并检查返回值，插入成功才写日志
    std::pair<std::map<std::string, CFLog*>::iterator, bool> inserted = logs.insert(std::make_pair(key, log));
    va_list ap;
    va_start(ap, fmt);
    if (inserted.second) {
        log->writeLog(&nowTm, fmt, (char*)ap);
    }
    va_end(ap);
}

void CFileLogWriter::writeRawLog(char const* file, char const* fmt, ...) {
    CGuard<CMutex> guard(&mutex);
    time_t now = time(NULL);
    tm nowTm;
    localtime_r(&now, &nowTm);

    char filename[256];
    sprintf(filename, "%s%04d%02d%02d.log", file, nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday);
    std::string key(filename);
    std::map<std::string, CFLog*>::iterator it = logs.find(key);
    if (it != logs.end()) {
        va_list ap;
        va_start(ap, fmt);
        it->second->writeLog(fmt, (char*)ap);
        va_end(ap);
        return;
    }

    time_t yesterday = now - 86400;
    tm yTm;
    localtime_r(&yesterday, &yTm);
    char yfile[256];
    sprintf(yfile, "%s%04d%02d%02d.log", file, yTm.tm_year + 1900, yTm.tm_mon + 1, yTm.tm_mday);
    std::map<std::string, CFLog*>::iterator yit = logs.find(std::string(yfile));
    if (yit != logs.end()) {
        // 原始：CFLog* 临时指针（ptEv 一次，delete 复用 eax）
        CFLog* oldLog = yit->second;
        delete oldLog;  // delete 自带 NULL 检查（原始单次 test eax,eax）
        logs.erase(yit);
    }

    CFLog* log = new CFLog(key);
    // 原始：insert(make_pair(...)) 并检查返回值，插入成功才写日志
    std::pair<std::map<std::string, CFLog*>::iterator, bool> inserted = logs.insert(std::make_pair(key, log));
    va_list ap;
    va_start(ap, fmt);
    if (inserted.second) {
        log->writeLog(fmt, (char*)ap);
    }
    va_end(ap);
}

// ---- CFileLogWriterInstance ----


// ---- CMyFileLog ----

CMyFileLog::CMyFileLog(char const* name, int level) : name(name), level(level) {
}

void CMyFileLog::operator()(char const* file, char const* fmt, ...) {
    char buffer[0x7000];
    memset(buffer, 0, 0x7000);
    sprintf(buffer, "%s(%d): ", name, level);
    va_list ap;
    va_start(ap, fmt);
    // 原始：strlen 内联于 vsprintf 参数（不存局部变量）
    vsprintf(buffer + strlen(buffer), fmt, ap);
    va_end(ap);
    // 原始：Instance() 结果直接作为实参
    CFileLogWriterInstance()->writeLog(file, buffer);  // 原始：缓冲区直接作为格式串传递
}

// ---- CMyRawFileLog ----

void CMyRawFileLog::operator()(char const* file, char const* fmt, ...) {
    char buffer[0x7000];
    memset(buffer, 0, 0x7000);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);
    // 原始：Instance() 结果直接作为实参（不存局部变量）
    CFileLogWriterInstance()->writeRawLog(file, buffer);
}

// ---- CToolFileLog ----

CToolFileLog::CToolFileLog(char const* name, int level) : name(name), level(level) {
}

void CToolFileLog::operator()(char const* file, int seq, char const* fmt, ...) {
    char line[0x7000];
    char message[0x7000];
    // 原始：subfile 聚合初始化在 memset 之后、sprintf(line) 之前（rep stosl 内联清 0）
    char subfile[256] = {0};
    memset(line, 0, 0x7000);
    memset(message, 0, 0x7000);
    sprintf(line, "%s(%d): ", name, level);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(message, fmt, ap);
    va_end(ap);
    // 原始：strlen 内联于 strcat 参数
    strcat(line + strlen(line), message);
    if (seq == -1) {
        CFileLogWriterInstance()->writeLog(file, line);  // 原始：line 直接作为格式串
    } else {
        sprintf(subfile, "%s_%02d_", file, seq);
        CFileLogWriterInstance()->writeLog(subfile, line);
    }
}
