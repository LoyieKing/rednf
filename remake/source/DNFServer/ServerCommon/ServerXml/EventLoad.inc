// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族），本件 3 处：
//   ORIG 0x080c604c / 0x080c60c8 / 0x080c610e: call *%edx（前置 mov (%eax),%edx）
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax
// 助记符序列与指令数全等（110/110），仅寄存器角色由后端决定 ⇒ 源码无解（N7 判词）。
// 修复路径 = 本 TU 路由到 C6446R（cc1plus 4.4.6；P230 补：4.4.6 间接 call 目标 = %edx）。
// 实测（同源码仅换后端）：ServerXml.cpp identical 69 / ae 88 / near 4 / diff 1
//   →  identical 69 / ae 92 / near 0 / diff 1（本件 NEAR→AE）。
// 路由表 dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落 *ServerCommon/ServerXml.cpp 路由。
// ============================================================================
void CServerXml::EventLoad(TiXmlNode* pRootNode)
{
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("event_str");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <event_str> Tag Skip!!");
    }
    else
    {
        pRoot = pRoot->FirstChild("event");
        if (pRoot == (TiXmlNode*)0)
        {
            puts("[CServerXml] <event> Tag Error");
            exit(-1);
        }
        for (; pRoot != (TiXmlNode*)0; pRoot = pRoot->NextSibling())
        {
            int un_ID = 0;
            char* strTemp = (char*)0;
            char* cSentence = (char*)0;
            TiXmlNode* cNode = (TiXmlNode*)0;
            pRoot->ToElement()->Attribute("id", &un_ID);
            RGBALoad(un_ID, pRoot);
            cNode = pRoot->FirstChild(m_cCharSet);
            if (cNode == (TiXmlNode*)0)
            {
                printf("%s Tag Error\n", this);
                exit(-1);
            }
            cSentence = (char*)cNode->ToElement()->Attribute("start_msg");
            StrPunish(un_ID, cSentence, ENUM_EVENT_STRING_START);
            cSentence = (char*)cNode->ToElement()->Attribute("end_msg");
            StrPunish(un_ID, cSentence, ENUM_EVENT_STRING_END);
        }
    }
