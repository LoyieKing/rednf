// ============================================================================
// df_game_r 还原 —— HistoryLog（G2-4 批次，历史日志文件写入）。
// 依据 docs/class_func_reports/HistoryLog.md + ORIG 反汇编逐函数转录。
// 静态成员：+0x00 StaticPool<_IO_FILE,50> logfiles_（0x30）| int count_
// | char time[0x10]。格式串字节直接取 ORIG rodata（EUC-KR）。
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "CGameManager.h"

// ---- 外部符号声明（对应 TU 翻译后移除） ----
void get_str_datetime(char* buf, int len);
void get_str_date(char* buf, int len, char sep);
char* NumberToString(unsigned int value, int radix);
void get_str_date(char* buf, int len, char sep)
{
    (void)len;
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm value;
    localtime_r(&now, &value);
    if (sep == 0) sprintf(buf, "%04d-%02d-%02d", value.tm_year + 1900, value.tm_mon + 1, value.tm_mday);
    else sprintf(buf, "%04d%c%02d%c%02d", value.tm_year + 1900, sep, value.tm_mon + 1, sep, value.tm_mday);
}
void get_str_datetime(char* buf, int len)
{
    (void)len;
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm value;
    localtime_r(&now, &value);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", value.tm_year + 1900, value.tm_mon + 1, value.tm_mday, value.tm_hour, value.tm_min, value.tm_sec);
}

// ---- 格式串（ORIG rodata 逐字节） ----
static const char FMT_BUY_ITEM[] =
    "\133\045\163\135\276\306\300\314\305\333\261\270\300\324\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012";
static const char FMT_CHAT[] =
    "\133\045\163\135\303\244\306\303\305\270\300\324\072\045\144\040\263\273\277"
    "\353\072\040\045\163\015\012";
static const char FMT_COMPOSITION[] =
    "\133\045\163\135\307\325\274\272\040\267\271\274\255\307\307\072\040\045\144"
    "\015\012";
static const char FMT_SELECT_CH[] =
    "\133\045\163\135\304\263\270\257\274\261\305\303\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\045\144\015\012";
static const char FMT_DUN_START[] =
    "\133\045\163\135\264\370\301\257\040\275\303\300\333\040\112\157\142\072\040"
    "\045\144\054\040\114\145\166\145\154\072\040\045\144\040\274\272\300\345\307"
    "\374\072\040\045\144\015\012";
static const char FMT_FINE_QUEST[] =
    "\133\045\163\135\304\371\275\272\306\256\040\277\317\267\341\072\040\045\144"
    "\015\012";
static const char FMT_GIVE_UP[] =
    "\133\045\163\135\264\370\301\257\306\367\261\342\072\040\045\163\015\012";
static const char FMT_KILL_MOB[] =
    "\133\045\163\135\113\151\154\154\040\115\157\142\124\171\160\145\072\040\045"
    "\144\015\012";
static const char FMT_LEARN_SKILL[] =
    "\133\045\163\135\275\272\305\263\300\324\274\366\040\111\104\072\045\144\054"
    "\040\114\145\166\145\154\072\045\144\015\012";
static const char FMT_LEVEL_UP[] =
    "\133\045\163\135\267\271\272\247\276\367\072\040\045\144\015\012";
static const char FMT_DUN_CLEAR[] =
    "\133\045\163\135\264\370\301\257\040\305\254\270\256\276\356\015\012";
static const char FMT_TRADE_DOWN[] =
    "\133\045\163\135\306\256\267\271\300\314\265\345\040\264\331\277\356\270\256"
    "\275\272\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144"
    "\015\012";
static const char FMT_TRADE_UP[] =
    "\133\045\163\135\306\256\267\271\300\314\265\345\040\276\367\270\256\275\272"
    "\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144\015\012";
static const char FMT_USE_COIN[] =
    "\133\045\163\135\304\332\300\316\273\347\277\353\040\055\076\040\045\163\015"
    "\012";
static const char FMT_SELL_ITEM[] =
    "\133\045\163\135\276\306\300\314\305\333\306\307\270\305\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012";
static const char FMT_DIE[] =
    "\133\045\163\135\307\303\267\271\300\314\301\337\040\273\347\270\301\015\012";
static const char FMT_DISJOINT[] =
    "\133\045\163\135\307\330\303\274\040\111\104\072\040\045\144\015\012";
static const char FMT_DELETE_CH[] =
    "\133\045\163\135\304\263\270\257\273\350\301\246\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\040\045\144\015\012";
static const char FMT_CREATE_CH[] =
    "\133\045\163\135\304\263\270\257\273\275\274\272\072\045\163\040\301\367\276"
    "\367\072\045\144\015\012";

// ----------------------------------------------------------------------------
// HistoryLog
// ----------------------------------------------------------------------------
class HistoryLog
{
public:
    static bool Init();
    static FILE* LogOpen(unsigned int chNo);
    static void LogClose(FILE* file);
    static bool CheckFD(FILE* file);

    static void WriteCreateCh(FILE* file, char* name, int job, int level, int growType);
    static void WriteDeleteCh(FILE* file, char* name, int job, int level, int growType);
    static void WriteSelectCh(FILE* file, const char* name, int job, int level,
                              int growType);
    static void WriteLevelUp(FILE* file, int level);
    static void WriteChat(FILE* file, int type, char* content, int level);
    static void WriteFineQuest(FILE* file, int questIdx);
    static void WriteLearnSkill(FILE* file, int skillIdx, int level);
    static void WriteDunStart(FILE* file, int job, int level, int growType);
    static void WriteKillMob(FILE* file, int mobType);
    static void WriteDie(FILE* file);
    static void WriteGiveUp(FILE* file, const char* reason);
    static void WriteUseCoin(FILE* file, const char* reason);
    static void WriteDunClear(FILE* file);
    static void WriteTradeUp(FILE* file, int itemIdx, int count);
    static void WriteTradeDown(FILE* file, int itemIdx, int count);
    static void WriteDisJoint(FILE* file, int itemIdx);
    static void WriteComposition(FILE* file, int itemIdx);
    static void WriteBuyItem(FILE* file, int itemIdx, int count);
    static void WriteSellItem(FILE* file, int itemIdx, int count);

    static StaticPool<_IO_FILE, 50> logfiles_;  // 0943dd00
    static int count_;                          // 0943dd30
    static char time[0x10];                     // 0943dd34
};

StaticPool<_IO_FILE, 50> HistoryLog::logfiles_;
int HistoryLog::count_;
char HistoryLog::time[0x10];

bool HistoryLog::Init()
{
    struct stat st;
    if (stat("./history/", &st) < 0)
    {
        if (mkdir("./history/", 0x1fd) < 0)
        {
            printf("Fail To Make Directory, PATH[%s]\n", "./history/");
            return false;
        }
    }
    return true;
}

bool HistoryLog::CheckFD(FILE* file)
{
    if (file == 0)
    {
        return false;
    }
    get_str_datetime(time, 0x10);
    return true;
}

void HistoryLog::LogClose(FILE* file)
{
    if (file != 0)
    {
        fclose(file);
        logfiles_.Free(file);
        count_ -= 1;
    }
}

FILE* HistoryLog::LogOpen(unsigned int chNo)
{
    int rnd = rand();
    rnd = rnd % 100;
    if (rnd <= 0x5f)
    {
        return 0;
    }

    FILE* file = logfiles_.Acquire();
    if (file == 0)
    {
        return 0;
    }

    char date[10];
    date[0] = '\0';
    date[1] = '\0';
    date[2] = '\0';
    date[3] = '\0';
    date[4] = '\0';
    date[5] = '\0';
    date[6] = '\0';
    date[7] = '\0';
    date[8] = '\0';
    date[9] = '\0';
    get_str_date(date, 10, '\0');

    char dir[50];
    memset(dir, 0, sizeof(dir));
    sprintf(dir, "%s%s/", "./history/", date);

    struct stat st;
    if (stat(dir, &st) < 0)
    {
        mkdir(dir, 0x1fd);
    }

    char path[127];
    memset(path, 0, sizeof(path));
    char* num = NumberToString(chNo, 0);
    sprintf(path, "%s%s.log", dir, num);

    file = fopen(path, "a");
    if (file == 0)
    {
        return 0;
    }
    count_ += 1;
    return file;
}

void HistoryLog::WriteCreateCh(FILE* file, char* name, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_CREATE_CH, time, name, job);
}

void HistoryLog::WriteDeleteCh(FILE* file, char* name, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DELETE_CH, time, name, job, level, growType);
}

void HistoryLog::WriteSelectCh(FILE* file, const char* name, int job, int level,
                               int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_SELECT_CH, time, name, job, level, growType);
}

void HistoryLog::WriteLevelUp(FILE* file, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_LEVEL_UP, time, level);
}

void HistoryLog::WriteChat(FILE* file, int type, char* content, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_CHAT, time, type, content);
}

void HistoryLog::WriteFineQuest(FILE* file, int questIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_FINE_QUEST, time, questIdx);
}

void HistoryLog::WriteLearnSkill(FILE* file, int skillIdx, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_LEARN_SKILL, time, skillIdx, level);
}

void HistoryLog::WriteDunStart(FILE* file, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DUN_START, time, job, level, growType);
}

void HistoryLog::WriteKillMob(FILE* file, int mobType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_KILL_MOB, time, mobType);
}

void HistoryLog::WriteDie(FILE* file)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DIE, time);
}

void HistoryLog::WriteGiveUp(FILE* file, const char* reason)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_GIVE_UP, time, reason);
}

void HistoryLog::WriteUseCoin(FILE* file, const char* reason)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_USE_COIN, time, reason);
}

void HistoryLog::WriteDunClear(FILE* file)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DUN_CLEAR, time);
}

void HistoryLog::WriteTradeUp(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_TRADE_UP, time, itemIdx, count);
}

void HistoryLog::WriteTradeDown(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_TRADE_DOWN, time, itemIdx, count);
}

void HistoryLog::WriteDisJoint(FILE* file, int itemIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DISJOINT, time, itemIdx);
}

void HistoryLog::WriteComposition(FILE* file, int itemIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_COMPOSITION, time, itemIdx);
}

void HistoryLog::WriteBuyItem(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_BUY_ITEM, time, itemIdx, count);
}

void HistoryLog::WriteSellItem(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_SELL_ITEM, time, itemIdx, count);
}
