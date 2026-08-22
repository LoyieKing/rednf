#ifndef DNFLEXWRAPPER_H
#define DNFLEXWRAPPER_H

#include <string>

#include "DNFLex.h"
#include "CompiledDNFLex.h"

class MySQL;

bool loadRDARScriptFile(const char* directory, const char* path) asm("_Z18loadRDARScriptFilePKcS0_");
void unloadRDARScriptFile() asm("_Z20unloadRDARScriptFilev");
bool ScanType(std::string& token, bool consume) asm("_Z8ScanTypeRSsb");
bool ScanInt(int* value) asm("_Z7ScanIntPi");
int ScanInt(bool* ok) asm("_Z7ScanIntPb");
float ScanFloat(float* value) asm("_Z9ScanFloatPf");
float ScanFloat(bool* ok) asm("_Z9ScanFloatPb");
bool ScanStr(std::string* value) asm("_Z7ScanStrPSs");
void GetEscapeString(MySQL* db, const char* src, char* dst);

// ============================================================================
// DNFLexWrapper（_ZTV13DNFLexWrapper：4 条目 = D1/D0 两槽，仅有虚析构）。
// 字段布局依据 ORIG DNFLexWrapper::DNFLexWrapper（0x8aced60）：
//   +0x00 vptr   +0x04 lastToken_   +0x08 lastIntValue_
//   +0x0c lastFloatValue_           +0x10 lastStringValue_   +0x14 lexType_
//   （lexType_：0=NORMAL 文本流 / 1=COMPILED 编译态）
//   +0x18 dnfLex_（DNFLex 子对象，0x40a4）/+0x40bc compiledDnfLex_
//   （CompiledDNFLex 子对象，0x204c）/+0x6108 LexerError_ / +0x610c LoadStream_
//   总大小 0x6110。
// ============================================================================

class DNFLexWrapper
{
public:
    enum ENUM_LEX_TYPE
    {
        LEX_TYPE_NORMAL   = 0,
        LEX_TYPE_COMPILED = 1
    };

    DNFLexWrapper();                 // 0x8aced60 C1/C2
    virtual ~DNFLexWrapper();        // 0x8acecf0 D1/D2, 0x8aced40 D0

    // 取词
    int  getPairInt();               // 0x8ace630
    char* getPairStr();              // 0x8ace5a0
    std::string getLastTokenDataAsString();  // 0x8ace4d0
    bool setStringTableFromBuffer(const unsigned char* buffer, int size);  // 0x8ace560
    bool setStringTableFromFile(const char* fileName);                     // 0x8ace530
    void SetHandler(bool (*isDbcsLeadChar)(char), void (*lexerOutput)(const char*, int),
                    void (*lexerError)(const char*),
                    bool (*loadStream)(const char*, const char*, DNFLex::stream_data_t*));
                                                                           // 0x8acec80

    // stream 访问
    void* getInputStream();          // 0x8ace900（istream*）
    char* getInputStreamBuffer();    // 0x8ace970
    int   getInputStreamSize();      // 0x8ace930
    std::string getStreamName();     // 0x8ace8a0
    void  closeInputStream();        // 0x8acea70
    bool  setStringStreamData(DNFLex::stream_data_t* s, char* buffer,
                              unsigned int bufferSize);  // 0x8ace9b0
    bool  switchInputStream(const char* dir, const char* filename,
                            DNFLex::stream_data_t* s);   // 0x8aceaa0
    bool  switchChannelInputStream(const char* filename, DNFLex::stream_data_t* s);
                                                         // 0x8acf060
    bool  LoadStream(const char* dir, const char* filename,
                     DNFLex::stream_data_t* s);          // 0x8ace2b0
    bool  setFileStreamData(DNFLex::stream_data_t* s, const char* path);  // 0x8acede0

    // ---- 数据 ----
    int               lastToken_;       // +0x04（vptr 由编译器生成于 +0x00）
    int               lastIntValue_;    // +0x08
    float             lastFloatValue_;  // +0x0c
    char*             lastStringValue_; // +0x10
    int               lexType_;         // +0x14  ENUM_LEX_TYPE
    DNFLex            dnfLex_;          // +0x18（0x40a4）
    CompiledDNFLex    compiledDnfLex_;  // +0x40bc（0x204c）
    void*             lexerError_;      // +0x6108
    void*             loadStream_;      // +0x610c
    // 总大小 0x6110
};

#endif
