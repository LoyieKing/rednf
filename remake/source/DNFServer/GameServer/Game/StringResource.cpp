// df_game_r 还原 —— StringResource / CStringManager 实现（报告依据见 .h 文件头）。
// 逐段转写自 ORIG objdump（地址见各函数注释）；static helper
// parseStringLine（ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f）同 TU。  

#include "StringResource.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>

// ---------------------------------------------------------------------------
// StringResource C1/D1 —— ORIG 仅构造/析构 std::map<int,CharString> 成员。
// ---------------------------------------------------------------------------
StringResource::StringResource() {}
StringResource::~StringResource() {}

// ---------------------------------------------------------------------------
// parseStringLine —— ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f。
// 行格式 "<key数字> > <value>"；'/' 开头("//")为注释；'\r'/'\n' 开头空行；
// 返回：0=成功（*key=atoi(key 区)、*value=value 起）、-1=key 含非数字、
// -2=key 位数>11。value 内 "\n" 转义为真实换行并左移，
// 其余 '\' 删除前缀字符（与 ORIG 循环形态一致），行结束('\r'/'\n')截断。
// 错误码经调用方 "Parse error : line:%d reason:%d" 记入 error。
// ---------------------------------------------------------------------------
static int parseStringLine(char* line, int* key, char** value)
{
    char* p = line;
    // 前导空白（' '/'\t'）跳过；空行/换行起始 → 失败（ORIG 返回非解析态）
    while (*p)
    {
        if (*p == '\r')
            return 0;
        if (*p == '\n')
            return 0;
        if (*p == ' ' || *p == '\t')
        {
            ++p;
            continue;
        }
        break;
    }
    if (*p == '\0')
        return 0;                       // 全空白行（ORIG 同路径返回）
    if (p[0] == '/' && p[1] == '/')     // ORIG "//" 注释 → return 0
        return 0;

    char* keyStart = p;
    char* valueStart = 0;
    while (*p)
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            valueStart = p;
            break;
        }
        if (*p <= '/')
            return -1;                  // key 非 [0-9] 字符
        if (*p > '9')
            return -1;
        ++p;
    }
    if (valueStart == 0)
        return -1;                      // 未发现 '>' 分隔

    const int keyDigits = static_cast<int>(valueStart - keyStart) - 1;
    if (keyDigits > 10)
        return -2;                      // ORIG cmp $0xa 上限

    // value 内扫描：'\r'/'\n' 终结该行；"\\n" 转义真实换行；其余 '\\' 消除。
    p = valueStart;
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        if (*p == '\\')
        {
            if (p[1] == 'n')
            {
                *p = '\n';
                // 左移覆盖被吃掉的第二个字符（ORIG copy 循环）。
                for (char* q = p + 1; *q; ++q)
                    *q = *(q + 1);
                continue;
            }
            // 其余转义：删除 '\' 本身，保留后续字面字符。
            for (char* q = p; *q; ++q)
                *q = *(q + 1);
            continue;
        }
        ++p;
    }

    *key = atoi(keyStart);
    *value = valueStart;
    return 0;
}

// ---------------------------------------------------------------------------
// add —— ORIG T 0x08ad47d0：map.insert(make_pair(id,value))，返回新增与否。
// set —— ORIG T 0x08ad48e6。
// get —— ORIG T 0x08ad4878：find != end 时 *out=&second 返回 1，否则返回 0。
// ---------------------------------------------------------------------------
bool StringResource::add(int id, const CharString& value)
{
    return m_strings.insert(std::make_pair(id, value)).second;
}

void StringResource::set(int id, const CharString& value)
{
    m_strings[id] = value;
}

bool StringResource::get(int id, const CharString** out) const
{
    std::map<int, CharString>::const_iterator it = m_strings.find(id);
    if (it == m_strings.end())
        return false;
    *out = &it->second;
    return true;
}

// ---------------------------------------------------------------------------
// loadFromString —— ORIG T 0x08ad4ac2。
// clear 表后逐行解析；成功且 key!=0 → add(key, CharString(value))；
// key==0 的条目跳过；重复 key 与解析错误写入 errOut（format 串逐字一致：
//   "Parse error : line:%d reason:%d"
//   "Duplicated key : line:%d key:%d" )。
// 行终结：'\n' 直接进下一行；'\r\n' 连续则一并消费（ORIG i++ 补偿）。
// i==len 时对最后一行就地 NUL 终结后解析。遍历边界 i<=len（setle 条件）。
// ---------------------------------------------------------------------------
bool StringResource::loadFromString(char* text, int length, CharString* error)
{
    m_strings.clear();

    int lineNo = 0;                 // -0x20
    char* lineStart = text;         // -0x1c
    int i = 0;                      // -0x18
    while (i <= length)
    {
        const int c = static_cast<int>(text[i]);
        if (c == '\n')
        {
            text[i] = '\0';
        }
        else if (c == '\r')
        {
            text[i] = '\0';
            if (text[i + 1] == '\n')
                ++i;                                // ORIG 双字节补偿
        }
        else if (i < length)
        {
            ++i;
            continue;
        }
        else
        {
            // i==length 且非换行：最后一行截断（ORIG 尾部 movb $0）
            text[length] = '\0';
        }

        // —— 解析本行（lineNo 与 ORIG 行号语义：++ 于行末消费后）——
        lineNo += 1;
        int key = 0;
        char* val = 0;
        const int r = parseStringLine(lineStart, &key, &val);
        if (r != 0)
        {
            if (error)
                *error = CharString().format(
                    "Parse error : line:%d reason:%d", lineNo, r);
            return false;                            // ORIG 返回 $0
        }
        if (key != 0)
        {
            CharString v(val);
            if (!add(key, v) && error)
                *error = CharString().format(
                    "Duplicated key : line:%d key:%d", lineNo, key);
        }

        lineStart = text + i + 1;
        ++i;                                         // 移入下一行起始
    }
    return true;
}

// ---------------------------------------------------------------------------
// loadFromResource —— ORIG T 0x08ad4dfa（size 0x0a）：空函数（push ebp /
// mov $0,%eax / pop ebp / ret）——真实 ORIG 即恒返 false（资源型装载入口
// 在 ORIG 中未启用），转写为空实现。签名 (void*, int, CharString*)。
// ---------------------------------------------------------------------------
bool StringResource::loadFromResource(void*, int, CharString*)
{
    return false;
}

// ---------------------------------------------------------------------------
// loadFromBinary —— ORIG T 0x08ad4e04。
// 分支一（SC01）：magic "S","C","0","1"；origSize=*(data+4)；payload 取
// round4(size+3) 字节 new[]；running-key XOR 解密（k 初值 0x5819af17，
// 每 dword：v^=k; k^=v）；convertFromUtf8(payload, origSize, ws) →
// convertToCharString(ws) 赋值。
// 分支二（UTF-8 BOM EF BB BF）：convertFromUtf8(data+3, size-3, ws)。
// 分支三（UTF-16LE BOM FF FE）：WideString(data+2, (size-2)/2)。
// 默认分支：CharString(data, size)。之后统一
// loadFromString(cs.c_str(), cs.length(), err)，透传结果。
// ---------------------------------------------------------------------------
bool StringResource::loadFromBinary(const unsigned char* data, int size, CharString* error)
{
    CharString content;                              // -0x48

    if (size > 4 && data[0] == 'S' && data[1] == 'C' &&
        data[2] == '0' && data[3] == '1')
    {
        const int origSize =
            *reinterpret_cast<const int*>(data + 4);       // -0x2c ORIG data+4 dword
        const unsigned int payloadBytes =
            static_cast<unsigned int>((size + 3) & ~3);   // round4(size+3)

        char* payload = new char[payloadBytes];
        memcpy(payload, data + 8, payloadBytes);     // ORIG 源 data+8

        unsigned int k = 0x5819af17u;
        for (unsigned int wi = 0; wi < payloadBytes / 4; ++wi)
        {
            unsigned int* w =
                reinterpret_cast<unsigned int*>(payload + wi * 4);
            *w ^= k;
            k ^= *w;
        }

        WideString ws;
        const bool converted = convertFromUtf8(payload, origSize, &ws);
        delete[] payload;
        if (!converted)
            return false;                             // ORIG 失败路径返 0
        content = convertToCharString(ws);
    }
    else if (size > 3 && data[0] == '\xef' &&
             data[1] == '\xbb' && data[2] == '\xbf')
    {
        WideString ws;
        if (!convertFromUtf8(reinterpret_cast<const char*>(data + 3), size - 3, &ws))
            return false;
        content = convertToCharString(ws);
    }
    else if (size > 2 && data[0] == '\xff' && data[1] == '\xfe')
    {
        // UTF-16LE → wchar_t(i386 上同为 4 字节 UCS-2 语义槽)——ORIG 经
        // WideStringC1EPKwi 直接借字节；重建按小端重组。
        const int wchCount = (size - 2) / 2;
        std::vector<wchar_t> wbuf(wchCount + 1);
        for (int wi = 0; wi < wchCount; ++wi)
            wbuf[wi] = static_cast<wchar_t>(
                static_cast<unsigned char>(data[2 + wi * 2]) |
                (static_cast<unsigned char>(data[3 + wi * 2]) << 8));
        wbuf[wchCount] = L'\0';
        WideString ws(&wbuf[0], wchCount);
        content = convertToCharString(ws);
    }
    else
    {
        content = CharString(reinterpret_cast<const char*>(data), size);
    }

    const bool result =
        loadFromString(const_cast<char*>(content.c_str()),
                       content.length(), error);
    return result;
}

// ---------------------------------------------------------------------------
// loadFromFile —— ORIG T 0x08ad4ce4：fopen(path,"rb")（模式串 @0x8e32c38）；
// 失败且 errOut!=0 → *errOut="Cannot open file"@0x8e32c3b 并返回 0。
// 成功：fseek end/ftell/回 0 → fread 整体 → fclose →
// loadFromBinary(buf,size,err) → delete[]，透传结果（size<0 失败返 0）。
// ---------------------------------------------------------------------------
bool StringResource::loadFromFile(const char* path, CharString* error)
{
    FILE* fp = fopen(path, "rb");
    if (fp == NULL)
    {
        if (error)
            *error = "Cannot open file";
        return false;
    }
    fseek(fp, 0, SEEK_END);
    const long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize < 0)
    {
        fclose(fp);
        return false;
    }
    const int size = static_cast<int>(fileSize);

    char* buf = new char[size];
    fread(buf, 1, static_cast<size_t>(size), fp);
    fclose(fp);

    const bool result = loadFromBinary(reinterpret_cast<const unsigned char*>(buf), size, error);
    delete[] buf;
    return result;
}

// ---------------------------------------------------------------------------
// app::stringResource —— ORIG T 0x08ad5439：guard 单例 + atexit 析构注册。
// C++98 局部静态生命周期与 ORIG guard/atexit 组合等价。
// ---------------------------------------------------------------------------
namespace app {

StringResource* stringResource()
{
    static StringResource sr;
    return &sr;
}

}  // namespace app

// STRING_FILE —— ORIG 单元内静态 _ZL11STRING_FILE（0x093f567c），恒 0。
static const char* STRING_FILE = 0;

// ---------------------------------------------------------------------------
// loadString —— ORIG T 0x08ac35cc：stringResource()->loadFromFile(path,err)。
// ---------------------------------------------------------------------------
bool loadString(CharString* error)
{
    return app::stringResource()->loadFromFile(STRING_FILE, error);
}

// ---------------------------------------------------------------------------
// 编码原语。
//
// convertToCharString(PKwj) —— ORIG T 0x08adf57f：wcstombs(NULL,src,0) 求长；
// 返回 (size_t)-1 或 0 → 返回空 CharString；否则 CharStringData::create(n)
// 缓冲内 wcstombs 写入并以 CharString(CharStringData*) 归还。
// convertToCharString(PKw)  —— ORIG T 0x08adf54c：转发 PKwj（ORIG 经单元内
// 静态哨兵长度上界，行为等价于以 NUL 定界求长）。
// convertToCharString(RK10WideString) —— ORIG T 0x08adf624：sret，转发 c_str()。
// ---------------------------------------------------------------------------
CharString convertToCharString(const wchar_t* src, unsigned int srcLength)
{
    (void)srcLength;
    const std::size_t n = wcstombs(NULL, src, 0);
    if (n == static_cast<std::size_t>(-1) || n == 0)
        return CharString();
    CharStringData* data = CharStringData::create(static_cast<int>(n));
    wcstombs(data->getBuffer(), src, n + 1);
    return CharString(data);
}

CharString convertToCharString(const wchar_t* src)
{
    return convertToCharString(src, 0);
}

CharString convertToCharString(const WideString& src)
{
    return convertToCharString(src.c_str());
}

// ---------------------------------------------------------------------------
// convertFromUtf8 —— ORIG T 0x08adf893（size 0x1fd）。两遍式 UTF-8→wchar_t：
// pass1 统计宽字符数（≤0x7f:1 / ≤0xdf:2 / ≤0xef:3 / 其余:4；
// 截断序列 → 返回 0）；count==0 → *out=L"" 返 1；否则 createTerminated 后逐字
// 合成输出，最后 *out = tmp 返 1。合成移位与 ORIG 一致：
//   cp=(b&0x1f)<<6|(b1&0x3f)；cp=(b&0x0f)<<12|(b1&0x3f)<<6|(b2&0x3f)；4 字节同理。
// ---------------------------------------------------------------------------
bool convertFromUtf8(const char* src, int srcLength, WideString* out)
{
    int wideCount = 0;                        // -0x24
    int i = 0;                                // -0x18
    while (i < srcLength)
    {
        const unsigned char b = static_cast<unsigned char>(src[i]);
        if ((b & 0x80) == 0)
        {
            wideCount += 1;
            i += 1;
        }
        else if (b <= 0xdf)
        {
            if (i + 1 >= srcLength)
                return false;
            wideCount += 1;
            i += 2;
        }
        else if (b <= 0xef)
        {
            if (i + 2 >= srcLength)
                return false;
            wideCount += 1;
            i += 3;
        }
        else
        {
            if (i + 3 >= srcLength)
                return false;
            wideCount += 1;
            i += 4;
        }
    }
    if (wideCount == 0)
    {
        *out = L"";                           // ORIG operator=(wchar_t const*)
        return true;
    }

    WideStringData* data = WideStringData::createTerminated(wideCount);
    wchar_t* dst = data->getBuffer();
    i = 0;
    while (i < srcLength)
    {
        const unsigned char b = static_cast<unsigned char>(src[i]);
        if ((b & 0x80) == 0)
        {
            *dst++ = static_cast<wchar_t>(b);
            i += 1;
        }
        else if (b <= 0xdf)
        {
            *dst++ = static_cast<wchar_t>(((b & 0x1f) << 6) |
                                           (src[i + 1] & 0x3f));
            i += 2;
        }
        else if (b <= 0xef)
        {
            *dst++ = static_cast<wchar_t>(((b & 0x0f) << 12) |
                                           ((src[i + 1] & 0x3f) << 6) |
                                           (src[i + 2] & 0x3f));
            i += 3;
        }
        else
        {
            *dst++ = static_cast<wchar_t>(((b & 0x07) << 18) |
                                           ((src[i + 1] & 0x3f) << 12) |
                                           ((src[i + 2] & 0x3f) << 6) |
                                           (src[i + 3] & 0x3f));
            i += 4;
        }
    }
    WideString tmp(data);
    *out = tmp;
    return true;
}

// ---------------------------------------------------------------------------
// app::getStr —— ORIG T 0x08ad54cc：get(id) 命中 → 返回表项拷贝；
// 未命中 → CharString::format("getStr(%d) == none", id)。
// ---------------------------------------------------------------------------
CharString app::getStr(int id)
{
    const CharString* entry = 0;
    if (app::stringResource()->get(id, &entry))
        return *entry;
    return CharString::format("getStr(%d) == none", id);
}

// ---------------------------------------------------------------------------
// CStringManager::getStr —— ORIG T 0x08ac35f8。
// is_sr_loaded（ORIG _ZL12is_sr_loaded @0x0950189a）未置位时先加载一次；
// 加载失败返回 NULL（ORIG ebx=0 路径）；命中取表项 c_str()；未命中 sprintf
// "getStr(%d) == none"（格式串 @0x8e2e793）到静态 errorMsg 缓冲并返回。
// errorMsg 对应 ORIG getStr 内静态缓冲（bss 块 0x508）。
// ---------------------------------------------------------------------------
static bool is_sr_loaded = false;

char* CStringManager::getStr(int id)
{
    static char errorMsg[0x50];   // ORIG getStr 内局部静态 _ZZN14CStringManager6getStrEiE8errorMsg
    if (!is_sr_loaded)
    {
        CharString err;
        const bool loaded = loadString(&err);
        is_sr_loaded = loaded;
    }
    if (!is_sr_loaded)
        return 0;

    const CharString* entry = 0;
    if (app::stringResource()->get(id, &entry))
        return const_cast<char*>(entry->c_str());

    sprintf(errorMsg, "getStr(%d) == none", id);
    return errorMsg;
}

// ---------------------------------------------------------------------------
// CStringManager::setStringResourceLoaded —— ORIG T 0x08ac36d2：置位即返回。
// ---------------------------------------------------------------------------
void CStringManager::setStringResourceLoaded()
{
    is_sr_loaded = true;
}
