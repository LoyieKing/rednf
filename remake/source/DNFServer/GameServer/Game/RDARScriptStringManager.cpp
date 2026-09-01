// ============================================================================
// df_game_r RDARScriptStringManager（G2 字符串表批次B）
// ----------------------------------------------------------------------------
// 全局 g_scriptStringManager_ 由 CTradeSpace.cpp 引用（findString(key) 邮件文案）。
// ORIG（docs/class_func_reports/RDARScriptStringManager/）：
//   C1    _ZN23RDARScriptStringManagerC1Ev                                 0x8aa4e98
//   D1    _ZN23RDARScriptStringManagerD1Ev                                 0x8aa4f04
//   release _ZN23RDARScriptStringManager7releaseEv                         0x8aa5014
//   loadStringFile _ZN23RDARScriptStringManager14loadStringFileEPSiPKciPSt6vectorI8ErrorLogSaIS4_EE  0x8aa529e
//   等（见各函数注释）。
// 类型布局（ORIG +0x00 STScriptFileList / +0x18 vector<StringDictionary*> /
// +0x24 wstring tag_）由 RDARScriptStringManager.h 成员近似，ABI 一致。
// StringDictionary 类型与实现见 StringDictionary.h / StringDictionary.cpp
// （批次A，本 TU 仅依赖其公开接口，不修改其实现）。
// 依赖 toTString(const char*) / CodePage::script2Database / LogManager，均已存在。
// toUnicode/toTString(wchar) 属 G4 域未落地，此处以文件局部 static 等价实现，
// 不导出全局符号、不与 G 域 worker 冲突。
// ============================================================================

#include "RDARScriptStringManager.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cwchar>

#include "DNFFunctionLib.h"   // CodePage::script2Database
#include "LogManager.h"       // LogManager::logFormat
#include "SyncScriptDeps.h"   // toTString(const char*) / toTString(const wchar_t*)
#include "StringTypes.h"      // toUnicode（G4 域；返回 TLS 环形缓冲槽指针）

namespace
{

// ----------------------------------------------------------------------------
// ORIG 静态 _ZL15parseStringLinePcPS_S0_ 0x8aa6551（本 TU 文件局部）。
// 语义（objdump 0x8aa6551）：
//   1) 跳过行首空白（0x0d/0x0a/0x20/0x09）；跳空过程中遇 CR/LF → 返回 0（空行）。
//   2) 若行首为 NUL 或 "//"（注释）→ 返回 0。
//   3) 扫描查找分隔符 '>'（0x3e）：找到则置 NUL、后移、记录 value 起始；未找到 → 返回 -1。
//   4) keyLen==1（无有效 key）→ 返回 -1；keyLen-1 > 0x28（41）→ 返回 -2。
//   5) 解析 value：遇 CR/LF → 置 NUL 结束；遇 "\\n" → 替换为真实 '\n' 并前移；
//   6) *out_key = keyStart; *out_value = valueStart; 返回 0。
// 返回：0=成功（或空/注释行，此时出参不变）、-1=无分隔符或 key 过短、-2=key 过长。
// ----------------------------------------------------------------------------
int parseStringLine(char* line, char** outKey, char** outValue)
{
    char* p = line;

    // 1) 跳过行首空白；遇换行 → 空行
    for (;;)
    {
        if (*p == '\0')
            break;
        if (*p == '\r' || *p == '\n')
            return 0;
        if (*p == ' ' || *p == '\t')
        {
            ++p;
            continue;
        }
        break;
    }

    // 2) 空行或注释
    if (*p == '\0')
        return 0;
    if (*p == '/' && *(p + 1) == '/')
        return 0;

    // 3) 找 '>' 分隔符
    char* keyStart = p;
    char* valueStart = 0;
    while (*p != '\0')
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            valueStart = p;
            break;
        }
        ++p;
    }

    // 4) 无分隔符或 key 无效
    if (valueStart == 0)
        return -1;
    if ((valueStart - keyStart) == 1)
        return -1;
    if ((valueStart - keyStart - 1) > 0x28)
        return -2;

    // 5) 解析 value：换行结束 / "\\n" 转义
    p = valueStart;
    while (*p != '\0')
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        if (*p == '\\' && *(p + 1) == 'n')
        {
            *p = '\n';
            // 整体前移，移除 'n'
            char* tail = p + 1;
            char* dst = p;
            ++tail;
            while (*tail != '\0')
                *dst++ = *tail++;
            *dst = '\0';
        }
        ++p;
    }

    // 6) 出参
    *outKey = keyStart;
    *outValue = valueStart;
    return 0;
}

}  // namespace

// ORIG 字符串表函数用 ss_sprintf<256>（安全限长）而非裸 sprintf：
// 模板定义与 OnlyOrigBatch12.cpp 一致（extern ss_vsprintf，直接传变参起始地址）。
extern int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList);
template <size_t N>
int ss_sprintf(char (&buffer)[N], const char* format, ...)
{
    return ss_vsprintf(buffer, N, format, reinterpret_cast<char*>(&format + 1));
}

RDARScriptStringManager::RDARScriptStringManager()
{
    // ORIG C1 0x8aa4e98：STScriptFileList(+0) / vector<StringDictionary*> (+0x18)
    // / wstring (+0x24) 依次默认构造。头成员 fileList_/dictionaries_/tag_ 由
    // 成员初始化列表默认构造（map/vector/wstring 空构造），体为空即达 ORIG 语义。
}

RDARScriptStringManager::~RDARScriptStringManager()
{
    // ORIG D1 0x8aa4f04：逐元素 delete StringDictionary*，随后成员析构。
    for (size_t i = 0; i < dictionaries_.size(); ++i)
    {
        StringDictionary* d = dictionaries_[i];
        if (d != 0)
            delete d;
    }
}

const char* RDARScriptStringManager::findString(int table, const char* key,
                                                bool* flag) const
{
    // ORIG 0x8aa57fe
    if (key == 0)
    {
        return "RDARScriptStringManager::ERROR::input NULL string";
    }

    static char errStr[256];
    if (table >= static_cast<int>(dictionaries_.size()))
    {
        return "RDARScriptStringManager::ERROR::requested fileIndex isn't in string table";
    }

    if (table < 0 || table >= static_cast<int>(dictionaries_.size()))
    {
        LogManager::logFormat(
            0, "../RDARScriptStringManager.cpp", __FUNCTION__, 0x11c,
            "오류 : 스트링 테이블 %d 는 범위를 벗어납니다. "
            "(index out of range reading string table %d)",
            table);
        ss_sprintf(errStr, "String table index out of range %d", table);
        return errStr;
    }

    const char* result = dictionaries_[table]->get(toTString(key));
    if (flag != 0)
        *flag = (result != 0);
    if (result != 0)
        return result;

    const char* fname = fileList_.GetFileName(table);
    ss_sprintf(errStr, "%s == NULL, %s : ", toTString(key), fname);
    return errStr;
}

const char* RDARScriptStringManager::findString(int table, const wchar_t* key,
                                                bool* flag) const
{
    // ORIG 0x8aa594c
    if (key == 0)
    {
        return "RDARScriptStringManager::ERROR::input NULL string";
    }

    static char errStr[256];
    std::string k = toTString(key);
    const char* result = dictionaries_[table]->get(k.c_str());
    if (flag != 0)
        *flag = (result != 0);
    if (result != 0)
        return result;

    const char* fname = fileList_.GetFileName(table);
    ss_sprintf(errStr, "%s == NULL, %s : ", k.c_str(), fname);
    return errStr;
}

int RDARScriptStringManager::getKeyIndex(int table, const wchar_t* key) const
{
    // ORIG 0x8aa5cba：key==0 或 table 越界 → -1
    if (key != 0 && static_cast<unsigned>(table) < dictionaries_.size())
    {
        StringDictionary* d = dictionaries_[table];
        int n = d->getCount();
        for (int i = 0; i < n; ++i)
        {
            const char* v = d->getValue(i);
            if (v != 0)
            {
                const wchar_t* wv = toUnicode(v);
                if (wcscmp(wv, key) == 0)
                {
                    if (d->getKey(i) != 0)
                        return i;
                }
            }
        }
    }
    return -1;
}

void RDARScriptStringManager::getTagByName(std::vector<std::wstring>* a,
                                           std::vector<std::wstring>* b,
                                           int table, const wchar_t* name) const
{
    // ORIG 0x8aa59f0：遍历表；每条 value（去空格）若 wcsstr 包含 name 且 key 非空，
    // 则把 key/value 分别 push 进 a/b。
    if (a == 0 || b == 0)
        return;
    if (static_cast<unsigned>(table) >= dictionaries_.size())
        return;

    StringDictionary* d = dictionaries_[table];
    int n = d->getCount();
    for (int i = 0; i < n; ++i)
    {
        const char* v = d->getValue(i);
        if (v == 0)
            continue;

        std::string sv = v;
        // ORIG 0x8aa59f0：仅 std::remove（未 erase，尾部空格仍保留在串中）。
        std::remove(sv.begin(), sv.end(), ' ');
        std::wstring wv = toUnicode(sv);
        if (wcsstr(wv.c_str(), name) == 0)
            continue;

        const char* k = d->getKey(i);
        if (k == 0)
            continue;

        a->push_back(std::wstring(toUnicode(k)));
        b->push_back(std::wstring(toUnicode(v)));
    }
}

const wchar_t* RDARScriptStringManager::getTagByName(int table, const wchar_t* a,
                                                     const wchar_t* b, bool flag) const
{
    // ORIG 0x8aa5e40（169 指令逐段转写）：全程窄 char[256] TLS 缓冲
    //（%gs 相对 -0x3c248 / -0x3c148）+ ss_sprintf<256> + strcmp，
    // 没有任何 std::wstring / std::allocator<wchar_t> 临时对象
    //（ORIG 调用点里 _ZNSbIwSt11char_traitsIwESaIwEEC1* / _ZNSaIwEC1Ev 全为 0）。
    // 原来的宽串拼接实现属臆造，每轮循环都多出 basic_string/allocator 构造
    // 与其 EH 清理（eager-ctor 冗余），故按 ORIG 形态改回窄串缓冲。
    static __thread char buf[256];      // ORIG TLS -0x3c248
    static __thread char errBuf[256];   // ORIG TLS -0x3c148

    (void)flag;                         // ORIG -0x2c：bool 形参归一化存栈后未再使用

    int idx = -1;                       // ORIG -0x24
    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        // ORIG -0x20 存的是 const operator[] 返回的引用地址，每次使用再解引用
        StringDictionary* const& d = dictionaries_[table];
        idx = getKeyIndex(table, a);
        if (idx == -1)
        {
            if (b != 0 && *b != L'\0')
                idx = getKeyIndex(table, b);

            const char* base = 0;       // ORIG -0x18
            if (idx != -1)
                base = d->getKey(idx);
            else
                base = reinterpret_cast<const char*>(tag_.c_str());  // ORIG 按窄串直用

            for (int i = 0; i < 10000; ++i)
            {
                ss_sprintf(buf, "%s_%d", base, i);
                bool exists = false;                       // ORIG -0xd
                for (int j = 0; j < d->getCount(); ++j)    // ORIG 每轮重调 getCount()
                {
                    if (strcmp(d->getKey(j), buf) == 0)    // ORIG 无 NULL 检查
                    {
                        exists = true;
                        break;
                    }
                }
                if (!exists)
                {
                    // ORIG：add(key=buf, value=a[0x10(%ebp)]) 直接把 wchar_t* 当 char* 传。
                    d->add(buf, reinterpret_cast<const char*>(a));
                    idx = d->getCount() - 1;
                    break;
                }
            }
        }

        int unusedSlot = 0;             // ORIG -0x1c：清零后未再引用
        (void)unusedSlot;
        if (idx != -1)
        {
            ss_sprintf(buf, "<%d::%s>", table, d->getKey(idx));
            return toUnicode(buf);
        }
    }

    ss_sprintf(errBuf, "`%s is not found, %s`",
               reinterpret_cast<const char*>(a), fileList_.GetFileName(table));
    return reinterpret_cast<const wchar_t*>(errBuf);
}

void RDARScriptStringManager::setTableValue(int table, int key, const char* value)
{
    // ORIG 0x8aa5d98
    if (static_cast<unsigned>(table) < dictionaries_.size())
        dictionaries_[table]->setValue(key, value);
}

bool RDARScriptStringManager::loadStringFile(std::istream* in, const char* path,
                                             int /*table*/,
                                             std::vector<ErrorLog>* err)
{
    // ORIG 0x8aa529e
    bool skip = false;
    if (in != 0 && path != 0)
    {
        if (in->bad())
            skip = true;
        // else skip 保持 false
    }
    else
    {
        skip = true;
    }
    if (skip)
        return false;

    std::auto_ptr<StringDictionary> dict(new StringDictionary());

    bool isJpn = (std::strstr(path, "Creature.jpn.str") != 0);   // local_32
    bool isKor = (std::strcmp(path, "Etc/Etc.kor.str") == 0);    // local_31

    int lineNo = 0;
    char lineBuf[0x2928];
    char cpBuf[4000];

    while (!in->eof())
    {
        ++lineNo;
        in->getline(lineBuf, 0x2928);

        char* key = 0;
        char* value = 0;
        int r = parseStringLine(lineBuf, &key, &value);

        if (r == -2)
        {
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000001);
                e.m_message = path;
                err->push_back(e);
            }
            return false;
        }
        if (r == -1)
        {
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000000);
                e.m_message = path;
                err->push_back(e);
            }
            return false;
        }

        if (key != 0 && value != 0)
        {
            if (isKor)
            {
                std::memset(cpBuf, 0, sizeof(cpBuf));
                std::strcpy(cpBuf, value);
                CodePage::script2Database(value, cpBuf);
                value = cpBuf;
            }

            if (!isJpn)
            {
                dict->add(toTString(key), toTString(value));
            }
            else
            {
                const char* tv = toTString(value);
                if (*tv == '\0')
                    dict->add(toTString(key), "NULL");
                else
                    dict->add(toTString(key), toTString(value));
            }
        }
    }

    dict->freeze();

    // 重复连续 key 检测
    bool foundDuplicate = false;
    int count = dict->getCount() - 1;
    for (int i = 0; i < count; ++i)
    {
        const char* k0 = dict->getKey(i);
        const char* k1 = dict->getKey(i + 1);
        if (k0 != 0 && k1 != 0 && std::strcmp(k0, k1) == 0)
        {
            std::printf("%s, %s \n", k0, k1);
            foundDuplicate = true;
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000002);
                e.m_message = path;
                e.m_message += dict->getKey(i);
                err->push_back(e);
            }
        }
    }

    if (!foundDuplicate)
    {
        dictionaries_.push_back(dict.release());
        return true;
    }
    return false;
}

void RDARScriptStringManager::writeStringFile(int table)
{
    // ORIG 0x8aa50bc：写 "Script/<文件名>"，逐条 "key>value\n"，value 内 '\n' 转义
    // 为 "\\n"。
    const char* fname = fileList_.GetFileName(table);
    if (fname == 0)
        return;

    char path[512];
    ss_sprintf(path, "Script/%s", fname);
    std::FILE* fp = std::fopen(path, "w");
    if (fp == 0)
        return;

    if (static_cast<unsigned>(table) < dictionaries_.size())
    {
        StringDictionary* d = dictionaries_[table];
        int n = d->getCount();
        for (int i = 0; i < n; ++i)
        {
            const char* key = d->getKey(i);
            const char* value = d->getValue(i);
            char buf[1024];
            std::strcpy(buf, (value != 0) ? value : "");

            if (key != 0)
            {
                // '\n' → "\\n"：从 '\n' 处右移剩余串，插入 '\' 和 'n'。
                for (char* p = buf; *p != '\0'; ++p)
                {
                    if (*p == '\n')
                    {
                        int len = 1;
                        char* q = p;
                        while (*(++q) != '\0')
                            ++len;
                        char* r = p + len;
                        for (int cnt = len; cnt >= 0; --cnt)
                        {
                            r[1] = *r;
                            --r;
                        }
                        *p = '\\';
                        ++p;
                        *p = 'n';
                    }
                }
                std::fprintf(fp, "%s>%s\n", key, buf);
            }
        }
    }
    std::fclose(fp);
}

int RDARScriptStringManager::getTagSizeInFile(int table)
{
    // ORIG 0x8aa5df2
    if (static_cast<unsigned>(table) < dictionaries_.size())
        return dictionaries_[table]->getCount();
    return -1;
}

const char* RDARScriptStringManager::findChangedString(int table, const char* key,
                                                       bool* flag)
{
    // ORIG 0x8aa6448：函数级 static std::string ret（__cxa_guard 单次初始化），
    // 首次调用以 findString 结果填充，随后 changeSpacilTagStr 并返回 c_str。
    static std::string ret = findString(table, key, flag);
    changeSpacilTagStr(ret);
    return ret.c_str();
}

void* RDARScriptStringManager::getStringFileList()
{
    // ORIG 0x8aa50b4：STScriptFileList 子对象位于 +0x00，返回 this。
    return this;
}

bool RDARScriptStringManager::changeSpacilTagStr(std::string& s)
{
    // ORIG 0x8aa60b0：把 "<URL LINK>内文</URL LINK>" 转为 0x1c 内文 0x1d，
    // 尾部残留追加，最后补 "\n"。任何初始条件不满足返回 0 且不修改 s；
    // 成功替换返回 1。
    static const char* OPEN = "<URL LINK>";
    static const char* CLOSE = "</URL LINK>";

    if (s.empty())
        return false;

    std::string::size_type posOpen = s.find(OPEN, 0);
    std::string::size_type posClose = s.rfind(CLOSE, std::string::npos);
    if (posOpen == std::string::npos || posClose == std::string::npos ||
        posOpen == posClose)
    {
        return false;
    }

    std::string part1 = s.substr(0, posOpen);
    std::string partInner =
        s.substr(posOpen + std::strlen(OPEN), posClose - posOpen - std::strlen(OPEN));
    std::string partTail =
        s.substr(posClose + std::strlen(CLOSE), s.length() - posClose - std::strlen(CLOSE));

    if (part1.empty() || partInner.empty())
        return false;

    std::string out = part1;
    out += static_cast<char>(0x1c);
    out += partInner;
    out += static_cast<char>(0x1d);
    if (!partTail.empty())
        out += partTail;
    out += "\n";

    if (out.empty())
        return false;

    s.clear();
    s = out;
    return true;
}

void RDARScriptStringManager::release()
{
    // ORIG 0x8aa5014：逐元素 delete StringDictionary*，随后 clear vector。
    for (size_t i = 0; i < dictionaries_.size(); ++i)
    {
        StringDictionary* d = dictionaries_[i];
        if (d != 0)
            delete d;
    }
    dictionaries_.clear();
}

RDARScriptStringManager g_scriptStringManager_;