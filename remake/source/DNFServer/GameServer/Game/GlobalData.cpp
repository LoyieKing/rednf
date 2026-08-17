// df_game_r GlobalData（D1 组）还原。
// 函数形态/字符串/池尺寸均取自 ORIG 反汇编与 Ghidra C。
// 外部类（后续批次）仅在本 TU 以最小 stub 声明，符号留给 ORIG/后续 TU。

#include "GlobalData.h"

#include <iostream>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <map>
#include <string>
#include <deque>
#include <math.h>

#include "CEnvironment.h"
#include "CRijndael.h"
#include "Cipher.h"
#include "BaseServerProxy.h"
#include "CHadesServerProxy.h"
#include "CaptchaGenData.h"
#include "CQueryCounter.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "CGuildServerProxy.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "CItemAmplifier.h"

// ============================================================================
// 依赖类最小 stub（符号/布局属后续批次；本 TU 只用于还原 GlobalData 函数）
// ============================================================================

typedef std::map<ENUM_DB_HANDLE_IDX, ENUM_SERVER_GROUP> DBGroupMap;
typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DBInfoMap;

class Thread
{
public:
    virtual ~Thread();
    virtual void Stop();
};

class CGlobalEffectManager
{
public:
    CGlobalEffectManager();
private:
    char m_pad[0x18];
};

class CTimeGate
{
public:
    CTimeGate();
private:
    char m_pad[0x1c];
};

namespace ARAD
{
class Arad_ServerStateManager
{
public:
    Arad_ServerStateManager();
private:
    char m_pad[0x34];
};
}

class PacketPool
{
public:
    PacketPool();
    ~PacketPool();
    pthread_t getThreadID();
    void setThreadID(unsigned long threadID);
private:
    char m_pad[8];
};

class DBThread : public Thread
{
public:
    DBThread();
    void setLogDB(bool isLogDB);
private:
    char m_pad[0x4c - 4];
};

class DispatchThread : public Thread
{
public:
    DispatchThread();
private:
    char m_pad[0xc - 4];
};

class NetworkThread : public Thread
{
public:
    NetworkThread();
private:
    char m_pad[0xc - 4];
};

class UdpThread : public Thread
{
public:
    UdpThread();
private:
    char m_pad[0xc - 4];
};

class MonitorTcpThread : public Thread
{
public:
    MonitorTcpThread();
private:
    char m_pad[0xc - 4];
};

class AcceptThread : public Thread
{
public:
    AcceptThread();
private:
    char m_pad[0x10 - 4];
};

template <class T> class CServerProxyMgr;

template <> class CServerProxyMgr<CStatisticServerProxy>
{
public:
    CServerProxyMgr();
    ~CServerProxyMgr();
private:
    char m_pad[0x20];
};

class CDoubleConnCheckServerProxy
{
public:
    CDoubleConnCheckServerProxy(std::string ip, int port);
    bool Init();
private:
    char m_pad[0x10];
};

class CSchoolServerProxy
{
public:
    CSchoolServerProxy(std::string ip, int port);
    bool Init();
private:
    char m_pad[0x10];
};

class CPCRoomServerProxy
{
public:
    CPCRoomServerProxy(char* ip, int port);
private:
    char m_pad[0x30e04];
};

class CCommunityServerProxy
{
public:
    CCommunityServerProxy(char* ip, int port);
private:
    char m_pad[0x30e04];
};

class CAuctionServerProxy
{
public:
    CAuctionServerProxy(char* ip, int port, char* name);
    ~CAuctionServerProxy();
private:
    char m_pad[0x30e10];
};

class CCeraAuctionServerProxy
{
public:
    CCeraAuctionServerProxy(char* ip, int port, char* name);
    ~CCeraAuctionServerProxy();
private:
    char m_pad[0x30e10];
};

class CPowerManager
{
public:
    CPowerManager();
    bool Init();
    static void LoadPowerWarInfo();
    static void LoadRankerInfo();
private:
    char m_pad[0x208];
};

class CFatigueBatteryHandle
{
public:
    CFatigueBatteryHandle();
private:
    char m_pad[1];
};

namespace secretshop
{
class CSecretShop
{
public:
    CSecretShop();
private:
    char m_pad[0x64];
};
}

class CmmChannelProxy
{
public:
    CmmChannelProxy(std::string ip, int port);
    bool Init();
private:
    char m_pad[0x80020];
};

namespace WongWork
{
class CLogGameChannel
{
public:
    CLogGameChannel();
    void StartLogTimer();
private:
    char m_pad[0x10];
};

class CCeraShop
{
public:
    CCeraShop();
private:
    char m_pad[8];
};

class CAutoPunishRuleBlackIPMgr
{
public:
    CAutoPunishRuleBlackIPMgr();
    void reset();
private:
    char m_pad[0x18];
};

class CAutoPunishRuleHackTypeMgr
{
public:
    CAutoPunishRuleHackTypeMgr();
    void reset();
private:
    char m_pad[0x70];
};

class CBlackIPMonitor
{
public:
    CBlackIPMonitor();
    void reset();
private:
    char m_pad[0x18];
};

class CSimpleSSO
{
public:
    CSimpleSSO();
    ~CSimpleSSO();
    void init(MySQL* mysql);
private:
    char m_pad[0xd0864];
};

class CSecurityCardCenter
{
public:
    CSecurityCardCenter();
    void init();
private:
    char m_pad[0x488];
};

namespace IPG
{
class CIPGHelper
{
public:
    CIPGHelper();
    ~CIPGHelper();
    int Initialize(unsigned int a, unsigned int b);
private:
    char m_pad[0x54];
};
}
}

class ItemVendingMachine
{
public:
    ItemVendingMachine();
    ~ItemVendingMachine();
private:
    char m_pad[0x20];
};

class Secu_DataControl
{
public:
    Secu_DataControl();
private:
    char m_pad[1];
};

class CSecu_ProtectionField
{
public:
    CSecu_ProtectionField();
private:
    char m_pad[0x18];
};

class RefPvpGrade
{
public:
    RefPvpGrade();
    ~RefPvpGrade();
private:
    char m_pad[0x14c];
};

class CDBConnector
{
public:
    CDBConnector(DBMgr* dbMgr);
    ~CDBConnector();
    bool Init(STDBConnInfo& info);
    bool GetConnInfo(DBGroupMap& groupMap, DBInfoMap& infoMap);
    bool Connect(DBInfoMap infoMap);
    bool ConnectWithServerGroup(DBInfoMap infoMap);
private:
    char m_pad[0x16c];
};

namespace sync_script
{
class CSyncScript
{
public:
    static CSyncScript* GetInstanceSyncScript();
    static void SetEnable(bool enable);
    static bool IsEnable();
};
}

class CSyncSlangFilter
{
public:
    static CSyncSlangFilter* GetInstance();
    static bool AddSlangList();
    static bool AddSlangListName();
    bool HasSlang(std::string& name);
    bool HasSlangName(std::string& name);
    static void FreeInstance();
};

class CGM_Manager
{
public:
    CGM_Manager();
    ~CGM_Manager();
private:
    char m_pad[0x3c];
};

class CStatisticMgr
{
public:
    CStatisticMgr();
private:
    char m_pad[0x30];
};

namespace break_away_prevent
{
class CBreakAwayPreventSystem
{
public:
    CBreakAwayPreventSystem();
    ~CBreakAwayPreventSystem();
private:
    char m_pad[0x32c];
};
}

namespace expert_job
{
class CExpertJobMgr
{
public:
    CExpertJobMgr();
    ~CExpertJobMgr();
private:
    char m_pad[4];
};
}

namespace village_object
{
class CVillageObjectMgr
{
public:
    CVillageObjectMgr();
    ~CVillageObjectMgr();
private:
    char m_pad[0x18];
};
}

namespace village_attacked
{
class CVillageMonsterMgr
{
public:
    CVillageMonsterMgr();
    ~CVillageMonsterMgr();
private:
    char m_pad[0x24];
};

class CRevengeDungeon
{
public:
    CRevengeDungeon();
private:
    char m_pad[8];
};
}

class InstanceRentalSystem
{
public:
    InstanceRentalSystem();
    ~InstanceRentalSystem();
private:
    char m_pad[4];
};

class CDailyScheduleManager
{
public:
    CDailyScheduleManager();
    ~CDailyScheduleManager();
private:
    char m_pad[0xe4];
};

namespace online_preliminary
{
class COnlinePreliminaryTeamMgr
{
public:
    COnlinePreliminaryTeamMgr();
    ~COnlinePreliminaryTeamMgr();
private:
    char m_pad[0x1c];
};
}

namespace game_master
{
class CGameMasterMgr
{
public:
    CGameMasterMgr();
private:
    char m_pad[0x40];
};
}

class CBusinessImpl
{
public:
    CBusinessImpl();
    ~CBusinessImpl();
private:
    char m_pad[4];
};

class TimerUpdatePvPGrade
{
public:
    static void registNextTimer(long long sec);
};

class Timer_GenerateCaptchaData
{
public:
    static void RegistNextTimer(int sec);
};

class DB_ReloadAutoPunishRuleBackIP
{
public:
    static void makeRequest();
};

class DB_ReloadAutoPunishRuleHackType
{
public:
    static void makeRequest();
};

class DB_LoadCleanPadPoint
{
public:
    static void makeRequest();
};

class DB_BlackIPMonitor
{
public:
    static void makeRequest();
};

void* CreateAntiServerObj(const char* path);

namespace StackableAction
{
class Action
{
public:
    Action();
private:
    char m_pad[0x30];
};
}

namespace expert_extraction
{
class CExtractionMgr
{
public:
    CExtractionMgr();
private:
    char m_pad[0xc];
};
}

class NPC_Teleport
{
public:
    NPC_Teleport();
private:
    char m_pad[0x14];
};

class User_Teleport
{
public:
    User_Teleport();
private:
    char m_pad[0x14];
};

namespace random_option
{
class CRandomOptionItemHandle
{
public:
    CRandomOptionItemHandle();
private:
    char m_pad[0x18];
};
}

namespace ARAD
{
template <class T> class Singleton
{
public:
    static T* Get();
};

template <class T> T* Singleton<T>::Get()
{
    static T inst;  // TODO(后续批次)：单例对象真实构造/销毁由对应类批次接管
    return &inst;
}
}

class ServiceRestrictManager
{
public:
    bool initialize();
    void savePolicy();
    void loadPolicy();
};

class RestrictGeolocation
{
public:
    bool initGeolocation();
};

class CHumanCertifier
{
public:
    static void init();
};

class GameWorld
{
public:
    int GetChannelType() const;
    bool IsIntegratedPvPBaseChannel() const;
};

GameWorld* G_GameWorld();

class CDBEnviroment : public CEnvironment
{
public:
    CDBEnviroment();
    ~CDBEnviroment();
private:
    char m_pad[8];   // ORIG：CEnvironment + 0x3e0 处成员/填充（0x3e8）
};

// ---- Captcha ----
struct CountryCode
{
    unsigned char idx_;           // +0x00（1 字节）
    static const CountryCode none;    // 09501a20
    static const CountryCode global;  // 09501a21
    static const CountryCode korea;   // 09501a22
    static const CountryCode japan;   // 09501a23
    static const CountryCode china;   // 09501a24
    static const CountryCode taiwan;  // 09501a25
    static const CountryCode usa;     // 09501a26

    bool operator==(CountryCode other) const;   // 0x08ad332e（按值，ORIG mangling eqES_）
};

const CountryCode CountryCode::none   = {0};
const CountryCode CountryCode::global = {1};
const CountryCode CountryCode::korea  = {2};
const CountryCode CountryCode::japan  = {3};
const CountryCode CountryCode::china  = {4};
const CountryCode CountryCode::taiwan = {5};
const CountryCode CountryCode::usa    = {6};

bool CountryCode::operator==(CountryCode other) const
{
    return idx_ == other.idx_;
}

// ---- XorRand4（ORIG 0x08adfa90；m_counter + 4 张字节替换表）----
class XorRand4
{
public:
    XorRand4() { m_counter = 0; }
    void seed(unsigned int s) { m_counter = s; }
    unsigned int getUInt32()
    {
        unsigned int v = m_counter;
        m_counter = v + 1;
        return kXorRandMsTable0[v & 0xff] ^ kXorRandMsTable1[(v >> 8) & 0xff] ^
               kXorRandMsTable2[(v >> 16) & 0xff] ^ kXorRandMsTable3[(v >> 24) & 0xff];
    }
    unsigned int operator()() { return getUInt32(); }
private:
    unsigned int m_counter;
};

class GreyImage
{
public:
    GreyImage();
    ~GreyImage();
    void init(int width, int height, unsigned char fill);
    void reset();
    int getWidth() const;
    int getHeight() const;
    void* getImage();
    void* getImage() const;
    unsigned char getAlpha(int x, int y);
    void setAlpha(int x, int y, unsigned char alpha);
    void setAlphaAsMax(int x, int y, unsigned char alpha);
    int sampleAlpha(int x, int y);
private:
    int m_width;          // +0x00
    int m_height;         // +0x04
    unsigned char* m_image;  // +0x08
};

// GreyImage 方法实现在 GameStubs.cpp（ORIG 中 GreyImage 独立 TU；GlobalData TU
// 仅保留声明，使 GenerateCaptchaData 的 EH 清理结构对齐 ORIG）。

#pragma pack(push, 2)
struct MSG_UDP_CAPCHA_CERTIFY
{
    unsigned short m_text[5];                        // +0x00
    int m_size;                                      // +0x0a
    unsigned char m_image[0x339e - 0xe];             // +0x0e
};
#pragma pack(pop)

// ---- drawLine（ORIG 0x080dea17，文件内静态）----
static void drawLine(GreyImage* img, int x1, int y1, int x2, int y2)
{
    if (x1 >= x2)
    {
        return;
    }
    int x = x1;
    int y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int err = dx / 2;
    int step = 1;
    if (y2 < y1)
    {
        step = -1;
        dy = -dy;
    }
    while (x < x2)
    {
        if (x != x1 || y != y1)
        {
            img->setAlphaAsMax(x, y, 0xff);
        }
        ++x;
        err -= dy;
        if (err < 0)
        {
            y += step;
            err += dx;
        }
    }
}

// ---- 字形渲染：A-Z 用内置 8x8 点阵；韩/日等码点用确定性伪字形。
//      ORIG 走 FreeType 栅格化；本环境无 FreeType/字体文件，算法等价占位。----
static void renderGlyph(unsigned short code, unsigned char* out, int& gw, int& gh)
{
    gw = 8;
    gh = 8;
    static const unsigned char font8x8[26][8] = {
        {0x18,0x3c,0x66,0x7e,0x66,0x66,0x66,0x00},  // A
        {0x7c,0x66,0x66,0x7c,0x66,0x66,0x7c,0x00},  // B
        {0x3c,0x66,0x60,0x60,0x60,0x66,0x3c,0x00},  // C
        {0x78,0x6c,0x66,0x66,0x66,0x6c,0x78,0x00},  // D
        {0x7e,0x60,0x60,0x7c,0x60,0x60,0x7e,0x00},  // E
        {0x7e,0x60,0x60,0x7c,0x60,0x60,0x60,0x00},  // F
        {0x3c,0x66,0x60,0x6e,0x66,0x66,0x3e,0x00},  // G
        {0x66,0x66,0x66,0x7e,0x66,0x66,0x66,0x00},  // H
        {0x3c,0x18,0x18,0x18,0x18,0x18,0x3c,0x00},  // I
        {0x1e,0x0c,0x0c,0x0c,0x0c,0x6c,0x38,0x00},  // J
        {0x66,0x6c,0x78,0x70,0x78,0x6c,0x66,0x00},  // K
        {0x60,0x60,0x60,0x60,0x60,0x60,0x7e,0x00},  // L
        {0x63,0x77,0x7f,0x6b,0x63,0x63,0x63,0x00},  // M
        {0x66,0x76,0x7e,0x7e,0x6e,0x66,0x66,0x00},  // N
        {0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00},  // O
        {0x7c,0x66,0x66,0x7c,0x60,0x60,0x60,0x00},  // P
        {0x3c,0x66,0x66,0x66,0x66,0x6c,0x36,0x00},  // Q
        {0x7c,0x66,0x66,0x7c,0x78,0x6c,0x66,0x00},  // R
        {0x3c,0x66,0x60,0x3c,0x06,0x66,0x3c,0x00},  // S
        {0x7e,0x18,0x18,0x18,0x18,0x18,0x18,0x00},  // T
        {0x66,0x66,0x66,0x66,0x66,0x66,0x3c,0x00},  // U
        {0x66,0x66,0x66,0x66,0x66,0x3c,0x18,0x00},  // V
        {0x63,0x63,0x63,0x6b,0x7f,0x77,0x63,0x00},  // W
        {0x66,0x66,0x3c,0x18,0x3c,0x66,0x66,0x00},  // X
        {0x66,0x66,0x66,0x3c,0x18,0x18,0x18,0x00},  // Y
        {0x7e,0x06,0x0c,0x18,0x30,0x60,0x7e,0x00}   // Z
    };
    if (code >= 'A' && code <= 'Z')
    {
        const unsigned char* g = font8x8[code - 'A'];
        for (int r = 0; r < 8; ++r)
        {
            for (int c = 0; c < 8; ++c)
            {
                out[r * 8 + c] = (g[r] & (1 << (7 - c))) ? 0xff : 0;
            }
        }
        return;
    }
    unsigned int seed = code * 2654435761u;
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            seed = seed * 1664525u + 1013904223u;
            out[r * 8 + c] = ((seed >> 16) & 1) ? 0xff : 0;
        }
    }
}

class CaptchaGen
{
public:
    struct ImageOption
    {
        ImageOption();
        int m_field0;    // +0x00 噪点圆数量
        int m_field4;    // +0x04 噪点圆最大半径
        int m_field8;    // +0x08 干扰线数量
        float m_fieldc;  // +0x0c 旋角下限
        float m_field10; // +0x10 旋角上限
        float m_field14; // +0x14 缩放下限
        float m_field18; // +0x18 缩放上限
        int m_field1c;   // +0x1c 字符纵向偏移范围
        float m_field20; // +0x20 字符推进系数
        float m_field24; // +0x24 波形频率下限
        float m_field28; // +0x28 波形频率上限
        float m_field2c; // +0x2c 波形幅度下限
        float m_field30; // +0x30 波形幅度上限
    };

    CaptchaGen();
    ~CaptchaGen();
    bool addFont(const char* font, int size);
    void generateText(CountryCode code, int len, unsigned short* out);
    void generateImage(int width, int height, const unsigned short* text, int len,
                       const ImageOption& opt, GreyImage* image);
private:
    static float getFloat(XorRand4& rng, float lo, float hi);
    void* m_library;             // +0x00（ORIG FT_Library；本还原中为句柄占位）
    std::vector<void*> m_faces;  // +0x04
    XorRand4 m_rng;              // +0x10
};

// CaptchaGen 实现（G7 验证码；ORIG 走 FreeType，本环境无 FreeType，按算法等价实现）
CaptchaGen::ImageOption::ImageOption()
{
    m_field0 = 100;
    m_field4 = 3;
    m_field8 = 2;
    m_fieldc = 0.8f;
    m_field10 = 1.3f;
    m_field14 = 0.0f;
    m_field18 = 0.5f;
    m_field1c = 8;
    m_field20 = 0.9f;
    m_field24 = 0.1f;
    m_field28 = 0.12f;
    m_field2c = 2.0f;
    m_field30 = 2.5f;
}

CaptchaGen::CaptchaGen()
{
    m_library = 0;
    m_rng.seed((unsigned int)time(0));
}

CaptchaGen::~CaptchaGen()
{
    for (size_t i = 0; i < m_faces.size(); ++i)
    {
        free(m_faces[i]);
    }
}

bool CaptchaGen::addFont(const char* font, int size)
{
    // ORIG：FT_New_Face -> FT_Set_Pixel_Sizes -> FT_Select_Charmap(UNICODE)，
    // 任一失败返回 false；成功后把 face 句柄存入 m_faces。
    // 本环境无 FreeType：校验字体文件可读，并把字体名句柄存入 m_faces。
    FILE* f = fopen(font, "rb");
    if (!f)
    {
        return false;
    }
    fclose(f);
    char* handle = (char*)malloc(strlen(font) + 1);
    if (!handle)
    {
        return false;
    }
    strcpy(handle, font);
    m_faces.push_back(handle);
    return true;
}

void CaptchaGen::generateText(CountryCode code, int len, unsigned short* out)
{
    const unsigned short* table = 0;
    unsigned int count = 0;
    if (code == CountryCode::korea)
    {
        table = kKoreanGeneral;
        count = sizeof(kKoreanGeneral) / sizeof(kKoreanGeneral[0]);
    }
    else if (code == CountryCode::japan)
    {
        table = kJapaneseHirakana;
        count = sizeof(kJapaneseHirakana) / sizeof(kJapaneseHirakana[0]);
    }
    else if (code == CountryCode::taiwan)
    {
        table = kTaiwanAlphabet;
        count = sizeof(kTaiwanAlphabet) / sizeof(kTaiwanAlphabet[0]);
    }
    else
    {
        return;
    }
    for (int i = 0; i < len; ++i)
    {
        out[i] = table[m_rng() % count];
    }
}

float CaptchaGen::getFloat(XorRand4& rng, float lo, float hi)
{
    unsigned int v = rng() % 0xffffu;
    return (float)v * (1.0f / 65536.0f) * (hi - lo) + lo;
}

void CaptchaGen::generateImage(int width, int height, const unsigned short* text, int len,
                               const ImageOption& opt, GreyImage* image)
{
    if (m_faces.empty())
    {
        return;
    }
    image->init(width, height, 0);

    // 噪点圆（ORIG 第一段循环）
    for (int i = 0; i < opt.m_field0; ++i)
    {
        int cx = m_rng() % (width - 10) + 5;
        int cy = m_rng() % (height - 10) + 5;
        int r = m_rng() % opt.m_field4 + 1;
        for (int dy = -r; dy <= r; ++dy)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                int v = ((dx * dx + dy * dy) * 0xff) / (r * r);
                if (v < 0xff)
                {
                    image->setAlphaAsMax(cx + dx, cy + dy,
                                         (unsigned char)((0xff - v) / r));
                }
            }
        }
    }

    // 干扰线（ORIG 第二段循环）
    for (int i = 0; i < opt.m_field8; ++i)
    {
        int x1 = m_rng() % 10 + 1;
        int y1 = m_rng() % height;
        int x2 = (width - m_rng() % 10) - 2;
        int y2 = m_rng() % height;
        drawLine(image, x1, y1, x2, y2);
    }

    // 字符渲染（ORIG 第三段循环：随机面/旋转/缩放 -> FT 栅格 -> 粘贴）
    int penX = 10;
    int penY = 2;
    for (int i = 0; i < len; ++i)
    {
        unsigned int faceIdx = m_rng() % m_faces.size();
        (void)faceIdx;
        (void)getFloat(m_rng, opt.m_fieldc, opt.m_field10);   // 旋角
        (void)getFloat(m_rng, opt.m_fieldc, opt.m_field10);
        (void)getFloat(m_rng, opt.m_field14, opt.m_field18);  // 缩放
        (void)getFloat(m_rng, opt.m_field14, opt.m_field18);
        (void)(m_rng() & 2);                                   // 镜像位
        (void)(m_rng() & 2);

        unsigned char glyph[8 * 8];
        int gw = 8;
        int gh = 8;
        renderGlyph(text[i], glyph, gw, gh);

        int yOff = m_rng() % opt.m_field1c;
        int cols = width - penX;
        if (cols > gw)
        {
            cols = gw;
        }
        for (int row = 0; row < gh; ++row)
        {
            int yy = penY + row + yOff;
            if (yy < 0 || yy >= height)
            {
                continue;
            }
            for (int c = 0; c < cols; ++c)
            {
                if (glyph[row * gw + c] != 0)
                {
                    image->setAlphaAsMax(penX + c, yy, glyph[row * gw + c]);
                }
            }
        }
        penX += (int)(opt.m_field20 * (float)gw);
    }

    // 波形扭曲（ORIG 第四段：cos/sin 16.16 定点偏移 + 双线性采样）
    GreyImage tmp;
    tmp.init(width, height, 0);
    float freq = getFloat(m_rng, opt.m_field24, opt.m_field28);
    float amp = getFloat(m_rng, opt.m_field2c, opt.m_field30);
    std::vector<int> cosTbl(width);
    std::vector<int> sinTbl(height);
    for (int x = 0; x < width; ++x)
    {
        cosTbl[x] = (int)(65536.0f * cosf((float)x * freq) * amp);
    }
    for (int y = 0; y < height; ++y)
    {
        sinTbl[y] = (int)(65536.0f * sinf((float)y * freq) * amp);
    }
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            tmp.setAlpha(x, y, (unsigned char)image->sampleAlpha(
                x * 65536 + sinTbl[y], y * 65536 + cosTbl[x]));
        }
    }
    memcpy(image->getImage(), tmp.getImage(), (size_t)width * height);
}

namespace WongWork
{
class CCapchaDataManager
{
public:
    void push(const MSG_UDP_CAPCHA_CERTIFY& msg);
private:
    std::deque<MSG_UDP_CAPCHA_CERTIFY> m_queue;   // ORIG：deque（0x28 字节）
};
}

WongWork::CCapchaDataManager GlobalData::s_capchaDataManager;
void WongWork::CCapchaDataManager::push(const MSG_UDP_CAPCHA_CERTIFY& msg)
{
    // ORIG 0x08299f3a：push_back；超过 0x28 条则 pop_front
    m_queue.push_back(msg);
    if (m_queue.size() > 0x28)
    {
        m_queue.pop_front();
    }
}

// ============================================================================
// 依赖类 stub 实现（GlobalData.cpp 自含声明的占位类；G2/G4/G6/G7/G8 批次接管）
// ============================================================================

// ---- Thread / 线程族 ----
Thread::~Thread() {}  // TODO(后续批次)
void Thread::Stop() {}  // TODO(后续批次)

DBThread::DBThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
void DBThread::setLogDB(bool) {}  // TODO(后续批次)
DispatchThread::DispatchThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
NetworkThread::NetworkThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
UdpThread::UdpThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
MonitorTcpThread::MonitorTcpThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
AcceptThread::AcceptThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)

// ---- GlobalData::Init 直接 new 的基础对象 ----
CGlobalEffectManager::CGlobalEffectManager() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CTimeGate::CTimeGate() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
ARAD::Arad_ServerStateManager::Arad_ServerStateManager()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)

PacketPool::PacketPool() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
PacketPool::~PacketPool() {}  // TODO(后续批次)
pthread_t PacketPool::getThreadID() { return 0; }  // TODO(后续批次)
void PacketPool::setThreadID(unsigned long) {}  // TODO(后续批次)

// ---- CServerProxyMgr<CMonitorServerProxy>/<CGuildServerProxy> ----
// 由 CServerProxyMgr.h 真实模板在下方 new/delete 处隐式实例化
// （ORIG 弱符号 0x82b314e/0x82b3212 等：+0x10 内嵌代理 + 字段清零 + Init），
// 不再显式特化。CStatisticServerProxy 属后续批次，保留 stub。
CServerProxyMgr<CStatisticServerProxy>::CServerProxyMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CServerProxyMgr<CStatisticServerProxy>::~CServerProxyMgr() {}  // TODO(后续批次)

// ---- ServerProxy（BaseServerProxy/CHadesServerProxy 属 E1 批次，见
//      BaseServerProxy.cpp/CHadesServerProxy.cpp）----
CSchoolServerProxy::CSchoolServerProxy(std::string, int)
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
bool CSchoolServerProxy::Init() { return true; }  // TODO(后续批次)
CPCRoomServerProxy::CPCRoomServerProxy(char*, int)
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CCommunityServerProxy::CCommunityServerProxy(char*, int)
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CPowerManager::CPowerManager() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
bool CPowerManager::Init() { return true; }  // TODO(后续批次)
void CPowerManager::LoadPowerWarInfo() {}  // TODO(后续批次)
void CPowerManager::LoadRankerInfo() {}  // TODO(后续批次)

CFatigueBatteryHandle::CFatigueBatteryHandle() { m_pad[0] = 0; }  // TODO(后续批次)
CmmChannelProxy::CmmChannelProxy(std::string, int)
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
bool CmmChannelProxy::Init() { return true; }  // TODO(后续批次)

// ---- WongWork ----
WongWork::CGMAccounts::CGMAccounts() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
WongWork::CGMAccounts::~CGMAccounts() {}  // TODO(后续批次)
bool WongWork::CGMAccounts::isGM(unsigned int) { return false; }  // TODO(后续批次)
WongWork::CLogGameChannel::CLogGameChannel()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CLogGameChannel::StartLogTimer() {}  // TODO(后续批次)
WongWork::CCeraShop::CCeraShop() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
WongWork::CAutoPunishRuleBlackIPMgr::CAutoPunishRuleBlackIPMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CAutoPunishRuleBlackIPMgr::reset() {}  // TODO(后续批次)
WongWork::CAutoPunishRuleHackTypeMgr::CAutoPunishRuleHackTypeMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CAutoPunishRuleHackTypeMgr::reset() {}  // TODO(后续批次)
WongWork::CBlackIPMonitor::CBlackIPMonitor()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CBlackIPMonitor::reset() {}  // TODO(后续批次)
WongWork::CSimpleSSO::CSimpleSSO() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
WongWork::CSimpleSSO::~CSimpleSSO() {}  // TODO(后续批次)
void WongWork::CSimpleSSO::init(MySQL*) {}  // TODO(后续批次)
WongWork::CSecurityCardCenter::CSecurityCardCenter()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CSecurityCardCenter::init() {}  // TODO(后续批次)
WongWork::IPG::CIPGHelper::CIPGHelper()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
WongWork::IPG::CIPGHelper::~CIPGHelper() {}  // TODO(后续批次)
int WongWork::IPG::CIPGHelper::Initialize(unsigned int, unsigned int)
{
    return 0;  // TODO(后续批次)
}

// ---- 全局业务对象 ----
ItemVendingMachine::ItemVendingMachine()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
ItemVendingMachine::~ItemVendingMachine() {}  // TODO(后续批次)
Secu_DataControl::Secu_DataControl() { m_pad[0] = 0; }  // TODO(后续批次)
CSecu_ProtectionField::CSecu_ProtectionField()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
RefPvpGrade::RefPvpGrade() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
RefPvpGrade::~RefPvpGrade() {}  // TODO(后续批次)

// ---- CDBConnector ----
CDBConnector::CDBConnector(DBMgr*) { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CDBConnector::~CDBConnector() {}  // TODO(后续批次)
bool CDBConnector::Init(STDBConnInfo&) { return true; }  // TODO(后续批次)
bool CDBConnector::GetConnInfo(DBGroupMap&, DBInfoMap&) { return true; }  // TODO(后续批次)
bool CDBConnector::Connect(DBInfoMap) { return true; }  // TODO(后续批次)
bool CDBConnector::ConnectWithServerGroup(DBInfoMap) { return true; }  // TODO(后续批次)

// ---- sync_script ----
namespace sync_script
{
CSyncScript* CSyncScript::GetInstanceSyncScript()
{
    static CSyncScript inst;  // TODO(后续批次)
    return &inst;
}
void CSyncScript::SetEnable(bool) {}  // TODO(后续批次)
bool CSyncScript::IsEnable() { return false; }  // TODO(后续批次)
}

// ---- CSyncSlangFilter（GetInstance/HasSlang/HasSlangName 同时服务 CDataManager）----
CSyncSlangFilter* CSyncSlangFilter::GetInstance()
{
    static CSyncSlangFilter inst;  // TODO(后续批次)
    return &inst;
}
bool CSyncSlangFilter::AddSlangList() { return true; }  // TODO(后续批次)
bool CSyncSlangFilter::AddSlangListName() { return true; }  // TODO(后续批次)
bool CSyncSlangFilter::HasSlang(std::string&) { return false; }  // TODO(后续批次)
bool CSyncSlangFilter::HasSlangName(std::string&) { return false; }  // TODO(后续批次)
void CSyncSlangFilter::FreeInstance() {}  // TODO(后续批次)

// ---- 事件/GM/统计 ----
CEventManager::CEventManager() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
Gm_List::Gm_List() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CGM_Manager::CGM_Manager() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CGM_Manager::~CGM_Manager() {}  // TODO(后续批次)
CStatisticMgr::CStatisticMgr() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)

// ---- 后续批次业务模块 ----
break_away_prevent::CBreakAwayPreventSystem::CBreakAwayPreventSystem()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
break_away_prevent::CBreakAwayPreventSystem::~CBreakAwayPreventSystem() {}  // TODO(后续批次)
expert_job::CExpertJobMgr::CExpertJobMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
expert_job::CExpertJobMgr::~CExpertJobMgr() {}  // TODO(后续批次)
village_object::CVillageObjectMgr::CVillageObjectMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
village_object::CVillageObjectMgr::~CVillageObjectMgr() {}  // TODO(后续批次)
village_attacked::CVillageMonsterMgr::CVillageMonsterMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
village_attacked::CVillageMonsterMgr::~CVillageMonsterMgr() {}  // TODO(后续批次)
village_attacked::CRevengeDungeon::CRevengeDungeon()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
InstanceRentalSystem::InstanceRentalSystem()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
InstanceRentalSystem::~InstanceRentalSystem() {}  // TODO(后续批次)
CDailyScheduleManager::CDailyScheduleManager()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CDailyScheduleManager::~CDailyScheduleManager() {}  // TODO(后续批次)
online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
online_preliminary::COnlinePreliminaryTeamMgr::~COnlinePreliminaryTeamMgr() {}  // TODO(后续批次)
game_master::CGameMasterMgr::CGameMasterMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CBusinessImpl::CBusinessImpl() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CBusinessImpl::~CBusinessImpl() {}  // TODO(后续批次)

// ---- 定时器/DB 请求/反外挂 ----
void TimerUpdatePvPGrade::registNextTimer(long long) {}  // TODO(后续批次)
void Timer_GenerateCaptchaData::RegistNextTimer(int) {}  // TODO(后续批次)
void DB_ReloadAutoPunishRuleBackIP::makeRequest() {}  // TODO(后续批次)
void DB_ReloadAutoPunishRuleHackType::makeRequest() {}  // TODO(后续批次)
void DB_LoadCleanPadPoint::makeRequest() {}  // TODO(后续批次)
void DB_BlackIPMonitor::makeRequest() {}  // TODO(后续批次)
void* CreateAntiServerObj(const char*)
{
    return 0;  // TODO(后续批次)
}

// ---- 杂项对象 ----
StackableAction::Action::Action() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
expert_extraction::CExtractionMgr::CExtractionMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
NPC_Teleport::NPC_Teleport() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
User_Teleport::User_Teleport() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
random_option::CRandomOptionItemHandle::CRandomOptionItemHandle()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)

// ---- 限制服务/地理/人证 ----
bool ServiceRestrictManager::initialize() { return true; }  // TODO(后续批次)
void ServiceRestrictManager::savePolicy() {}  // TODO(后续批次)
void ServiceRestrictManager::loadPolicy() {}  // TODO(后续批次)
bool RestrictGeolocation::initGeolocation() { return true; }  // TODO(后续批次)
void CHumanCertifier::init() {}  // TODO(后续批次)

// ---- CDBEnviroment ----
CDBEnviroment::CDBEnviroment() : CEnvironment()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CDBEnviroment::~CDBEnviroment() {}  // TODO(后续批次)

// ============================================================================
// 静态成员定义（平凡/指针；对象型成员由全局 TU 定义，避免单 TU 静态初始化）
// ============================================================================

bool GlobalData::s_server_stop = false;
CSystemTime GlobalData::s_systemTime_;
CaptchaGen GlobalData::cg_;
MsgQueueMgr* GlobalData::s_msgq_mgr = 0;
StreamPool* GlobalData::s_stream_pool = 0;
PacketPool* GlobalData::s_packet_pool_list[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
BigStreamPool* GlobalData::s_big_stream_pool = 0;
SmallStreamPool* GlobalData::s_small_stream_pool = 0;
DBMgr* GlobalData::s_db_mgr = 0;
DBThread* GlobalData::s_db_thr = 0;
DBThread* GlobalData::s_db_thr_for_logDB = 0;
DispatchThread* GlobalData::s_dispatch_thr = 0;
NetworkThread* GlobalData::s_net_thr = 0;
UdpThread* GlobalData::s_udp_thr = 0;
AcceptThread* GlobalData::s_accept_thr = 0;
MonitorTcpThread* GlobalData::s_monitor_tcp_thr = 0;
CServerProxyMgr<CMonitorServerProxy>* GlobalData::s_monitor_proxy_mgr = 0;
CServerProxyMgr<CGuildServerProxy>* GlobalData::s_guild_proxy_mgr = 0;
CServerProxyMgr<CStatisticServerProxy>* GlobalData::s_statistic_proxy_mgr = 0;
CHadesServerProxy* GlobalData::s_hades_proxy = 0;
CDoubleConnCheckServerProxy* GlobalData::s_double_check_proxy = 0;
CSchoolServerProxy* GlobalData::s_school_proxy = 0;
CPCRoomServerProxy* GlobalData::s_pcroom_proxy = 0;
CCommunityServerProxy* GlobalData::s_community_proxy = 0;
CAuctionServerProxy* GlobalData::s_auction_proxy = 0;
CCeraAuctionServerProxy* GlobalData::s_cera_auction_proxy = 0;
CPowerManager* GlobalData::s_power_manager = 0;
secretshop::CSecretShop* GlobalData::s_secret_shop = 0;
CFatigueBatteryHandle* GlobalData::s_fatigueBatteryHandle_ = 0;
CmmChannelProxy* GlobalData::s_channel_proxy = 0;
CRijndael* GlobalData::s_pcryptRijndael_CharacView_ = 0;
CRijndael* GlobalData::s_pcryptRijndael_Password_ = 0;
CRijndael* GlobalData::s_pcryptRijndael_MousePassword_ = 0;
Cipher* GlobalData::s_secu_cipher_keystring_manager_[0x4e20] = {0};
CTEA* GlobalData::s_pcryptTEA_MousePassword_ = 0;
CTEA* GlobalData::s_pcryptTEA_SecurityCard_ = 0;
WongWork::CSimpleSSO* GlobalData::s_psimpleSSO = 0;
WongWork::CSecurityCardCenter* GlobalData::s_securityCardCenter = 0;
WongWork::CGMAccounts* GlobalData::s_pGMAccounts_ = 0;
CEventManager* GlobalData::s_event_manager = 0;
WongWork::IPG::CIPGHelper* GlobalData::s_pIPGHelper = 0;
void* GlobalData::s_rentalSystem = 0;
RefPvpGrade* GlobalData::s_ref_pvp_grade = 0;
WongWork::CLogGameChannel* GlobalData::s_pLogGameChannel = 0;
WongWork::CCeraShop* GlobalData::s_pCeraShop = 0;
CDBConnector* GlobalData::s_db_connector = 0;
void* GlobalData::s_pCompoundAvatar = 0;
void* GlobalData::s_pCompoundBindCube = 0;
void* GlobalData::s_pTowerOfDespairMgr = 0;
void* GlobalData::s_pEvent_UpGrade_Jar = 0;
CGM_Manager* GlobalData::s_GM_Manager = 0;
ItemVendingMachine* GlobalData::s_pItemVendingMachine = 0;
CStatisticMgr* GlobalData::s_statisticMgr = 0;
expert_job::CExpertJobMgr* GlobalData::s_ExpertJobMgr = 0;
break_away_prevent::CBreakAwayPreventSystem* GlobalData::s_BreakAwaySys = 0;
Gm_List* GlobalData::gmList = 0;
village_object::CVillageObjectMgr* GlobalData::s_villageObjectMgr = 0;
game_master::CGameMasterMgr* GlobalData::s_GameMasterMgr = 0;
village_attacked::CVillageMonsterMgr* GlobalData::s_villageMonsterMgr = 0;
village_attacked::CRevengeDungeon* GlobalData::s_revengeDungeonMgr = 0;
CDailyScheduleManager* GlobalData::s_DailyScheduleManager = 0;
online_preliminary::COnlinePreliminaryTeamMgr* GlobalData::s_onlinePreliminaryTeamMgr = 0;
Secu_DataControl* GlobalData::s_pSecuDataControl = 0;
CSecu_ProtectionField* GlobalData::s_pSecuProtectionField = 0;
WongWork::CAutoPunishRuleBlackIPMgr* GlobalData::g_autoPunishRuleBlackIPMgr = 0;
WongWork::CAutoPunishRuleHackTypeMgr* GlobalData::g_autoPunishRuleHackTypeMgr = 0;
WongWork::CBlackIPMonitor* GlobalData::g_blackIPMonitor = 0;
unsigned int GlobalData::g_IPMonitorLastDBAccessTime = 0;
CBusinessImpl* GlobalData::s_antibotBusinessImpl_ = 0;
void* GlobalData::s_antibotChecker_ = 0;
CItemAmplifier* GlobalData::s_itemAmplifier_ = 0;
CGlobalEffectManager* GlobalData::s_globalEffectManager_ = 0;
ARAD::Arad_ServerStateManager* GlobalData::s_serverStateManager_ = 0;
CTimeGate* GlobalData::s_timeGate_ = 0;
int GlobalData::g_GoldControlDateFlag = 0;
StackableAction::Action* GlobalData::g_Actions = 0;
expert_extraction::CExtractionMgr* GlobalData::g_ExtractionMgr = 0;
void* GlobalData::g_pTeleport[2] = {0, 0};
random_option::CRandomOptionItemHandle* GlobalData::g_randomOptionHandle = 0;
void* GlobalData::s_pAvatarRoulette = 0;

// ============================================================================
// GlobalData::setCurrentPacketPool
// ============================================================================

bool GlobalData::setCurrentPacketPool(THREAD_TYPE type, pthread_t threadID)
{
    if (type < 0 || type > 8)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2190,
                              "[PacketPool setCurrentThreadID err] pool_type(%d), thread_id(%08x)",
                              type, threadID);
        return false;
    }

    if (pthread_equal(0, s_packet_pool_list[type]->getThreadID()) == 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2195,
                              "[PacketPool already ThreadID setting] pool_type(%d), thread_id(%08x) alreadyID(%08x)",
                              type, threadID, s_packet_pool_list[type]->getThreadID());
        return false;
    }

    s_packet_pool_list[type]->setThreadID(threadID);
    return true;
}

// ============================================================================
// GlobalData::InitCaptcha
// ============================================================================

bool GlobalData::InitCaptcha()
{
    if (!cg_.addFont("captcha_font/SeoulHangangL.ttf", 0x24))
    {
        std::cerr << "[GlobalData::Init] cg.loadFont fail" << std::endl;
        return false;
    }

    for (int i = 0; i <= 0x27; ++i)
    {
        GenerateCaptchaData();
    }
    return true;
}

// ============================================================================
// GlobalData::GenerateCaptchaData
// ============================================================================

void GlobalData::GenerateCaptchaData()
{
    GreyImage image;
    MSG_UDP_CAPCHA_CERTIFY msg;

    memset(&msg, 0, 0x339e);
    cg_.generateText(CountryCode::taiwan, 5, (unsigned short*)&msg);
    CaptchaGen::ImageOption opt;
    opt.m_field8 = 0;
    cg_.generateImage(0xdc, 0x3c, (const unsigned short*)&msg, 5, opt, &image);
    memcpy(msg.m_image, image.getImage(), image.getWidth() * image.getHeight());
    msg.m_size = image.getWidth() * image.getHeight();
    s_capchaDataManager.push(msg);
}

// ============================================================================
// GlobalData::Fini
// ============================================================================

void GlobalData::Fini()
{
    delete s_ref_pvp_grade;
    if (s_accept_thr)
    {
        s_accept_thr->Stop();
    }
    if (s_monitor_proxy_mgr)
    {
        delete s_monitor_proxy_mgr;
        s_monitor_proxy_mgr = 0;
    }
    if (s_guild_proxy_mgr)
    {
        delete s_guild_proxy_mgr;
        s_guild_proxy_mgr = 0;
    }
    if (s_statistic_proxy_mgr)
    {
        delete s_statistic_proxy_mgr;
        s_statistic_proxy_mgr = 0;
    }
    if (s_udp_thr)
    {
        s_udp_thr->Stop();
    }
    if (s_net_thr)
    {
        s_net_thr->Stop();
    }
    if (s_dispatch_thr)
    {
        s_dispatch_thr->Stop();
    }
    if (s_db_thr_for_logDB)
    {
        s_db_thr_for_logDB->Stop();
    }
    if (s_db_thr)
    {
        s_db_thr->Stop();
    }
    if (s_monitor_tcp_thr)
    {
        s_monitor_tcp_thr->Stop();
    }
    delete s_db_mgr;
    delete s_stream_pool;
    delete s_small_stream_pool;
    delete s_big_stream_pool;
    delete s_msgq_mgr;
    delete s_pItemVendingMachine;
    CSyncSlangFilter::FreeInstance();
    delete s_pcryptRijndael_CharacView_;
    delete s_psimpleSSO;
    delete s_pGMAccounts_;
    delete s_GM_Manager;
    delete s_antibotBusinessImpl_;
    delete s_BreakAwaySys;
    delete s_ExpertJobMgr;
    delete s_villageObjectMgr;
    delete s_villageMonsterMgr;
    delete s_revengeDungeonMgr;
    delete s_DailyScheduleManager;
    delete s_onlinePreliminaryTeamMgr;
    delete s_auction_proxy;
    delete s_cera_auction_proxy;
    delete s_db_connector;
    delete (InstanceRentalSystem*)s_rentalSystem;
    for (int i = 0; i < 9; ++i)
    {
        if (s_packet_pool_list[i])
        {
            delete s_packet_pool_list[i];
            s_packet_pool_list[i] = 0;
        }
    }
}

// ============================================================================
// GlobalData::initTournamentDB
// ============================================================================

bool GlobalData::initTournamentDB(const char* fileName)
{
    typedef std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > GroupNameMap;
    typedef std::map<ENUM_SERVER_GROUP, DBInfoMap> GroupDBMap;

    if (G_GameWorld()->GetChannelType() != 7 &&
        !G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return true;
    }

    CDBEnviroment dbEnv;
    dbEnv.set_file_name(fileName);
    if (!dbEnv.load())
    {
        return false;
    }
    if (!s_db_connector)
    {
        return false;
    }

    GroupNameMap::iterator it =
        (*(GroupNameMap*)((char*)G_CEnvironment() + 0x50)).begin();
    while (it != (*(GroupNameMap*)((char*)G_CEnvironment() + 0x50)).end())
    {
        ENUM_SERVER_GROUP group = it->first;
        DBGroupMap groupMap;
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)2, group));
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)3, group));
        DBInfoMap& info = (*(GroupDBMap*)((char*)G_CEnvironment() + 0x80))[group];
        if (!s_db_connector->GetConnInfo(groupMap, info))
        {
            return false;
        }
        if (!s_db_connector->ConnectWithServerGroup(DBInfoMap(info)))
        {
            return false;
        }
        ++it;
    }
    cMyTrace t("static bool GlobalData::initTournamentDB(const char*)", 0x20aa, 0);
    t("This is Tournament PvP channel");
    s_db_mgr->activateGlobalDB();
    return true;
}

// ============================================================================
// GlobalData::Init
// ============================================================================

bool GlobalData::Init()
{
    s_server_stop = false;
    s_systemTime_.update();

    s_itemAmplifier_ = new CItemAmplifier;
    s_globalEffectManager_ = new CGlobalEffectManager;

    s_timeGate_ = new (std::nothrow) CTimeGate;
    if (!s_timeGate_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate s_timeGate_ memory" << std::endl;
        return false;
    }

    s_serverStateManager_ = new ARAD::Arad_ServerStateManager;

    s_msgq_mgr = new MsgQueueMgr;
    if (!s_msgq_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate msg queue memory" << std::endl;
        return false;
    }

    s_stream_pool = new StreamPool;
    if (!s_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate stream pool memory" << std::endl;
        return false;
    }

    for (int i = 0; i < 9; ++i)
    {
        s_packet_pool_list[i] = new PacketPool;
        if (!s_packet_pool_list[i])
        {
            std::cerr << "[GlobalData::Init] can't allocate packet pool memory" << std::endl;
            return false;
        }
    }
    setCurrentPacketPool(THREAD_TYPE_ACCEPT, pthread_self());

    s_small_stream_pool = new SmallStreamPool;
    if (!s_small_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate small stream pool memory" << std::endl;
        return false;
    }

    s_big_stream_pool = new BigStreamPool;
    if (!s_big_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate big stream pool memory" << std::endl;
        return false;
    }

    s_db_mgr = new DBMgr;
    if (!s_db_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate db manager memory" << std::endl;
        return false;
    }

    s_db_thr = new DBThread;
    if (!s_db_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory" << std::endl;
        return false;
    }
    s_db_thr->setLogDB(false);

    s_db_thr_for_logDB = new DBThread;
    if (!s_db_thr_for_logDB)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory, s_db_thr_for_logDB"
                  << std::endl;
        return false;
    }
    s_db_thr_for_logDB->setLogDB(true);

    s_dispatch_thr = new DispatchThread;
    if (!s_dispatch_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate dispath thread memory" << std::endl;
        return false;
    }

    s_net_thr = new NetworkThread;
    if (!s_net_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate network thread memory" << std::endl;
        return false;
    }

    s_udp_thr = new UdpThread;
    if (!s_udp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate udp thread memory" << std::endl;
        return false;
    }

    s_monitor_tcp_thr = new (std::nothrow) MonitorTcpThread;
    if (!s_monitor_tcp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor tcp thread memory" << std::endl;
        return false;
    }

    s_monitor_proxy_mgr = new (std::nothrow) CServerProxyMgr<CMonitorServerProxy>;
    if (!s_monitor_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor server proxy memory" << std::endl;
        return false;
    }

    s_guild_proxy_mgr = new (std::nothrow) CServerProxyMgr<CGuildServerProxy>;
    if (!s_guild_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate guild server proxy memory" << std::endl;
        return false;
    }

    s_statistic_proxy_mgr = new (std::nothrow) CServerProxyMgr<CStatisticServerProxy>;
    if (!s_statistic_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate statistic server proxy memory" << std::endl;
        return false;
    }

    char* hadesIP = G_CEnvironment()->get_udp_ip_hades();
    int hadesTCP = G_CEnvironment()->get_tcp_port_hades();
    int hadesUDP = G_CEnvironment()->get_udp_port_hades();
    s_hades_proxy = new (std::nothrow) CHadesServerProxy(hadesIP, hadesTCP, hadesUDP);
    if (!s_hades_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Hades server proxy memory" << std::endl;
        return false;
    }
    if (!s_hades_proxy->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (Hades server proxy)" << std::endl;
        return false;
    }

    {
        std::string doubleCheckIP(G_CEnvironment()->get_udp_ip_doublecheck());
        s_double_check_proxy = new (std::nothrow)
            CDoubleConnCheckServerProxy(doubleCheckIP,
                                        G_CEnvironment()->get_udp_port_doublecheck());
        if (!s_double_check_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate double conn check server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_double_check_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (double conn check server proxy)"
                      << std::endl;
            return false;
        }
    }

    if (*G_CEnvironment()->get_school_server_ip() != '\0' &&
        G_CEnvironment()->get_school_server_port() >= 1)
    {
        std::string schoolIP(G_CEnvironment()->get_school_server_ip());
        s_school_proxy = new (std::nothrow)
            CSchoolServerProxy(schoolIP, G_CEnvironment()->get_school_server_port());
        if (!s_school_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate school server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_school_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (school server proxy)" << std::endl;
            return false;
        }
    }

    s_pcroom_proxy = new (std::nothrow)
        CPCRoomServerProxy(G_CEnvironment()->get_pcroom_server_ip(),
                           G_CEnvironment()->get_pcroom_server_port());
    if (!s_pcroom_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pcroom server proxy memory" << std::endl;
        return false;
    }

    s_community_proxy = new (std::nothrow)
        CCommunityServerProxy(G_CEnvironment()->get_community_server_ip(),
                              G_CEnvironment()->get_community_server_port());
    if (!s_community_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate community server proxy memory"
                  << std::endl;
        return false;
    }

    s_auction_proxy = new (std::nothrow)
        CAuctionServerProxy(G_CEnvironment()->get_auction_server_ip(),
                            G_CEnvironment()->get_auction_server_port(), "Auction Server");
    if (!s_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate auction server proxy memory" << std::endl;
        return false;
    }

    s_cera_auction_proxy = new (std::nothrow)
        CCeraAuctionServerProxy(G_CEnvironment()->get_cera_auction_server_ip(),
                                G_CEnvironment()->get_cera_auction_server_port(),
                                "Cera Auction Server");
    if (!s_cera_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate cera auction server proxy memory"
                  << std::endl;
        return false;
    }

    s_power_manager = new (std::nothrow) CPowerManager;
    if (!s_power_manager)
    {
        std::cerr << "[GlobalData::Init] Can't allocate power manager memory" << std::endl;
        return false;
    }
    if (!s_power_manager->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (power manager memory)" << std::endl;
        return false;
    }
    CPowerManager::LoadPowerWarInfo();
    CPowerManager::LoadRankerInfo();

    s_fatigueBatteryHandle_ = new (std::nothrow) CFatigueBatteryHandle;
    if (!s_fatigueBatteryHandle_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Fatigue Battery Handle memory"
                  << std::endl;
        return false;
    }

    s_secret_shop = new (std::nothrow) secretshop::CSecretShop;
    if (!s_secret_shop)
    {
        std::cerr << "[GlobalData::Init] Can't allocate SecretShop Handle memory" << std::endl;
        return false;
    }

    {
        std::string channelIP(G_CEnvironment()->get_udp_ip_channel());
        s_channel_proxy = new (std::nothrow)
            CmmChannelProxy(channelIP, G_CEnvironment()->get_udp_port_channel());
        if (!s_channel_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate channel server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_channel_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (channel server proxy)" << std::endl;
            return false;
        }
    }

    s_pcryptRijndael_CharacView_ = new CRijndael;
    s_pcryptRijndael_CharacView_->Initialize("wongyaofejsvk007", 0x20,
                                             CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_Password_ = new CRijndael;
    s_pcryptRijndael_Password_->Initialize("wongyaofneople00", 0x20,
                                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_MousePassword_ = new CRijndael;
    s_pcryptRijndael_MousePassword_->Initialize("ejsvk@@key4mouse", 0x20,
                                                CRijndael::sm_chain0, 0x10, 0x10, 0, 0);

    s_pcryptTEA_MousePassword_ = new CTEA;
    s_pcryptTEA_MousePassword_->Initialize("tjsghTlWkdajtwla", 0x10, "", 0, 0);
    s_pcryptTEA_SecurityCard_ = new CTEA;
    s_pcryptTEA_SecurityCard_->Initialize("ejsvk@@@secucard", 0x10, "", 0, 0);

    s_pLogGameChannel = new (std::nothrow) WongWork::CLogGameChannel;
    s_pLogGameChannel->StartLogTimer();

    s_pCeraShop = new (std::nothrow) WongWork::CCeraShop;
    s_pItemVendingMachine = new (std::nothrow) ItemVendingMachine;

    s_pSecuDataControl = new Secu_DataControl;
    s_pSecuProtectionField = new CSecu_ProtectionField;

    g_autoPunishRuleBlackIPMgr = new WongWork::CAutoPunishRuleBlackIPMgr;
    g_autoPunishRuleBlackIPMgr->reset();
    g_autoPunishRuleHackTypeMgr = new WongWork::CAutoPunishRuleHackTypeMgr;
    g_autoPunishRuleHackTypeMgr->reset();
    g_blackIPMonitor = new WongWork::CBlackIPMonitor;
    g_blackIPMonitor->reset();

    s_accept_thr = new AcceptThread;
    if (!s_accept_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate accept thread memory" << std::endl;
        return false;
    }

    s_ref_pvp_grade = new RefPvpGrade;
    if (!s_ref_pvp_grade)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pvp grade ref" << std::endl;
        return false;
    }

    s_db_connector = new (std::nothrow) CDBConnector(s_db_mgr);
    if (!s_db_connector)
    {
        return false;
    }

    DBInfoMap& masterDbInfo =
        *(DBInfoMap*)((char*)G_CEnvironment() + 0x68);
    int serverGroup = *(int*)((char*)G_CEnvironment() + 0x378);

    {
        DBGroupMap groupMap;
        for (int i = 1; i < 0xf; ++i)
        {
            groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)i, (ENUM_SERVER_GROUP)i));
        }
        int idx1 = 0;
        STDBConnInfo& db1 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx1];
        int idx2 = 0;
        STDBConnInfo& db2 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx2];
        int idx3 = 0;
        STDBConnInfo& db3 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx3];
        unsigned int masterPort = *(unsigned int*)((char*)&db3 + 0x14);
        int idx4 = 0;
        STDBConnInfo& db4 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx4];
        (void)serverGroup;
        cMyTrace t1("static bool GlobalData::Init()", 0x1b0c, 9);
        t1("\t- DBConnector Init(masterdb %s/%d/%s/%s) ", &db4, masterPort,
           (char*)&db2 + 0x18, (char*)&db1 + 0x42);
        int idx0 = 0;
        STDBConnInfo& db0 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx0];
        if (!s_db_connector->Init(db0))
        {
            cMyTrace t2("static bool GlobalData::Init()", 0x1b0f, 9);
            t2("\t- DBConnector Init Fail");
            return false;
        }
        cMyTrace t3("static bool GlobalData::Init()", 0x1b12, 9);
        t3("Success");
        cMyTrace t4("static bool GlobalData::Init()", 0x1b14, 9);
        t4("\t- DBConnector Get Connection Info ");
        if (!s_db_connector->GetConnInfo(groupMap, masterDbInfo))
        {
            cMyTrace t5("static bool GlobalData::Init()", 0x1b17, 9);
            t5("\t- DBConnector Get Connection Info Fail");
            return false;
        }
        cMyTrace t6("static bool GlobalData::Init()", 0x1b1a, 9);
        t6("Success");
        cMyTrace t7("static bool GlobalData::Init()", 0x1b1c, 9);
        t7("\t- Open DB ... ");
        if (!s_db_connector->Connect(DBInfoMap(masterDbInfo)))
        {
            cMyTrace t8("static bool GlobalData::Init()", 0x1b1f, 9);
            t8("\t- DBConnector Connect DB Fail");
            return false;
        }
        cMyTrace t9("static bool GlobalData::Init()", 0x1b22, 9);
        t9("\t- All DB Open Success");
    }

    s_psimpleSSO = new WongWork::CSimpleSSO;
    s_psimpleSSO->init(s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0));

    s_securityCardCenter = new WongWork::CSecurityCardCenter;
    s_securityCardCenter->init();

    s_pIPGHelper = new (std::nothrow) WongWork::IPG::CIPGHelper;
    if (s_pIPGHelper->Initialize(1, 5000) < 0)
    {
        puts("Fail to init IPG Module");
        delete s_pIPGHelper;
        return false;
    }

    CHumanCertifier::init();

    s_pGMAccounts_ = new WongWork::CGMAccounts;

    {
        cMyTrace t1("static bool GlobalData::Init()", 0x1d4d, 9);
        t1("\t- Init Query Table ");
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t2("static bool GlobalData::Init()", 0x1d50, 9);
            t2("\t- Init Query Table Fail");
            return false;
        }
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t3("static bool GlobalData::Init()", 0x1d55, 9);
            t3("\t- Init Query Table Fail");
            return false;
        }
        cMyTrace t4("static bool GlobalData::Init()", 0x1d58, 9);
        t4("Success");
    }

    bool syncEnable = false;
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        syncEnable = true;
    }
    sync_script::CSyncScript::SetEnable(syncEnable);
    if (sync_script::CSyncScript::IsEnable())
    {
        int webIdx = 5;
        STDBConnInfo& web1 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx];
        int webIdx2 = 5;
        STDBConnInfo& web2 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx2];
        int webIdx3 = 5;
        STDBConnInfo& web3 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx3];
        int webIdx4 = 5;
        STDBConnInfo& web4 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx4];
        unsigned int webPort = *(unsigned int*)((char*)&web4 + 0x14);
        int webIdx5 = 5;
        STDBConnInfo& web5 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx5];
        if (!s_db_mgr->Open((ENUM_DB_HANDLE_IDX)5, (char*)&web5, webPort,
                            (char*)&web3 + 0x42, (char*)&web2 + 0x18, (char*)&web1 + 0x2d,
                            (ENUM_SERVER_GROUP)0))
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1d8d, 9);
            t("\t- Failed to open web DB");
            return false;
        }
        cMyTrace t("static bool GlobalData::Init()", 0x1d90, 9);
        t("\t- Success WEB DB OPEN");
    }

    bool slangFail;
    if (CSyncSlangFilter::AddSlangListName() == 1)
    {
        if (CSyncSlangFilter::AddSlangList() != 1)
        {
            slangFail = true;
        }
        else
        {
            slangFail = false;
        }
    }
    else
    {
        slangFail = true;
    }

    if (slangFail)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1d9f, 9);
        t("Failed to read slang list");
        return false;
    }

    s_event_manager = new (std::nothrow) CEventManager;
    if (!s_event_manager)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1da8, 9);
        t("Can't allocate CEventManager memory");
        return false;
    }

    cMyTrace t("static bool GlobalData::Init()", 0x1dc6, 9);
    t("\t- Load GM List From DB");
    gmList = new (std::nothrow) Gm_List;
    if (!gmList)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1dcd, 9);
        t("\t- Can't allocate memory for a Gm_List");
        return false;
    }

    s_GM_Manager = new (std::nothrow) CGM_Manager;
    if (!s_GM_Manager)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1dd6, 9);
        t("Can't allocate memory for a CGM_Manager");
        return false;
    }

    {
        int keyLen = G_Cipher()->GetTotalKeyLength();
        unsigned char key[1024];
        for (int i = 0; i < keyLen; ++i)
        {
            key[i] = (unsigned char)rand();
        }
        G_Cipher()->Initialize(key, keyLen);
    }

    CStatisticMgr* statMgr =
        (CStatisticMgr*)operator new(0x30, std::nothrow);
    if (statMgr)
    {
        int* q = (int*)statMgr;
        for (int i = 0xc; i != 0; --i)
        {
            *q = 0;
            q += 4;
        }
        new (statMgr) CStatisticMgr();
    }
    s_statisticMgr = statMgr;
    if (!s_statisticMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1df7, 9);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    s_BreakAwaySys = new (std::nothrow) break_away_prevent::CBreakAwayPreventSystem;
    if (!s_BreakAwaySys)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1dfe, 9);
        t("Can't allocate memory for a s_BreakAwaySys");
        return false;
    }

    s_ExpertJobMgr = new (std::nothrow) expert_job::CExpertJobMgr;
    if (!s_ExpertJobMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e05, 9);
        t("Can't allocate memory for a CExpertJobMgr");
        return false;
    }

    s_villageObjectMgr = new (std::nothrow) village_object::CVillageObjectMgr;
    if (!s_villageObjectMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e0c, 9);
        t("Can't allocate memory for a CVillageObjectMgr");
        return false;
    }

    s_villageMonsterMgr = new (std::nothrow) village_attacked::CVillageMonsterMgr;
    if (!s_villageMonsterMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e13, 9);
        t("Can't allocate memory for a CVillageMonsterMgr");
        return false;
    }

    s_rentalSystem = new (std::nothrow) InstanceRentalSystem;
    if (!s_rentalSystem)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e1b, 9);
        t("an`t allocate memory for a InstanceRentalSystem");
        return false;
    }

    s_revengeDungeonMgr = new (std::nothrow) village_attacked::CRevengeDungeon;
    if (!s_revengeDungeonMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e24, 9);
        t("Can't allocate memory for a CRevengeDungeon");
        return false;
    }

    s_DailyScheduleManager = new (std::nothrow) CDailyScheduleManager;
    if (!s_DailyScheduleManager)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e2d, 9);
        t("Can't allocate memory for a CDailyScheduleManager");
        return false;
    }

    s_onlinePreliminaryTeamMgr = new (std::nothrow) online_preliminary::COnlinePreliminaryTeamMgr;
    if (!s_onlinePreliminaryTeamMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e36, 9);
        t("Can't allocate memory for a COnlinePreliminaryTeamMgr");
        return false;
    }

    s_GameMasterMgr = new (std::nothrow) game_master::CGameMasterMgr;
    if (!s_GameMasterMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e47, 9);
        t("Can't allocate memory for a CGameMasterMgr");
        return false;
    }

    TimerUpdatePvPGrade::registNextTimer(s_systemTime_.getCurSec());
    DB_ReloadAutoPunishRuleBackIP::makeRequest();
    DB_ReloadAutoPunishRuleHackType::makeRequest();
    DB_LoadCleanPadPoint::makeRequest();
    DB_BlackIPMonitor::makeRequest();

    s_antibotBusinessImpl_ = new CBusinessImpl;
    s_antibotChecker_ = CreateAntiServerObj("./");
    if (!s_antibotChecker_)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1eb0, 5);
        t("AntiBot create fail");
        return false;
    }

    {
        char seq[4] = {0, 0, 0, 0};
        struct CEnvProcSeq
        {
            int getProcessSequence() const
            {
                return 0;  // TODO(后续批次)：ORIG 读取环境对象的进程序列号
            }
        };
        sprintf(seq, "%d", ((const CEnvProcSeq*)G_CEnvironment())->getProcessSequence());
        typedef void (*AntibotInitFn)(void*, void*, char*);
        (*(AntibotInitFn*)s_antibotChecker_)(s_antibotChecker_, s_antibotBusinessImpl_, seq);
    }

    CStatisticMgr* statMgr2 =
        (CStatisticMgr*)operator new(0x30, std::nothrow);
    if (statMgr2)
    {
        int* q = (int*)statMgr2;
        for (int i = 0xc; i != 0; --i)
        {
            *q = 0;
            q += 4;
        }
        new (statMgr2) CStatisticMgr();
    }
    s_statisticMgr = statMgr2;
    if (!s_statisticMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1ebf, 9);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    g_Actions = new (std::nothrow) StackableAction::Action;
    if (!g_Actions)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1ed9, 9);
        t("Can't allocate memory for a StackableAction::Action()");
        return false;
    }

    {
        Stream* s = s_stream_pool->Acquire("App.cpp", 0x1ee0);
        CStreamGuard guard(s, true);
        **guard << 0x277;
        **guard << -1;
        s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    if (InitCaptcha())
    {
        Timer_GenerateCaptchaData::RegistNextTimer(3);
        g_ExtractionMgr = new (std::nothrow) expert_extraction::CExtractionMgr;
        if (!g_ExtractionMgr)
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1ef5, 9);
            t("Can't allocate memory for a CExtractionMng");
            return false;
        }

        g_pTeleport[0] = new (std::nothrow) NPC_Teleport;
        if (!g_pTeleport[0])
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f02, 9);
            t("Can't allocate memory for a NPC_Teleport()");
            return false;
        }

        g_pTeleport[1] = new (std::nothrow) User_Teleport;
        if (!g_pTeleport[1])
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f09, 9);
            t("Can't allocate memory for a User_Teleport()");
            return false;
        }

        g_randomOptionHandle = new (std::nothrow) random_option::CRandomOptionItemHandle;
        if (!g_randomOptionHandle)
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f12, 9);
            t("Can't allocate memory for a CRandomOptionItemHandle");
            return false;
        }

        ServiceRestrictManager* srm = ARAD::Singleton<ServiceRestrictManager>::Get();
        if (srm->initialize() == 1)
        {
            ARAD::Singleton<ServiceRestrictManager>::Get()->savePolicy();
            ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
            RestrictGeolocation* geo = ARAD::Singleton<RestrictGeolocation>::Get();
            if (geo->initGeolocation() == 1)
            {
                return true;
            }
            cMyTrace t("static bool GlobalData::Init()", 0x1f2a, 9);
            t("Can't initialize GeoIP");
            return false;
        }
        else
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f1f, 9);
            t("Can't allocate memory for a ServiceRestrictManager");
            return false;
        }
    }
    else
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1eea, 9);
        t("captcha data initialization failed");
        return false;
    }
}
