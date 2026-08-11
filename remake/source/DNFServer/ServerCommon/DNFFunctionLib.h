#ifndef REDNF_DNFFUNCTIONLIB_H_
#define REDNF_DNFFUNCTIONLIB_H_

#include <string>

// 通用工具函数，实现来自 Ghidra 反编译（2026-08-06）

// 原始 comp_by_time 为多个 TU 内的 static 函数（_ZL12comp_by_timeRK15STGuildCargoLogS1_），
// 仅比较首字段（推断为时间）。
struct STGuildCargoLog {
    STGuildCargoLog();
    ~STGuildCargoLog();
    int time;  // offset 0
    char m_rest[0x2c];
} __attribute__((packed));

// 原始 comp_by_time 为头文件内 static 函数（_ZL12comp_by_timeRK15STGuildCargoLogS1_），
// 被多个 TU 包含后每个 TU 生成一份本地副本（原始共 18 份）。
// 语义：a.time > b.time；返回 bool（原始：cmp;setg al，无 movzx）。
static bool comp_by_time(STGuildCargoLog const& a, STGuildCargoLog const& b) {
    return a.time > b.time;
}

unsigned int timeGetTime();
char* NumberToString(unsigned int value, int index);
char* NumberToString(unsigned long long value, int index);
void PrintBackTrace();
unsigned long StringToNumber(char const* str);
char checkTimeToday(long t, int hour);
void GetCurrentResetBaseTime(long t, int hour);
void Char2Hex(unsigned char c, char* out);
bool Hex2Char(char const* hex, unsigned char& out);  // 原始：bool 返回（调用处 xor 惯用法）
#ifndef DF_NO_CODEPAGE
unsigned int SDC_Rand(unsigned int* seed);
#endif

class DNFFLib {
public:
    template<class T>
    static void Swap(T* a, T* b)
    {
        if (a != 0 && b != 0)
        {
            T t = *a;
            *a = *b;
            *b = t;
        }
    }
    static void Binary2Hex(unsigned char const* data, int len, char* out);
    static bool Hex2Binary(char const* hex, unsigned char* out, int maxLen);
    static unsigned int get_rand_int(int range);
    static int ExplodeString(char* str, char* delims, char** out, int maxCount);
    static void PrintTextFile(char* file, char* text);
    static void fPrintTextFile(char* file, char* fmt, ...);
#ifndef DF_NO_CODEPAGE
    static bool CharacSetSwitch(char const* from, char const* to, char* src, char* dst);
    static bool ConvertGBKtoUTF8(char* src, char* dst);
    static bool ConvertUTF8toGBK(char* src, char* dst);
#endif
    static void Make_Dir(char* path);
    static void Sleep_Ext(int sec, int usec);
};

#ifndef DF_NO_CODEPAGE
class CodePage {
public:
    static void initCodePage();
    static char const* client();
    static char const* script();
    static char const* database();
    static bool client2Database(char* src, char* dst);
    static bool database2Client(char* src, char* dst);
    static bool database2Script(char* src, char* dst);
    static bool script2Database(char* src, char* dst);
};
#endif

#endif  // REDNF_DNFFUNCTIONLIB_H_
