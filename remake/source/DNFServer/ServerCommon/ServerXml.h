#ifndef SERVERXML_H_
#define SERVERXML_H_

#include <map>
#include <string>
#include "tinyxml.h"

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
    std::string GetEventString(int nTextID, _eStringType type, bool* pbResult) const;
    int GetEventRGBA(int nEventID) const;
    std::string getAuctionString(int textNo, bool& result);
    void CharsetInit(TiXmlNode* pRootNode);
    void RGBALoad(int eventID, TiXmlNode* pRootNode);
    void EventLoad(TiXmlNode* pRootNode);
    void ProcessLoad(TiXmlNode* pRootNode);
    void StrPunish(int nTextID, const char* pText, _eStringType type);
    void auctionStringLoad(TiXmlNode* rootNode);

private:
    char m_cCharSet[5];
    TiXmlDocument m_cStrDoc;
    _eCharSetType m_eCharSet;
    std::string xmlFileName_;
    std::map<int, std::string> m_mapServerStr;
    std::map<int, std::string> m_mapEventStartStr;
    std::map<int, std::string> m_mapEventEndStr;
    std::map<int, int> m_map_EventRGBA;
    std::map<int, std::string> auctionString_;
};

} // namespace np_server_xml

extern np_server_xml::CServerXml g_ServerString_;

#endif // SERVERXML_H_
