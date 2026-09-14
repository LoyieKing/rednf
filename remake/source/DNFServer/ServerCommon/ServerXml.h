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
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT)
    // ORIG df_auction_r / df_point_r 的 CServerXml 比 df_game_r 多这一套拍卖串接口：
    //   nm: np_server_xml::CServerXml::getAuctionString(int, bool&)   @0x0808ff70
    //       np_server_xml::CServerXml::auctionStringLoad(TiXmlNode*)  @0x0808fb50
    //   readelf: g_ServerString_ OBJECT 208 字节（含第 5 个 map<int,string> @+0xb8）
    //   InitString 实测 clear 5 个 map（+0x58/+0x70/+0x88/+0xa0/+0xb8），
    //   StrLoading(std::string) 实测在 ProcessLoad 之后调 auctionStringLoad。
    // 提交 1dc28fd43 为对齐 df_game_r 把这三项从本共享头里删了，导致 auction/point
    // 无法编译（AuctionString.cpp: 'CServerXml' has no member named 'getAuctionString'）。
    // 用服务宏隔离：game/其它服务走未定义的 false 分支，产物逐字节不变。
    std::string getAuctionString(int textNo, bool& result);
#endif
    void CharsetInit(TiXmlNode* pRootNode);
    void RGBALoad(int eventID, TiXmlNode* pRootNode);
    void EventLoad(TiXmlNode* pRootNode);
    void ProcessLoad(TiXmlNode* pRootNode);
    void StrPunish(int nTextID, const char* pText, _eStringType type);
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT)
    void auctionStringLoad(TiXmlNode* rootNode);
#endif

private:
    char m_cCharSet[5];
    TiXmlDocument m_cStrDoc;
    _eCharSetType m_eCharSet;
    std::string xmlFileName_;
    std::map<int, std::string> m_mapServerStr;
    std::map<int, std::string> m_mapEventStartStr;
    std::map<int, std::string> m_mapEventEndStr;
    std::map<int, int> m_map_EventRGBA;
#if defined(DNF_SVC_AUCTION) || defined(DNF_SVC_POINT)
    // ORIG df_auction_r/df_point_r 的第 5 个 map（offset +0xb8，对象共 208 字节）
    std::map<int, std::string> auctionString_;
#endif
};

} // namespace np_server_xml

extern np_server_xml::CServerXml g_ServerString_;

#endif // SERVERXML_H_
