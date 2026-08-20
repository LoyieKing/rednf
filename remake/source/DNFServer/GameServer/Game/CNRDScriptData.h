#ifndef DNF_GAME_CNRD_SCRIPT_DATA_H_
#define DNF_GAME_CNRD_SCRIPT_DATA_H_

// ============================================================================
// df_game_r CNRDScriptData —— RDAR 脚本键值表
// 布局依据 ORIG 构造（0x0807f858，见 docs/class_func_reports/CNRDScriptData.md）:
//   +0x00 CharString                                        —— 已导入脚本路径
//   +0x04 StringUnorderedMap<CharStringHashTrait,int>       —— getInt 表
//   +0x24 StringUnorderedMap<CharStringHashTrait,float>     —— getFloat 表
//   +0x44 StringUnorderedMap<CharStringHashTrait,int>       —— getCLong 表
//   +0x64 StringUnorderedMap<CharStringHashTrait,float>     —— getCFloat 表
//   +0x84 StringUnorderedMap<CharStringHashTrait,std::string> —— getString 表
// 各 getXxx 未命中时返回默认值：int -> -1、float -> -1.0、string -> 错误串。
// ============================================================================

#include <string>
#include "StringTypes.h"
#include "HashMap.h"

class CNRDScriptData
{
public:
    CNRDScriptData();
    ~CNRDScriptData();

    int  import(const char* path);
    int  getInt(const char* key) const;
    int  getCLong(const char* key) const;
    float getFloat(const char* key) const;
    float getCFloat(const char* key) const;
    const char* getString(const char* key) const;

    void clear();
    void reset();
    void create();
    void destroy();

private:
    CharString m_path;                                            // +0x00
    StringUnorderedMap<CharStringHashTrait, int>    m_intMap;     // +0x04
    StringUnorderedMap<CharStringHashTrait, float>  m_floatMap;   // +0x24
    StringUnorderedMap<CharStringHashTrait, int>    m_clongMap;   // +0x44
    StringUnorderedMap<CharStringHashTrait, float>  m_cfloatMap;  // +0x64
    StringUnorderedMap<CharStringHashTrait, std::string> m_strMap; // +0x84
};

#endif  // DNF_GAME_CNRD_SCRIPT_DATA_H_