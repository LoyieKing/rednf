#ifndef DBMW_SERVERXML_H_
#define DBMW_SERVERXML_H_

#include "DBMWCommon.h"




#include <map>
#include <string>
#include "tinyxml.h"

// dbmw 专用 CServerXml（无 auction 扩展成员，布局与 ORIG 一致：
// +0 m_cCharSet[5]、+8 TiXmlDocument(0x48)、+0x50 m_eCharSet、
// +0x54 xmlFileName_、+0x58/0x70/0x88/0xa0 四个 map）
namespace np_server_xml
{

enum _eCharSetType
{
    ENUM_CHARSET_KOR = 0,
    ENUM_CHARSET_CHN = 1,
    ENUM_CHARSET_JPN = 2,
    ENUM_CHARSET_USA = 3,
    ENUM_CHARSET_TWN = 4,
};

enum _eStringType
{
    ENUM_SERVER_STRING = 0,
    ENUM_EVENT_STRING_START = 1,
    ENUM_EVENT_STRING_END = 2,
};

class CServerXml
{
public:
    CServerXml();
    ~CServerXml();
    void InitString();
    void StrLoading();
    void StrLoading(std::string file);
    std::string GetServerString(int nTextID, bool* pbResult) const;
    std::string GetEventString(int nTextID, _eStringType type,
                               bool* pbResult) const;
    int GetEventRGBA(int nEventID) const;
    void CharsetInit(TiXmlNode* pRootNode);
    void RGBALoad(int eventID, TiXmlNode* pRootNode);
    void EventLoad(TiXmlNode* pRootNode);
    void ProcessLoad(TiXmlNode* pRootNode);
    void StrPunish(int nTextID, const char* pText, _eStringType type);

private:
    char m_cCharSet[5];                    // +0
    TiXmlDocument m_cStrDoc;               // +8
    _eCharSetType m_eCharSet;              // +0x50
    std::string xmlFileName_;              // +0x54
    std::map<int, std::string> m_mapServerStr;      // +0x58
    std::map<int, std::string> m_mapEventStartStr;  // +0x70
    std::map<int, std::string> m_mapEventEndStr;    // +0x88
    std::map<int, int> m_map_EventRGBA;             // +0xa0
};

} // namespace np_server_xml

extern np_server_xml::CServerXml g_ServerString_;



#endif  // DBMW_SERVERXML_H_
