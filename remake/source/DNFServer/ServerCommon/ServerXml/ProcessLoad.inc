// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族），本件 2 处：
//   ORIG 0x080c6354 / 0x080c63ad: call *%edx（前置 mov (%eax),%edx）
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax
// 助记符序列与指令数全等（83/83），仅寄存器角色由后端决定 ⇒ 源码无解（N7 判词）。
// 修复路径 = 本 TU 路由到 C6446R（cc1plus 4.4.6；P230 补：4.4.6 间接 call 目标 = %edx）。
// 实测（同源码仅换后端）：ServerXml.cpp identical 69 / ae 88 / near 4 / diff 1
//   →  identical 69 / ae 92 / near 0 / diff 1（本件 NEAR→AE）。
// 路由表 dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落 *ServerCommon/ServerXml.cpp 路由。
// ============================================================================
void CServerXml::ProcessLoad(TiXmlNode* pRootNode)
{
    TiXmlNode* pRoot = (TiXmlNode*)0;
    pRoot = pRootNode->FirstChild("str");
    if (pRoot == (TiXmlNode*)0)
    {
        puts("[CServerXml] <str> Tag Skip!!");
    }
    else
    {
        pRoot = pRoot->FirstChild("string");
        if (pRoot == (TiXmlNode*)0)
        {
            puts("[CServerXml] <string> Tag Error");
            exit(-1);
        }
        for (; pRoot != (TiXmlNode*)0; pRoot = pRoot->NextSibling())
        {
            int un_ID = 0;
            char* strTemp = (char*)0;
            char* cSentence = (char*)0;
            TiXmlNode* cNode = (TiXmlNode*)0;
            pRoot->ToElement()->Attribute("id", &un_ID);
            cNode = pRoot->FirstChild(m_cCharSet);
            if (cNode == (TiXmlNode*)0)
            {
                printf("%s Tag Error\n", this);
                return;
            }
            cSentence = (char*)cNode->ToElement()->Attribute("text");
            StrPunish(un_ID, cSentence, ENUM_SERVER_STRING);
        }
    }
