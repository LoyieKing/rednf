#ifndef COMPILEDDNFLEX_H
#define COMPILEDDNFLEX_H

#include <deque>
#include <string>
#include "DNFLex.h"

class ScriptStringManager;

// ============================================================================
// CompiledDNFLex（_ZTV14CompiledDNFLex：4 条目 = D1/D0 两槽，仅有虚析构）。
// ORIG 地址与汇编证据见 docs/class_func_reports/CompiledDNFLex.md。
// 布局（0x204c 字节）：
//   +0x00 vptr（0x8e2ed20）
//   +0x04 offset_（char*） +0x08 lastToken_ +0x0c stringManagerDelete_（bool）
//   +0x10 stringManager_（ScriptStringManager*） +0x14 stringBuffer_
//   +0x18 tokenBuffer_[0x2000]
//   +0x2018 std::deque<stream_t>（_Deque_base：map/map_size + start/finish 迭代器）
//   +0x2040 allow_inclusion_ +0x2044 LexerError_ +0x2048 LoadStream_
// ============================================================================

class CompiledDNFLex
{
public:
    // stream_t（0x14 字节）：dir/filename 用真实 std::string（COW 4.4 布局），
    // size/buffer/offset 为读取位置。
    struct stream_t
    {
        std::string  dir;      // +0x00
        std::string  filename; // +0x04
        unsigned int size;     // +0x08
        char*        buffer;   // +0x0c
        char*        offset;   // +0x10

        stream_t() : dir(), filename(), size(0), buffer(0), offset(0) {}
        stream_t(const std::string& d, const std::string& f)
            : dir(d), filename(f), size(0), buffer(0), offset(0) {}
    };

    typedef void (*TLexerError)(const char*);
    typedef bool (*TLoadStream)(const char*, const char*, DNFLex::stream_data_t*);

    CompiledDNFLex();                       // 0x8ad07c0
    explicit CompiledDNFLex(ScriptStringManager* pStringManager);  // 0x8ad0580
    virtual ~CompiledDNFLex();              // 0x8ad0ed0 D1 / 0x8ad0f20 D0

    ScriptStringManager* getStringManager();  // 0x8ad0070
    void setStringManager(ScriptStringManager* pStringManager, bool allowOwner);  // 0x8ad0050
    stream_t* GetInputStream();             // 0x8acfea0
    void CloseInputStream();                // 0x8ad0e90
    bool PushInputStream(const char* dir, const char* filename,
                         char* buffer, unsigned int bufferSize);  // 0x8ad0a00
    bool SwitchInputStream(const char* dir, const char* filename,
                           char* buffer, unsigned int bufferSize);  // 0x8ad0f40
    void SetHandler(TLexerError lexerError, TLoadStream loadStream);  // 0x8acfdf0
    void EnableInclusion();                 // 0x8ad0090
    void DisableInclusion();                // 0x8ad0080
    bool IsStream();                        // 0x8acfe80
    int  GetOffset();                       // 0x8acff20
    std::string getStreamName();            // 0x8ad0290
    bool isEnd();                           // 0x8ad0000
    bool PopInputStream(bool pop_all);      // 0x8ad0c80
    void LexerError(const char* msg);       // 0x8acfe10
    bool LoadStream(const char* dir, const char* filename,
                    DNFLex::stream_data_t* s);        // 0x8acfe40
    int  getToken();                        // 0x8ad0fa0（DNFLEX_TOKEN）
    int  getInt(bool* result);              // 0x8acff70
    float getFloat(bool* result);           // 0x8acffc0
    bool getStr(std::string& stringBuffer); // 0x8ad00d0
    bool getStr(char* buffer, int maxSize); // 0x8ad0190
    char* getStr(bool* result);             // 0x8ad0250
    void create(bool createStringTable);    // 0x8ad03d0
    void destroy();                         // 0x8ad0370

    // ---- 数据 ----
    char*         offset_;           // +0x04（vptr 由编译器生成于 +0x00）
    int           lastToken_;        // +0x08
    bool          stringManagerDelete_;  // +0x0c
    ScriptStringManager* stringManager_; // +0x10
    char*         stringBuffer_;     // +0x14
    unsigned char tokenBuffer_[0x2000];  // +0x18
    // ---- std::deque<stream_t>（_Deque_base 布局 0x28 字节）@ +0x2018 ----
    std::deque<stream_t> streamStack_;   // +0x2018 .. +0x203f
    unsigned char flag2040_;         // +0x2040（allow_inclusion_）
    char          pad2041_[3];       // +0x2041..0x2043
    TLexerError   lexerError_;       // +0x2044
    TLoadStream   loadStream_;       // +0x2048
    // 总大小 0x204c
};

#endif  // COMPILEDDNFLEX_H
