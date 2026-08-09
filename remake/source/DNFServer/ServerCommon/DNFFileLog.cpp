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

void CFileLogWriter::writeLog(char const* fileName, char const* logMsg, ...) {
    CGuard<CMutex> auto1(&mutex);
    time_t currTime = time(NULL);
    tm currtm;
    // 原始：使用 localtime_r 返回的 tm*（非直接取 buffer）
    tm* result = localtime_r(&currTime, &currtm);
    // 原始：va_list 在 sprintf 之前建立
    va_list args;
    va_start(args, logMsg);

    char newTempFileName[256];
    sprintf(newTempFileName, "%s%04d%02d%02d.log", fileName, result->tm_year + 1900,
            result->tm_mon + 1, result->tm_mday);
    std::string newFileName(newTempFileName);
    std::map<std::string, CFLog*>::iterator itr = logs.find(newFileName);
    if (itr != logs.end()) {
        CFLog* log = itr->second;
        log->writeLog(result, logMsg, (char*)args);
        va_end(args);
        return;
    }

    // 首次写今日日志：关闭并移除昨日句柄，再新建今日句柄
    time_t prevtime = currTime - 86400;
    tm prevtm;
    tm* result2 = localtime_r(&prevtime, &prevtm);
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
    va_end(args);
}

void CFileLogWriter::writeRawLog(char const* fileName, char const* logMsg, ...) {
    CGuard<CMutex> auto1(&mutex);
    time_t currTime = time(NULL);
    tm currtm;
    tm* result = localtime_r(&currTime, &currtm);
    va_list args;
    va_start(args, logMsg);

    char newTempFileName[256];
    sprintf(newTempFileName, "%s%04d%02d%02d.log", fileName, result->tm_year + 1900,
            result->tm_mon + 1, result->tm_mday);
    std::string newFileName(newTempFileName);
    std::map<std::string, CFLog*>::iterator itr = logs.find(newFileName);
    if (itr != logs.end()) {
        CFLog* log = itr->second;
        log->writeLog(logMsg, (char*)args);
        va_end(args);
        return;
    }

    time_t prevtime = currTime - 86400;
    tm prevtm;
    tm* result2 = localtime_r(&prevtime, &prevtm);
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
    va_end(args);
}

// ---- CFileLogWriterInstance ----


// ---- CMyFileLog ----

CMyFileLog::CMyFileLog(char const* name, int level) : name(name), level(level) {
}

void CMyFileLog::operator()(char const* filename, char const* format, ...) {
    char buff[0x7000];
    // 原始：size 经寄存器局部现装（ORIG 为 mov $0x7000,%edx；本工具链 register size_t 落 ebx，
    // 为不可复现的寄存器分配伪影，指令条数与结构一致）
    {
        register size_t n = 0x7000;
        memset(buff, 0, n);
    }
    sprintf(buff, "%s(%d): ", name, level);
    va_list args;
    va_start(args, format);
    // 原始：strlen 内联于 vsprintf 参数（不存局部变量）
    vsprintf(buff + strlen(buff), format, args);
    va_end(args);
    // 原始：Instance() 结果直接作为实参
    CFileLogWriterInstance()->writeLog(filename, buff);  // 原始：缓冲区直接作为格式串传递
}

// ---- CMyRawFileLog ----

void CMyRawFileLog::operator()(char const* filename, char const* format, ...) {
    char buff[0x7000];
    // ORIG 是 mov $0x7000,%edx 直装（无 push ebx）；本工具链 register 局部必落 ebx 并强制保存
    // （30 条）。字面量折叠为 movl $0x7000,0x8(%esp)（26 条），仅差装载形态，无额外保存。
    memset(buff, 0, 0x7000);
    va_list args;
    va_start(args, format);
    vsprintf(buff, format, args);
    va_end(args);
    // 原始：Instance() 结果直接作为实参（不存局部变量）
    CFileLogWriterInstance()->writeRawLog(filename, buff);
}

// ---- CToolFileLog ----

CToolFileLog::CToolFileLog(char const* name, int level) : name(name), level(level) {
}

// DWARF 参数名：CToolFileLog::operator()(this, filename, no, format) —— no 为 int
void CToolFileLog::operator()(char const* filename, int no, char const* format, ...) {
    char buff[0x7000];
    char argsbuff[0x7000];
    // 原始：先 memset line/message（ORIG 每处现装 %edx；此处两个独立 register 局部以匹配
    // 两次现装的指令数，寄存器名差异为工具链伪影），再 subfile 聚合初始化（rep stosl）
    {
        register size_t n1 = 0x7000;
        memset(buff, 0, n1);
    }
    {
        register size_t n2 = 0x7000;
        memset(argsbuff, 0, n2);
    }
    char fbuff[256] = {0};
    sprintf(buff, "%s(%d): ", name, level);
    va_list args;
    va_start(args, format);
    vsprintf(argsbuff, format, args);
    va_end(args);
    // 原始：strlen 内联于 strcat 参数
    strcat(buff + strlen(buff), argsbuff);
    // 原始：no != -1 分支在前（fall-through 写 subfile），no==-1 用 je 跳到后块
    // → if (no != -1) { sprintf+write(subfile) } else { write(file) }
    if (no != -1) {
        sprintf(fbuff, "%s_%02d_", filename, no);
        CFileLogWriterInstance()->writeLog(fbuff, buff);
    } else {
        CFileLogWriterInstance()->writeLog(filename, buff);
    }
}
