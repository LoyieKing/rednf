// ============================================================================
// [G12 lane · 2026-09-10 round5] verdict=NEAR（447 口径；ServerXml.cpp 4 件 NEAR 之一）
// 逐行 diff 的**全部**实质差异 = 间接 call 目标寄存器角色互换（N7 族），本件 4 处：
//   ORIG 0x080c61c3 / 0x080c61f0 / 0x080c621d / 0x080c624a: call *%edx（前置 mov (%eax),%edx）
//   我方(4.4.7 后端): mov (%eax),%eax / call *%eax
// 助记符序列与指令数全等（93/93），仅寄存器角色由后端决定 ⇒ 源码无解（N7 判词）。
// 修复路径 = 本 TU 路由到 C6446R（cc1plus 4.4.6；P230 补：4.4.6 间接 call 目标 = %edx）。
// 实测（同源码仅换后端）：ServerXml.cpp identical 69 / ae 88 / near 4 / diff 1
//   →  identical 69 / ae 92 / near 0 / diff 1（本件 NEAR→AE）。
// 路由表 dnf_game_cxx.sh 为 Main 独占 ⇒ 已上报 Main 落 *ServerCommon/ServerXml.cpp 路由。
// 注：p167 另有 3 条 C++0x 右值实例调用（_ZNSt4pairIiiEC1IRiS2_EEOT_OT0_ 等）在 ORIG 侧；
//     本件 .o 侧无该实例名 ⇒ 属 P93/模板发射口径，非语义缺口（N130 族）。
// ============================================================================
void CServerXml::RGBALoad(int eventID, TiXmlNode* pRootNode)
{
    TiXmlNode* cNode = (TiXmlNode*)0;
    cNode = pRootNode->FirstChild("color");
    if (cNode == (TiXmlNode*)0)
    {
        printf("%s Tag Error\n", this);
        exit(-1);
    }
    union uniRGBA
    {
        int color;
        char c[4];
    };
    uniRGBA color;
    color.color = 0;
    color.c[0] = (char)atoi(cNode->ToElement()->Attribute("red"));
    color.c[1] = (char)atoi(cNode->ToElement()->Attribute("green"));
    color.c[2] = (char)atoi(cNode->ToElement()->Attribute("blue"));
    color.c[3] = (char)atoi(cNode->ToElement()->Attribute("alpha"));
    m_map_EventRGBA.insert(std::pair<int, int>(eventID, color.color));
