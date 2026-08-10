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
    // ORIG DWARF: saucHex = unsigned char[] (non-const, .data); const 会改变
    // GCC 4.4 -O0 的寄存器分配（edx 直接装载 vs eax->edx 拷贝），须保持非 const。
    static unsigned char saucHex[] = "0123456789abcdef";
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
// DWARF 类型为 char[8][0x40]（两个 subrange：7/63），必须按二维数组形态书写，
// 才能让 &gNumberToStringBuffer[index] 的 PLUS 以「基址寄存器为目的地」展开（lea），
// 与 ORIG 一致；写成 char[0x200] + index*0x40 会产生 add 形态（§39 伪影）。
static __thread char gNumberToStringBuffer[8][0x40];

char* NumberToString(unsigned int value, int index) {
    sprintf(gNumberToStringBuffer[index], "%u", value);
    return gNumberToStringBuffer[index];
}

char* NumberToString(unsigned long long value, int index) {
    sprintf(gNumberToStringBuffer[index], "%qu", value);
    return gNumberToStringBuffer[index];
}

void PrintBackTrace() {
    void* frames[100];
    // ORIG DWARF（line 455-457）：buffer、stack_strings、nptrs 声明序为
    // stack_strings 在前、nptrs 在后（§40 栈槽反序分配 → nptrs@-0x10、
    // stack_strings@-0x14）。写成 count 在前会产生槽位互换 DIFF。
    char** symbols;
    int count;
    count = backtrace(frames, 100);
    symbols = backtrace_symbols(frames, count);
    // 原始：if (== NULL) return; 早退形态（§3.3 共享出口 jmp+nop），
    // 循环变量名 index（ORIG DWARF decl_line 464）。
    if (symbols == NULL) {
        return;
    }
    for (int index = 0; index < count; index++) {
        // 原始：先取 symbols[index] 到局部变量（触发 ebx 保存，ctor 调用前取值）
        register char* s = symbols[index];
        CMyFileLog log("PrintBackTrace", 0x1d1);
        log("./log/BackTrace", s);
    }
    free(symbols);
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
    long yester_time = t - 86400;
    tm now;
    tm yday;
    tm* tm_ptr = localtime_r(&t, &now);
    tm* tm_ptr_yester = localtime_r(&yester_time, &yday);
    // ORIG：localtime_r 调用后声明 reset_time（置 0）与 day（赋 hour）。
    long reset_time = 0;
    int day;
    day = hour;
    // 原始：条件为 tm_ptr->tm_hour >= 0 && tm_ptr->tm_hour < day 时改 ydayP，否则改 nowP
    if (tm_ptr->tm_hour >= 0 && tm_ptr->tm_hour < day) {
        tm_ptr_yester->tm_hour = day;
        tm_ptr_yester->tm_min = 0;
        tm_ptr_yester->tm_sec = 0;
        mktime(tm_ptr_yester);
    } else {
        tm_ptr->tm_hour = day;
        tm_ptr->tm_min = 0;
        tm_ptr->tm_sec = 0;
        mktime(tm_ptr);
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
    // ORIG DWARF：局部 i(67)/szHex(68)/pucBinStr1(69)，声明不初始化，
    // 体中按 pucBinStr1→out[0]→i 序赋值（同 wrapper 版形态）。
    int i;
    char szHex[3];
    unsigned char const* pucBinStr1;
    pucBinStr1 = data;
    out[0] = '\0';
    i = 0;
    while (i < len) {
        Char2Hex(*pucBinStr1, szHex);
        strcat(out, szHex);
        // ORIG：i 先自增、指针后自增。
        i++;
        pucBinStr1++;
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
        // 原始：*out = value 后按 count→hex→out 序自增。
        *out = value;
        count++;
        hex += 2;
        out++;
    }
    return 1;
}

// ORIG 证据（全部 9 个服务）：r / 65536 用非 cmov 的
// mov edx,eax; sar 31,edx; shr 16,edx; lea; sar 16 形态（i586 除法定型），
// 与 -march=i586 产物逐指令一致；§89 允许函数级 target 覆盖。
// divide==0 分支为 switch 形态（mov divide,%eax; test; jne），不能写成 if。
__attribute__((target("arch=i586")))
unsigned int DNFFLib::get_rand_int(int divide) {
    if (divide < 0) {
        return 0;
    }
    switch (divide) {
    case 0:
        return rand();
    default:
        break;
    }
    int r = rand();
    // 原始：r > range 时直接返回 rand() % range
    if (r > divide) {
        return rand() % divide;
    }
    // 原始：单累加器逐步 LCG（imul → 存栈 → add [ebp]；result 逐步 shl/xor）
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int result = (r / 65536) & 0x7ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    // 原始：result > range（无符号）才取模
    if (result > (unsigned int)divide) {
        return result % divide;
    }
    return result;
}

int DNFFLib::ExplodeString(char* str, char* delims, char** out, int maxCount) {
    // ORIG DWARF 局部：iTokenCnt(158)/pToken(159)/pLast(160)/pTail(160)。
    int iTokenCnt = 0;
    if (maxCount < 1 || str == NULL) {
        out[0] = str;
        return 0;
    }
    // 原始：直接存尾指针 end = str + strlen(str)（mov [ebp-0x10],eax）
    // ORIG 声明序：pToken(159) 先于 pLast(160)，但体中先赋 pLast 再 pToken。
    char* pToken;
    char* pLast;
    pLast = str + strlen(str);
    pToken = strtok(str, delims);
    while (pToken != NULL) {
        out[iTokenCnt] = pToken;
        iTokenCnt++;
        if (iTokenCnt >= maxCount) {
            // 原始：token + strlen(token) 存为临时指针（mov [ebp-0xc],eax）
            char* pTail = pToken + strlen(pToken);
            if (pTail == pLast) {
                out[iTokenCnt] = NULL;
            } else {
                out[iTokenCnt] = pTail + 1;
            }
            break;
        }
        pToken = strtok(NULL, delims);
    }
    return iTokenCnt;
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
    char* SrcPtr = NULL;
    char* DstPtr = NULL;
    size_t size1 = 0;
    size_t string_size = 0;
    size_t size2 = 0;
    size_t cc;
    iconv_t it = iconv_open(to, from);
    if (it == (iconv_t)-1) {
        printf("iconv_open error : %s\n", strerror(errno));
        return 0;
    }
    SrcPtr = src;
    DstPtr = dst;
    size1 = strlen(SrcPtr);
    string_size = size1 * 3;
    size2 = string_size;
    cc = iconv(it, &SrcPtr, &size1, &DstPtr, &string_size);
    if (cc == (size_t)-1) {
        printf("iconv error : %s\n", strerror(errno));
        return 0;
    }
    dst[size2 - string_size] = '\0';
    iconv_close(it);
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
