// ============================================================================
// df_game_r Village（G2-5 村庄对象，sizeof 0x34）
// 逐函数对照 docs/class_func_reports/Village.md 与 ORIG 反汇编还原。
// 布局（ORIG ctor 086c32e0 / set_village 086c33b8 推导）：
//   +0x00 int    m_villageId      （set_village 写入 town script +0）
//   +0x04 pad（4）
//   +0x08 map<ushort,CUser*> m_users（0x18 字节，ctor 调 _ZNSt3map...C1Ev）
//   +0x20 bool   m_bEnabled
//   +0x24 int    m_areaCount
//   +0x28 Area*  m_pAreas         （new[] 数组，前 4 字节存元素个数）
//   +0x2c int    m_field2c        （set_village 写入 town script +0x3c）
//   +0x30 int    m_field30        （set_village 写入 town script +0x40）
// 跨类调用全部走 extern "C" asm 标签（Area/TownAreaScript/STMapScript/
// std::map/LogManager/cMyTrace 等），符号由 ORIG/GameStubs 提供。
// ============================================================================

namespace std
{
template <class T> class allocator;
template <class T, class A> class vector;
}

class CUser;
class PacketGuard;
class TownScript;
class Area;
class TownAreaScript;
class STMapScript;

// ---- 跨类 extern（真实符号，asm 标签按 ORIG mangling）----
extern "C" void sub_map_ushort_ctor(void* self)
    asm("_ZNSt3mapItP5CUserSt4lessItESaISt4pairIKtS1_EEEC1Ev");
extern "C" void sub_map_ushort_dtor(void* self)
    asm("_ZNSt3mapItP5CUserSt4lessItESaISt4pairIKtS1_EEED1Ev");

extern "C" void sub_Area_ctor(void* self) asm("_ZN4AreaC1Ev");
extern "C" void sub_Area_dtor(void* self) asm("_ZN4AreaD1Ev");
extern "C" int sub_Area_GetUserCount(void* self)
    asm("_ZNK4Area12GetUserCountEv");
extern "C" void sub_Area_get_user_id_list(void* self, void* list)
    asm("_ZN4Area16get_user_id_listERSt6vectorItSaItEE");
extern "C" void sub_Area_send_to_all(void* self, void* pkt)
    asm("_ZN4Area11send_to_allER11PacketGuard");
extern "C" void sub_Area_set_area(void* self, int villageId, void* areaScript,
                                  void* mapScript)
    asm("_ZN4Area8set_areaEiR14TownAreaScriptR11STMapScript");

extern "C" unsigned int sub_town_map_size(void* self)
    asm("_ZNKSt3mapIi14TownAreaScriptSt4lessIiESaISt4pairIKiS0_EEE4sizeEv");
extern "C" void sub_town_map_begin(void* sret, void* self)
    asm("_ZNSt3mapIi14TownAreaScriptSt4lessIiESaISt4pairIKiS0_EEE5beginEv");
extern "C" void sub_town_map_end(void* sret, void* self)
    asm("_ZNSt3mapIi14TownAreaScriptSt4lessIiESaISt4pairIKiS0_EEE3endEv");
extern "C" void* sub_town_iter_deref(void* self)
    asm("_ZNKSt17_Rb_tree_iteratorISt4pairIKi14TownAreaScriptEEptEv");
extern "C" void sub_town_iter_pp(void* sret, void* self, int dummy)
    asm("_ZNSt17_Rb_tree_iteratorISt4pairIKi14TownAreaScriptEEppEi");
extern "C" bool sub_town_iter_ne(void* self, void* other)
    asm("_ZNKSt17_Rb_tree_iteratorISt4pairIKi14TownAreaScriptEEneERKS4_");

extern "C" const char* sub_Ss_c_str(void* self) asm("_ZNKSs5c_strEv");
extern "C" bool sub_ImportMapScript(void* self, const char* mapName)
    asm("_Z15ImportMapScriptP11STMapScriptPKc");

extern "C" void* sub_G_CDataManager() asm("_Z14G_CDataManagerv");
extern "C" void* sub_CDataManager_get_dungeon(void* self, int dungeonId)
    asm("_ZN12CDataManager11get_dungeonEi");
extern "C" void sub_vector_int_push_back(void* self, const int* value)
    asm("_ZNSt6vectorIiSaIiEE9push_backERKi");

extern "C" void* sub_G_GameWorld() asm("_Z11G_GameWorldv");
extern "C" bool sub_GameWorld_IsSchoolPvPChannel(void* self)
    asm("_ZNK9GameWorld18IsSchoolPvPChannelEv");
extern "C" void sub_CSchoolMgr_SetSchoolArea(void* self, int begin, int end)
    asm("_ZN10CSchoolMgr13SetSchoolAreaEii");
extern "C" void sub_CGuildAgitManager_SetGuildAgitAreaDomain(void* self,
                                                             int begin,
                                                             int end)
    asm("_ZN17CGuildAgitManager22SetGuildAgitAreaDomainEii");

extern "C" void sub_LogManager_logFormat(int level, const char* file,
                                         const char* func, int line,
                                         const char* fmt, ...)
    asm("_ZN10LogManager9logFormatEiPKcS1_iS1_z");
extern "C" void sub_cMyTrace_ctor(void* self, const char* func, int line,
                                  int level)
    asm("_ZN8cMyTraceC1EPKcii");
extern "C" void sub_cMyTrace_log(void* self, const char* fmt, ...)
    asm("_ZN8cMyTraceclEPKcz");

extern "C" void* sub_operator_new_array(unsigned int size)
    asm("_Znaj");
extern "C" void sub_operator_delete_array(void* p)
    asm("_ZdaPv");

// ---- Area（0x98 字节；成员经 asm 标签调用，本 TU 不定义其字段）----
class Area
{
public:
    char m_pad[0x98];
};

// ---- TownAreaScript（0x30：int areaIndex_ + string mapName_ 等；0x30 由
// set_village 栈布局 -0x98/-0x68 推导）。构造/赋值/析构由 ORIG 提供。----
class TownAreaScript
{
public:
    TownAreaScript();
    TownAreaScript(const TownAreaScript& other);
    TownAreaScript& operator=(const TownAreaScript& other);
    ~TownAreaScript();

    int m_areaIndex;   // +0x00
    char m_pad[0x2c];  // +0x04（string mapName_ 位于 +0x04，c_str 经 asm 调用）
};

// ---- STMapScript（892 字节，构造/析构由 ORIG 提供）----
class STMapScript
{
public:
    STMapScript();
    ~STMapScript();

    char m_pad[0x37c];
};

// ---- TownScript（跨类只读：+0 类型、+0x18 dungeon、+0x20 map、+0x3c/+0x40）----
class TownScript
{
public:
    int m_id;                  // +0x00
    char m_pad04[0x14];        // +0x04
    int m_dungeonId;           // +0x18
    char m_pad1c[0x04];        // +0x1c
    char m_areaScripts[0x18];  // +0x20 map<int, TownAreaScript>
    char m_pad38[0x04];        // +0x38
    int m_field3c;             // +0x3c
    int m_field40;             // +0x40
};

// ---- Village（sizeof 0x34）----
class Village
{
public:
    Village();
    ~Village();
    void destroy();
    bool set_village(TownScript& script);
    int get_gate_area();
    Area* getArea(int area);
    void get_user_id_list(int area,
                          std::vector<unsigned short,
                                      std::allocator<unsigned short> >& list);
    void send_to_area(int area, PacketGuard& pkt);
    int GetUserCount(int area) const;

    int m_villageId;      // +0x00
    char m_pad04[4];      // +0x04
    char m_users[0x18];   // +0x08 std::map<unsigned short, CUser*>
    bool m_bEnabled;      // +0x20
    char m_pad21[3];      // +0x21
    int m_areaCount;      // +0x24
    Area* m_pAreas;       // +0x28
    int m_field2c;        // +0x2c
    int m_field30;        // +0x30
};

Village::Village()
{
    sub_map_ushort_ctor(m_users);
    m_pAreas = 0;
}

Village::~Village()
{
    destroy();
    sub_map_ushort_dtor(m_users);
}

void Village::destroy()
{
    if (m_pAreas)
    {
        if (m_pAreas)
        {
            int count = ((int*)m_pAreas)[-1];
            Area* pEnd = (Area*)((char*)m_pAreas + count * 0x98);
            while (pEnd != m_pAreas)
            {
                pEnd = (Area*)((char*)pEnd - 0x98);
                sub_Area_dtor(pEnd);
            }
            sub_operator_delete_array((char*)m_pAreas - 4);
        }
        m_pAreas = 0;
    }
}

bool Village::set_village(TownScript& script)
{
    bool result = false;

    m_villageId = script.m_id;
    if (script.m_dungeonId == -1)
    {
        m_bEnabled = false;
    }
    else
    {
        m_bEnabled = true;
        void* dungeon =
            sub_CDataManager_get_dungeon(sub_G_CDataManager(),
                                         script.m_dungeonId);
        sub_vector_int_push_back((char*)dungeon + 0x6d4, &m_villageId);
    }

    m_areaCount = (int)sub_town_map_size(script.m_areaScripts);
    if (m_areaCount == 0)
    {
        sub_LogManager_logFormat(
            1, "world.cpp", "bool Village::set_village(TownScript&)", 0x2d0,
            "town.townAreaScripts_.size() == 0");
        return false;
    }

    bool bSchool = sub_GameWorld_IsSchoolPvPChannel(sub_G_GameWorld()) != 0 &&
                   script.m_id == 2;
    if (bSchool)
    {
        int count = m_areaCount;
        int total = count + 600;
        int* raw = (int*)sub_operator_new_array((total * 0x26 + 1) * 4);
        raw[0] = total;
        Area* pArea = (Area*)(raw + 1);
        for (int i = total - 1; i != -1; --i)
        {
            sub_Area_ctor(pArea);
            pArea = (Area*)((char*)pArea + 0x98);
        }
        m_pAreas = (Area*)(raw + 1);
        sub_CSchoolMgr_SetSchoolArea((void*)0x943e080, m_areaCount,
                                     m_areaCount + 600);
    }
    else if (script.m_id == 8)
    {
        int count = m_areaCount;
        int total = count + 0x960;
        int* raw = (int*)sub_operator_new_array((total * 0x26 + 1) * 4);
        raw[0] = total;
        Area* pArea = (Area*)(raw + 1);
        for (int i = total - 1; i != -1; --i)
        {
            sub_Area_ctor(pArea);
            pArea = (Area*)((char*)pArea + 0x98);
        }
        m_pAreas = (Area*)(raw + 1);
        sub_CGuildAgitManager_SetGuildAgitAreaDomain((void*)0x943e0e0,
                                                     m_areaCount, total);
    }
    else
    {
        int count = m_areaCount;
        int* raw = (int*)sub_operator_new_array((count * 0x26 + 1) * 4);
        raw[0] = count;
        Area* pArea = (Area*)(raw + 1);
        for (int i = count - 1; i != -1; --i)
        {
            sub_Area_ctor(pArea);
            pArea = (Area*)((char*)pArea + 0x98);
        }
        m_pAreas = (Area*)(raw + 1);
    }

    m_field2c = script.m_field3c;
    m_field30 = script.m_field40;

    TownAreaScript temp;
    bool bSuccess = false;
    char iterBuf[4];
    char iterTmp[4];
    char endBuf[4];

    sub_town_map_begin(iterBuf, script.m_areaScripts);
    for (;;)
    {
        sub_town_map_end(endBuf, script.m_areaScripts);
        if (!sub_town_iter_ne(iterBuf, endBuf))
            break;

        void* pairPtr = sub_town_iter_deref(iterBuf);
        {
            TownAreaScript copy(*(TownAreaScript*)((char*)pairPtr + 4));
            temp = copy;
        }

        int areaIndex = temp.m_areaIndex;
        if (areaIndex < 0 || areaIndex >= m_areaCount)
        {
            char trace[16];
            sub_cMyTrace_ctor(trace, "bool Village::set_village(TownScript&)",
                              0x32e, 5);
            sub_cMyTrace_log(trace, "temp_area.areaIndex_ %d", areaIndex);
        }
        else
        {
            STMapScript mapScript;
            const char* mapName = sub_Ss_c_str((char*)&temp + 4);
            if (sub_ImportMapScript(&mapScript, mapName) == 1)
            {
                sub_Area_set_area((char*)m_pAreas + areaIndex * 0x98,
                                  m_villageId, &temp, &mapScript);
                if (sub_GameWorld_IsSchoolPvPChannel(sub_G_GameWorld()) != 0 &&
                    script.m_id == 2 && areaIndex == 4)
                {
                    for (int i = m_areaCount; i < m_areaCount + 600; ++i)
                    {
                        temp.m_areaIndex = i;
                        sub_Area_set_area((char*)m_pAreas + i * 0x98,
                                          m_villageId, &temp, &mapScript);
                    }
                    m_areaCount += 600;
                }
                if (script.m_id == 8 && areaIndex < 4)
                {
                    int nEnd = m_areaCount + 0x960;
                    int nStart = 0;
                    switch (areaIndex)
                    {
                    case 0:
                        nStart = m_areaCount;
                        break;
                    case 1:
                        nStart = m_areaCount + 1;
                        break;
                    case 2:
                        nStart = m_areaCount + 2;
                        break;
                    case 3:
                        nStart = m_areaCount + 3;
                        m_areaCount += 0x960;
                        break;
                    }
                    for (int i = nStart; i < nEnd; i += 4)
                    {
                        temp.m_areaIndex = i;
                        sub_Area_set_area((char*)m_pAreas + i * 0x98,
                                          m_villageId, &temp, &mapScript);
                    }
                }
                bSuccess = true;
            }
            else
            {
                result = false;
                bSuccess = false;
            }
            if (!bSuccess)
                goto cleanup;
        }

        sub_town_iter_pp(iterTmp, iterBuf, 0);
    }
    result = true;

cleanup:
    return result;
}

int Village::get_gate_area()
{
    for (int i = 0; i < m_areaCount; ++i)
    {
        if (*(int*)((char*)m_pAreas + i * 0x98 + 0x68) == 1)
            return i;
    }
    return 1;
}

Area* Village::getArea(int area)
{
    if (area < 0 || area >= m_areaCount)
    {
        char trace[16];
        sub_cMyTrace_ctor(trace, "Area* Village::getArea(int)", 0x356, 5);
        sub_cMyTrace_log(trace, "area(%d) < 0 || area >= m_iAreaCount(%d)",
                         area, m_areaCount);
        return 0;
    }
    return (Area*)((char*)m_pAreas + area * 0x98);
}

void Village::get_user_id_list(
    int area,
    std::vector<unsigned short, std::allocator<unsigned short> >& list)
{
    if (area < 0 || area >= m_areaCount)
    {
        sub_LogManager_logFormat(
            1, "world.cpp",
            "void Village::get_user_id_list(int, std::vector<short unsigned "
            "int, std::allocator<short unsigned int> >&)",
            0x36a, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        sub_Area_get_user_id_list((char*)m_pAreas + area * 0x98, &list);
    }
}

void Village::send_to_area(int area, PacketGuard& pkt)
{
    if (area < 0 || area >= m_areaCount)
    {
        sub_LogManager_logFormat(
            1, "world.cpp", "void Village::send_to_area(int, PacketGuard&)",
            0x360, "area(%d) < 0 || area >= m_iAreaCount(%d)", area,
            m_areaCount);
    }
    else
    {
        sub_Area_send_to_all((char*)m_pAreas + area * 0x98, &pkt);
    }
}

int Village::GetUserCount(int area) const
{
    if (area < 0 || area >= m_areaCount)
        return 0;
    return sub_Area_GetUserCount((char*)m_pAreas + area * 0x98);
}
