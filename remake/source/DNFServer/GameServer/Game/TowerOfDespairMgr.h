#ifndef GAME_TOWEROFDESPAIRMGR_H_
#define GAME_TOWEROFDESPAIRMGR_H_

// TowerOfDespair 域最小边界声明（供 CParty::_Send_TOD_APCInfo 等调用方编译；
// 完整类布局由后续 TOD 域批次按 docs/class_func_reports/TowerOfDespairMgr/ 扩展）。
// 依据报告：
//   docs/class_func_reports/TOD_Layer/TOD_Layer.md
//     —— ctor ORIG T 0x085fe7b4：对象仅 unsigned short（调用点栈槽 -0xe/-0x10 相邻
//        证实 sizeof==2），写入后 >100 时 cMyTrace 报错并回退为 1。
//   docs/class_func_reports/TowerOfDespairMgr/SendAPCInfo.md
//     —— ORIG T 0x08644338：转发 this+0x354 处 TowerOfDespair_APC_Mgr。
// GlobalData::s_pTowerOfDespairMgr（0941f754，void*）持有本类实例指针。

class CUser;

class TOD_Layer
{
public:
    TOD_Layer(unsigned short layer);       // ORIG T 0x085fe7b4（C1/C2 同址）

    unsigned short m_layer;                // +0x00
};

class TowerOfDespair_APC_Mgr
{
public:
    // ORIG T 0x085feee4 —— 定义待 TOD 域批次补齐（依赖成员
    // map<TOD_Layer, SendingAPC_Info> 与 SendingAPC_Info::Send_APC_Info/
    // get_user_charac_no；报告 docs/class_func_reports/TowerOfDespair_APC_Mgr/
    // Send_APC_Info.md 与 SendingAPC_Info/ 已核对，链路见交付说明）。
    void Send_APC_Info(TOD_Layer const& layer, CUser* user);
};

class TowerOfDespairMgr
{
public:
    void SendAPCInfo(TOD_Layer const& layer, CUser* user);   // ORIG T 0x08644338

    char m_pad[0x354];                     // +0x000..+0x353（本批次不建模）
    TowerOfDespair_APC_Mgr* m_apcMgr;      // +0x354
};

#endif  // GAME_TOWEROFDESPAIRMGR_H_
