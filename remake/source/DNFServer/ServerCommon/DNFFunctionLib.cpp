#include "DNFFunctionLib.h"


#include <iostream>

#include <dirent.h>
#include <execinfo.h>
#include <iconv.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "DNFFileLog.h"






void Char2Hex(unsigned char c, char* out) {
    static const char saucHex[] = "0123456789abcdef";
    out[0] = saucHex[c >> 4];
    out[1] = saucHex[c & 0xf];
    out[2] = '\0';
}


// ---- 字符集全局（原始为 file-scope static std::string，_ZL 本地符号） ----
#ifndef DF_NO_CODEPAGE
static std::string gClientEncoding;
static std::string gScriptEncoding;
static std::string gDatabaseEncoding;
#endif

// ---- 自由函数 ----

// 原始为 TLS 缓冲区 gNumberToStringBuffer（符号 0x200 字节，GS-0x208 + index*0x40，8 槽）
static __thread char gNumberToStringBuffer[0x200];

char* NumberToString(unsigned int value, int index) {
    sprintf(&gNumberToStringBuffer[index * 0x40], "%u", value);
    return &gNumberToStringBuffer[index * 0x40];
}

char* NumberToString(unsigned long long value, int index) {
    sprintf(&gNumberToStringBuffer[index * 0x40], "%qu", value);
    return &gNumberToStringBuffer[index * 0x40];
}

void PrintBackTrace() {
    void* frames[100];
    int count = backtrace(frames, 100);
    char** symbols = backtrace_symbols(frames, count);
    if (symbols != NULL) {
        for (int i = 0; i < count; i++) {
            // 原始：先取 symbols[i] 到局部变量（触发 ebx 保存，ctor 调用前取值）
            char* s = symbols[i];
            CMyFileLog log("PrintBackTrace", 0x1d1);
            log("./log/BackTrace", s);
        }
        free(symbols);
    }
}

unsigned long StringToNumber(char const* str) {
    if (str == NULL) {
        return 0;
    }
    return strtoul(str, NULL, 10);
}

char checkTimeToday(long t, int hour) {
    long yesterday = t - 86400;
    tm now;
    tm yday;
    tm* nowP = localtime_r(&t, &now);
    tm* ydayP = localtime_r(&yesterday, &yday);
    (void)ydayP;
    int localHour = hour;
    char result = 0;
    if (nowP->tm_hour >= 0 && nowP->tm_hour < localHour) {
        result = 0;
    } else {
        result = 1;
    }
    return result;
}

void GetCurrentResetBaseTime(long t, int hour) {
    long yesterday = t - 86400;
    tm now;
    tm yday;
    tm* nowP = localtime_r(&t, &now);
    tm* ydayP = localtime_r(&yesterday, &yday);
    // 原始：int localHour = 0 后再赋 hour（mov [ebp-X],0 + mov [ebp-X],hour）
    int localHour = 0;
    localHour = hour;
    // 原始：条件为 nowP->tm_hour >= 0 && nowP->tm_hour < localHour 时改 ydayP，否则改 nowP
    if (nowP->tm_hour >= 0 && nowP->tm_hour < localHour) {
        ydayP->tm_hour = localHour;
        ydayP->tm_min = 0;
        ydayP->tm_sec = 0;
        mktime(ydayP);
    } else {
        nowP->tm_hour = localHour;
        nowP->tm_min = 0;
        nowP->tm_sec = 0;
        mktime(nowP);
    }
}

// 反编译：saucHex = "0123456789abcdef"

bool Hex2Char(char const* hex, unsigned char& out) {
    // 原始：数字分支在前（cmp 0x2f/0x39），hex 分支其次（cmp 0x60/0x66），失败最后
    if (hex[0] > '/' && hex[0] <= '9') {
        out = hex[0] - '0';
    } else if (hex[0] > '`' && hex[0] <= 'f') {
        out = hex[0] - 'a' + 10;
    } else {
        return 0;
    }
    // 原始：指针递增 hex++（add DWORD PTR [ebp+0x8],0x1）
    hex++;
    if (hex[0] > '/' && hex[0] <= '9') {
        // 原始：两步赋值（out<<4 先存回 out，再与低半字节相加）
        out = (unsigned char)(out << 4);
        out = (unsigned char)(out + (hex[0] - '0'));
    } else if (hex[0] > '`' && hex[0] <= 'f') {
        out = (unsigned char)(out << 4);
        out = (unsigned char)(out + (hex[0] - 'a' + 10));
    } else {
        return 0;
    }
    return 1;
}

#ifndef DF_NO_CODEPAGE
unsigned int SDC_Rand(unsigned int* seed) {
    if (seed == NULL) {
        return 0;
    }
    // 原始：单累加器逐步 LCG（imul → 存栈 → add [ebp] 内存 RMW；result 逐步 shl/xor）
    unsigned int a = *seed;
    a *= 0x41c64e6d;
    a += 0x3039;
    unsigned int result = (a >> 0x10) & 0x7ff;
    a *= 0x41c64e6d;
    a += 0x3039;
    result <<= 10;
    result ^= (a >> 0x10) & 0x3ff;
    a *= 0x41c64e6d;
    a += 0x3039;
    result <<= 10;
    result ^= (a >> 0x10) & 0x3ff;
    *seed = a;
    return result;
}
#endif

// ---- DNFFLib ----

void DNFFLib::Binary2Hex(unsigned char const* data, int len, char* out) {
    // 原始：data 本地指针副本（mov [ebp-X],eax + 循环内 *p 递进）
    unsigned char const* p = data;
    out[0] = '\0';
    for (int i = 0; i < len; i++) {
        char tmp[3];
        Char2Hex(*p, tmp);
        strcat(out, tmp);
        p++;
    }
}

int DNFFLib::Hex2Binary(char const* hex, unsigned char* out, int maxLen) {
    int count = 0;
    while (count < maxLen) {
        unsigned char value;
        if (Hex2Char(hex, value) != 1) {
            // 原始：Hex2Char 失败即返回 0
            return 0;
        }
        // 原始：*out++ 指针递进（mov [eax],dl + add out,1）
        *out = value;
        out++;
        count++;
        hex += 2;
    }
    return 1;
}

unsigned int DNFFLib::get_rand_int(int range) {
    if (range < 0) {
        return 0;
    }
    if (range == 0) {
        return rand();
    }
    int r = rand();
    // 原始：r > range 时直接返回 rand() % range
    if (r > range) {
        return rand() % range;
    }
    // 原始：单累加器逐步 LCG（imul → 存栈 → add [ebp]；result 逐步 shl/xor）
    r *= 0x41c64e6d;
    r += 0x3039;
    // 原始：可移植算术右移惯用法（sar edx,31; shr edx,16; lea; sar）
    unsigned int result = ((int)(((unsigned int)(r >> 0x1f) >> 0x10) + r) >> 0x10) & 0x7ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= ((int)(((unsigned int)(r >> 0x1f) >> 0x10) + r) >> 0x10) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= ((int)(((unsigned int)(r >> 0x1f) >> 0x10) + r) >> 0x10) & 0x3ff;
    // 原始：result > range（无符号）才取模
    if (result > (unsigned int)range) {
        return result % range;
    }
    return result;
}

int DNFFLib::ExplodeString(char* str, char* delims, char** out, int maxCount) {
    int count = 0;
    if (maxCount < 1 || str == NULL) {
        out[0] = str;
        return 0;
    }
    // 原始：直接存尾指针 end = str + strlen(str)（mov [ebp-0x10],eax）
    char* end = str + strlen(str);
    char* token = strtok(str, delims);
    while (token != NULL) {
        out[count] = token;
        count++;
        if (count >= maxCount) {
            // 原始：token + strlen(token) 存为临时指针（mov [ebp-0xc],eax）
            char* tEnd = token + strlen(token);
            if (tEnd == end) {
                out[count] = NULL;
            } else {
                out[count] = tEnd + 1;
            }
            break;
        }
        token = strtok(NULL, delims);
    }
    return count;
}

void DNFFLib::PrintTextFile(char* file, char* text) {
    FILE* f = fopen(file, "a");
    if (f != NULL) {
        fprintf(f, text);  // 原始：文本直接作为格式串
        fclose(f);
    }
}

void DNFFLib::fPrintTextFile(char* file, char* fmt, ...) {
    char buffer[0x1000];
    memset(buffer, 0, 0x1000);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);
    PrintTextFile(file, buffer);
}

#ifndef DF_NO_CODEPAGE
bool DNFFLib::CharacSetSwitch(char const* from, char const* to, char* src, char* dst) {
    char* in = NULL;
    char* out = NULL;
    size_t inLen = 0;
    size_t outLen = 0;
    size_t remain = 0;
    iconv_t cd = iconv_open(to, from);
    if (cd == (iconv_t)-1) {
        // 原始：直接 return 0
        printf("iconv_open error : %s\n", strerror(errno));
        return 0;
    }
    in = src;
    out = dst;
    inLen = strlen(src);
    outLen = inLen * 3;
    remain = outLen;
    // 原始：iconv 结果存局部变量（size_t），成功分支在尾部（jne 跳转）
    size_t iconvResult = iconv(cd, &in, &inLen, &out, &remain);
    if (iconvResult == (size_t)-1) {
        printf("iconv error : %s\n", strerror(errno));
        return 0;
    }
    dst[outLen - remain] = '\0';
    iconv_close(cd);
    return 1;
}

bool DNFFLib::ConvertGBKtoUTF8(char* src, char* dst) {
    // 原始：call CharacSetSwitch 后 xor eax,1; test al,al; je（if (== false) return 0; return 1 形态）
    if (CharacSetSwitch("GBK", "UTF-8", src, dst) == false) {
        return 0;
    }
    return 1;
}

bool DNFFLib::ConvertUTF8toGBK(char* src, char* dst) {
    if (CharacSetSwitch("UTF-8", "GBK", src, dst) == false) {
        return 0;
    }
    return 1;
}
#endif

void DNFFLib::Make_Dir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        mkdir(path, 0x1ff);
    }
}

void DNFFLib::Sleep_Ext(int sec, int usec) {
    timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

#ifndef DF_NO_CODEPAGE
// ---- CodePage ----

void CodePage::initCodePage() {
    gClientEncoding = "UTF-8";
    gScriptEncoding = "BIG5";
    gDatabaseEncoding = "UTF-8";
}

char const* CodePage::client() {
    return gClientEncoding.c_str();
}

char const* CodePage::script() {
    return gScriptEncoding.c_str();
}

char const* CodePage::database() {
    return gDatabaseEncoding.c_str();
}

bool CodePage::client2Database(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gClientEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gClientEncoding.c_str(), gDatabaseEncoding.c_str(), src, dst);
}

bool CodePage::database2Client(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gClientEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gDatabaseEncoding.c_str(), gClientEncoding.c_str(), src, dst);
}

bool CodePage::database2Script(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gScriptEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gDatabaseEncoding.c_str(), gScriptEncoding.c_str(), src, dst);
}

bool CodePage::script2Database(char* src, char* dst) {
    if (gDatabaseEncoding.compare(gScriptEncoding) == 0) {
        return 1;
    }
    return DNFFLib::CharacSetSwitch(gScriptEncoding.c_str(), gDatabaseEncoding.c_str(), src, dst);
}
#endif
