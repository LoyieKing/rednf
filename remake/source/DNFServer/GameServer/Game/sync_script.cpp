// ============================================================================
// df_game_r 还原 —— sync_script::CSyncScript 剩余方法（G5 数据/脚本域）
// 物品相关方法（insert_equiptment_item_to_db / insert_stackable_item_to_db /
// truncate_dnf_item_info_table / insertCeraShopItemIntoDB 等）已由 CItemList.cpp
// 实现；本 TU 补齐 skill/quest/dungeon/town/exp/creature/aicharacter/
// item_master/random_option 等。逐函数对照
// docs/class_func_reports/sync_script__CSyncScript.md 的 C 代码实现。
// ---------------------------------------------------------------------------
// [b22判词·N族/索引伪差集 P93·P154 同族] __tcf_0（<global>）：本 TU 文件作用域带
// 非平凡析构全局对象的 GCC 合成 atexit cleanup，无独立可写源码函数体，指令完全由
// 本 TU 全局对象集合与构造序决定。ORIG 二进制内 `__tcf_0` 同名 local 数百枚（每含
// 全局对象 TU 一枚），compare 工具按名取到 0x0828f940（WongWork::CDeathTowerRanking
// 域另一 TU，与本 TU 全局无关）→ 逐条 DIFF 系索引伪差集（§6.3 撞名族），非实现缺口。
// 语义等价，不可源码复现，保留判词。
// ============================================================================

#include "sync_script.h"

#include <stdio.h>
#include <string.h>

#include "DBMgr.h"
#include "GlobalData.h"
#include "MySQL.h"
#include "STSkillScript.h"
#include "DNFLexWrapper.h"
#include "DNFFunctionLib.h"
#include "CStreamBuffer.h"   // 权威 CStreamBuffer（ORIG 0x086269be 域，全局 W 符号）

// ---- ORIG 编码转换 / 脚本解析辅助（真实符号） ----
// toMbcs/getCharacterJob 声明唯一出处：SyncScriptDeps.h（sync_script.h 尾部已 include）。

#include "CDataManager.h"   // G_CDataManager()->m_randomOption 具名访问（CDataManager.h:3081，ORIG +0x4e10）

// ---- ORIG 全局数据（唯一定义点在 InitScriptProducers_3.cpp，ORIG B 0x095000a0）----
// insert_old_equip_info_to_db 的 ORIG 0x08624d5d 直接 `movl $&g_equipmentStatInfoMap`
// 遍历该全局（成员 m_oldEquipMap 全仓无填充点 ⇒ 旧实现恒空，属数据源真 bug）。
// 定义方以 C 链接发射未修饰符号名（C++ 全局变量名本就不入 mangling），
// 此处按权威类型作 C++ 声明引用（非 extern "C" 桥，§4.5 合规）。
extern std::map<unsigned int, STEquipmentStatInfo> g_equipmentStatInfoMap;

namespace
{
// 保留 ORIG 作业名（EUC-KR 字节，见 GetUserJob 0x861d2dc 常量表）。
const char* GetJobString(int job)
{
    switch (job)
    {
    case 0:
        return "\xb1\xcd\xb0\xcb\xbb\xe7";   // 귀검사
    case 1:
        return "\xb0\xdd\xc5\xf5\xb0\xa1";   // 격투가
    case 2:
        return "\xb0\xc5\xb3\xca";           // 거너
    case 3:
        return "\xb8\xb6\xb9\xfd\xbb\xe7";   // 마법사
    case 4:
        return "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";  // 프리스트
    case 5:
        return "\xbf\xa9\xb0\xc5\xb3\xca";   // 여거너
    case 6:
        return "\xb5\xb5\xc0\xfb";           // 도적
    case 7:
        return "\xb3\xb2\xb0\xdd\xc5\xf5\xb0\xa1";  // 남격투가
    default:
        return "";
    }
}

const char* GetAttachTypeString(int type)
{
    switch (type)
    {
    case 0:
        return "\xc0\xda\xc0\xaf";           // 검사
    case 1:
        return "\xb1\xb3\xc8\xaf";           // 격가
    case 2:
        return "\xb1\xb3\xc8\xaf\xbb\xe8\xc1\xa6";  // 격가프리
    case 3:
        return "\xb9\xd0\xba\xc0";           // 마공
    case 5:
        return "\xb0\xe8\xc1\xa4";           // 계열
    default:
        return "";
    }
}

const char* GetAttackElementString(int element)
{
    switch (element)
    {
    case 0:
        return "\xc8\xad";                   // 화
    case 1:
        return "\xbc\xf6";                   // 수
    case 2:
        return "\xbe\xcf";                   // 암
    case 3:
        return "\xb8\xed";                   // 명
    case 4:
        return "\xb9\xab";                   // 무
    default:
        return "";
    }
}


}

// ===================== CSyncScript 实现 =====================

namespace sync_script
{

// [B01 铁律1] 原此处 4 个 `g_*InfoTable` 为**发明符号**（ORIG nm 无此名）+ 与前缀
// `_ZN11sync_script18g_dungeonInfoTableE` 形 GI 键名一并删除。ORIG 真名与定义点：
//   g_stDungeonScriptFileList        B 0x94f6ea0 (0x18)  → InitScriptProducers_4.cpp:156
//   g_stMonsterScriptFileList        B 0x94ff260 (0x18)  → InitScriptProducers_4.cpp:499
//   g_questScriptFileList            B 0x9500480 (0x18)  → InitScriptProducers_4.cpp:157
//   g_staSkillScriptFileList[11]     B 0x9500820 (0x108) → InitScriptProducers_4.cpp:185
// 全仓消费点（CListTables.cpp:56-59/460/778/1082/1710、GameMain.cpp:1826-1828、
// STDungeonScript.cpp:23、STMonsterScript.cpp:46、STQuestScript.cpp:63、STSkillScript.cpp:34）
// 早已按真名引用，本 4 名 0 引用（841 .o 的 `nm -u` 扫描零命中）⇒ 删定义零链接影响。

// ---- CStringMaker 实现（ORIG 各符号均为 W 内联；唯一定义发射点） ----

CStringMaker::CStringMaker() {}                       // C1 0x08375f58
CStringMaker::CStringMaker(const CStringMaker& o)     // copy C1 0x0837f594
    : m_str(o.m_str)
{
}
CStringMaker::~CStringMaker() {}                      // D1 0x0837f580

void CStringMaker::operator()(int v)                  // 0x08375f6c "%d"
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    sprintf(num, "%d", v);
    if (!m_str.empty())
    {
        m_str += ",";
    }
    m_str += num;
}

void CStringMaker::operator()(float v)                // 0x08626cd0 "%f"
{
    // b28d-35：ORIG 缓冲 0xff 字节为 4.1 内联三段清零（对齐+字+双字 rep stos）；
    //   `memset` 调用形在 4.4 恒 call。按 P214③/P253 改 `={0}` 复现内联 rep stos。
    char num[0xff] = {0};
    sprintf(num, "%f", v);
    if (!m_str.empty())
    {
        m_str += ",";
    }
    m_str += num;
}

void CStringMaker::operator()(SkillUseItem v)         // 0x08626d98 "%d,%d,%d"
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    sprintf(num, "%d,%d,%d", v.m_index, v.m_useLevel[0], v.m_useLevel[1]);
    if (!m_str.empty())
    {
        m_str += ";";   // ORIG 分隔串 @0x8ce2df7 为 ';'
    }
    m_str += num;
}

void CStringMaker::operator()(EquipmentSkillDataUp v) // 0x08626e6e 七个 %d
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    sprintf(num, "%d,%d,%d,%d,%d,%d,%d", v.m_job, v.m_v4, v.m_v8,
            v.m_percent, v.m_v10, v.m_key, v.m_module);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}

void CStringMaker::operator()(LevelUpSkill v)         // 0x08626f7a job==0xb→99
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（P75 跨版本 vararg 装载时刻）。
    // ORIG 0x08626f7a：76=76，唯一残差 = sprintf 前两个 vararg 先装 %ecx/%edx、
    // job 后读 %eax，再统一压栈；4.4 恒边求边压（cast/裸形实测同形，ndiff 恒 5，
    // 全为标签+装载序）。语义等价。
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    // b28d-35：ORIG 0x08626f7a 为 then-first if/else（`cmp $0xb; jne <else>;
    //   movl $0x63,-0xc; jmp` 后 else `mov 0xc(%ebp),%eax; mov %eax,-0xc`）；
    //   三目形产镜像块序（`je` + 常量块在后）⇒ 改 if/else。
    int job;
    if (v.m_job == 0xb)
        job = 0x63;
    else
        job = v.m_job;
    sprintf(num, "%d,%d,%d", job, v.m_field4, v.m_field8);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}

void CStringMaker::operator()(ItemAuraInfo v)         // 0x08627066 "%d,%d,%d,%d"
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    // ORIG 参数栈：v+0 int / (uchar)v+4 / float v+8（vararg 提升 double）/
    // v+c int，fmt "%d,%d,%d,%d"。
    sprintf(num, "%d,%d,%d,%d", v.m_field0, (int)v.m_field4, v.m_float8,
            v.m_fieldc);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}

void CStringMaker::operator()(std::vector<float>& values)  // 0x08627146
{
    if (values.empty())
    {
        return;
    }
    if (!m_str.empty())
    {
        m_str += ";";
    }
    // b28d-35：ORIG 0x08627146 的 num 缓冲在循环**外**（内联清零位于 m_str
    //   检查之后、begin() 之前，循环体内仅 sprintf 直接引用 -0x113 槽）⇒
    //   声明移出循环体（`={0}` 复现 4.1 内联三段清零，P214③/P253）。
    char num[0xff] = {0};
    for (std::vector<float>::iterator it = values.begin(); it != values.end();
         ++it)
    {
        // b28d-35：ORIG 循环体语句序 = sprintf → 逗号判断 → += num（0x86271fe
        //   先 deEv+sprintf，0x8627225 才调 begin 比较）⇒ 保持该序。
        sprintf(num, "%f", *it);
        if (it != values.begin())
        {
            m_str += ",";   // 元素间 ORIG 逗号 @0x8ce2dec
        }
        m_str += num;
    }
}

const char* CStringMaker::c_str()                     // 0x08376034
{
    return m_str.c_str();
}

CStringMaker::operator std::string()                  // 0x086272c0
{
    return m_str;
}

// CSyncScript 构造/析构由 CItemList.cpp（ORIG 0x861cbb4 区）提供；本 TU 不重复定义。

bool CSyncScript::GetType(std::string name, ENUM_CHARACTERJOB job, int idx,
                          int& out1, int& out2, bool flag)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；290=290）。
    //   归位要点：`name.compare("creature")==0`（int 直测）、SWEAPONTYPE/SARMORTYPE
    //   改 operator[] 实参处临时、`m_vec1.size()!=0`（免 empty 的 xor 段）、
    //   比较操作数交换使 local.m_field1 先载 %ebx。ORIG @0x0861cf0a。
    if (name.empty())
    {
        return false;
    }
    if (name.find_first_of("[", 0) != 0)
    {
        return false;
    }
    // ORIG 0x0861cf5f..0x0861d037：迭代器区间 erase（begin,begin+1 去 '['、
    // end-1,end 去尾 ']'，__normal_iterator begin/operator+/operator- 各带
    // sret 临时；索引 erase 形指令数不符）。
    name.erase(name.begin(), name.begin() + 1);
    name.erase(name.end() - 1, name.end());
    std::string key = "stackable ";
    // ORIG 0x0861d08c：`call compare("creature"); test %eax,%eax; jne <else>`
    //（int 直测 + 手动 bool 物化）⇒ 源形是 compare(...)==0，不是 operator==
    //（后者产 `test %al,%al; je`，与 ORIG 差 4 条）。
    if (!flag && name.compare("creature") == 0)
    {
        key += name;
    }
    else
    {
        key = name;
    }
    SITEMTYPE local = m_itemTypeMap[key];
    // ORIG 0x0861d10e：weapon 命中 = size()!=0 && vec[0]==local.m_field1。
    // ORIG 为 `call size; test %eax,%eax; je <else>`（32 位直测）⇒ 源形
    // `size() != 0`；`!empty()` 在 4.4 产 `xor $1; test %al` 两段（P197④ 镜像）。
    if (m_vec1.size() != 0 && local.m_field1 == m_vec1[0])
    {
        // ORIG 0x0861d14b：SWEAPONTYPE 是 operator[] 实参处的**临时**
        //（lea -0x1c 直接在实参位构造）；具名局部会把对象排到深槽。
        local = m_weaponMap[SWEAPONTYPE(job, idx)];
    }
    else if (m_vec1.size() >= 2 && local.m_field1 == m_vec1[1])
    {
        // ORIG 0x861d1d5：SARMORTYPE((uchar)idx, key)，key 为 "stackable "/名
        // 组合串（拷贝自 local_50），非去括号原名。
        local = m_armorMap[SARMORTYPE((unsigned char)idx, key)];
    }
    out1 = local.m_field1;
    out2 = local.m_field4;
    return true;
}

const char* CSyncScript::GetURL(const char* str, int len, int idx)
{
    static std::string strUrl;
    strUrl.clear();
    int start = 0;
    int end = 0;
    int i = 0;
    for (i = 0; i < len; ++i)
    {
        if (str[i] == '/')
        {
            start = i + 1;
        }
        if (str[i] == '.')
        {
            end = i;
        }
    }
    strUrl.insert(0, str + start, end - start);
    char tmp[16] = {0};
    sprintf(tmp, "/%05d.png", idx);
    strUrl += tmp;
    return strUrl.c_str();
}

const char* CSyncScript::GetUserJob(ENUM_CHARACTERJOB job)
{
    // ORIG 0x0861d2dc：switch 跳表（cmp $7; ja 边界 + .rodata 8 项表）→
    // EUC-KR 职业名常量（.rodata 0x08ce2e6c.. objdump 实证，与 GetJobString 同
    // 编码），default 为单引号串（0x08ce2e28 = "'"）。逐 case 独立 return。
    switch ((int)job)
    {
    case 0:  return "\xb1\xcd\xb0\xcb\xbb\xe7";                // 귀검사
    case 1:  return "\xb0\xdd\xc5\xf5\xb0\xa1";                // 격투가
    case 2:  return "\xb0\xc5\xb3\xca";                        // 거너
    case 3:  return "\xb8\xb6\xb9\xfd\xbb\xe7";                // 마법사
    case 4:  return "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";        // 프리스트
    case 5:  return "\xbf\xa9\xb0\xc5\xb3\xca";                // 여거너
    case 6:  return "\xb5\xb5\xc0\xfb";                        // 도적
    case 7:  return "\xb3\xb2\xb0\xdd\xc5\xf5\xb0\xa1";        // 남격투가
    default: return "'";
    }
}

bool CSyncScript::insert_artifact_info_to_db(STEquipmentScript* script)
{
    // ORIG 0x08624b8e：未启用直接 return true；启用则向 dnf_artifact_info 写入
    // it_id（script+0）与 m_artifact738（script+0x738 起 11 列）。
    if (m_field0 != 1)
    {
        return true;
    }
    /* 声明序 = ORIG 槽序（p@-0x10 先、db@-0xc 后；4.1/4.4 标量槽向相反，
     * N93 标量例外翻转声明序即可，探针 /tmp/l5_mul4.cpp）。 */
    const STEquipmentScript::artifact_info& art = script->m_artifact738;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)5,
                                                  (ENUM_SERVER_GROUP)0);
    if (!db->set_query(
            "inSert into dnf_artifact_info(it_id, creature_min_level, "
            "physical_attack, magical_attack, skill_consume_mp_rate, "
            "skill_charge_time_rate, skill_overcharge_time_rate, "
            "experience_amount, physical_critical_hit, magical_critical_hit, "
            "stuck) values(%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f)",
            (int)script->m_field0, art.m_minLevel, art.m_physAttack,
            art.m_magAttack, art.m_mpRate, art.m_chargeRate,
            art.m_overChargeRate, art.m_expRate, art.m_physCrit,
            art.m_magCrit, art.m_stuck))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

const char* CSyncScript::GetAttachType(int type)
{
    switch (type)
    {
    case 0:
        return "\xc0\xda\xc0\xaf";           // 검사
    case 1:
        return "\xb1\xb3\xc8\xaf";           // 격가
    case 2:
        return "\xb1\xb3\xc8\xaf\xbb\xe8\xc1\xa6";  // 격가프리
    case 3:
        return "\xb9\xd0\xba\xc0";           // 마공
    case 5:
        return "\xb0\xe8\xc1\xa4";           // 계열
    default:
        return "";
    }
}

const char* CSyncScript::GetAttackElement(EquipmentParameterInfo* param)
{
    switch (param->m_elementalProperty)
    {
    case 0:
        return "\xc8\xad";                   // 화
    case 1:
        return "\xbc\xf6";                   // 수
    case 2:
        return "\xbe\xcf";                   // 암
    case 3:
        return "\xb8\xed";                   // 명
    case 4:
        return "\xb9\xab";                   // 무
    default:
        return "";
    }
}

void CSyncScript::GetLevelUpSkill(
    STEquipmentScript* script,
    EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX vectorIndex)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；178=178）。
    //   归位要点：allocator 改 string ctor 实参处临时（full-expression 末 dtor）、
    //   `compare("")==0`（int 直测+sete）、int 循环变量保 at(i) 值先/add 两步寻址、
    //   m_field8 内联实参升格 %ebx（P0 右值直传）。ORIG @0x0861d472。
    static std::string level_up_skill;
    level_up_skill.clear();
    // ORIG 0x0861d472：无 vectorIndex 值域检查——直接 m_levelUpSkills[idx] 下标
    // 寻址（param_3*0xc 每次 shl 重算，0x40c 基址；idx=2 落 +0x264 即权威头
    // m_avatar264 邻域，ORIG 行为如此，前批“保守 return”系过度防御，已按原码
    // 照抄）。size 落 int 槽、(int)i<n 有符号比较；at(i) 每处独立重读（P48/P157
    // 无 CSE）；每轮 memset(tmp,0,0x400)；job 串经 allocator+string(PKc,alloc)
    // 临时（EH landing pad），非裸 compare。
    int skillCount = (int)script->m_param1c0.m_levelUpSkills[(int)vectorIndex].size();
    // ORIG 0x0861d529：`int i` + 有符号尾测 `setl`；且 at(i) 处 ORIG 为
    // `mov i,%ecx`（值先）+ `add $0xc,%eax` 两步寻址 ⇒ 下标必须是 int 局部
    //（P218：unsigned 下标令 4.4 翻成 this 先 + lea 折叠）。
    for (int i = 0; i < skillCount; ++i)
    {
        level_up_skill += "|";
        // ORIG 0x0861d549..0x0861d5be：allocator 是 string ctor 实参处的**临时**
        // 对象（full-expression 末即 dtor，EH landing 0x861d5a3）；具名局部会把
        // dtor 推迟到循环体末并多出整段 cleanup。
        std::string jobStr(GetUserJob((ENUM_CHARACTERJOB)
            script->m_param1c0.m_levelUpSkills[(int)vectorIndex].at(i).m_job),
            std::allocator<char>());
        // ORIG 0x0861d5d7：`call compare(""); test %eax,%eax; sete %al; test %al,%al`
        //（operator== 形产 bool 直判，缺 sete 段）。
        if (jobStr.compare("") == 0)
        {
            level_up_skill += "\xb0\xf8\xc5\xeb";   // 공용
        }
        else
        {
            level_up_skill += jobStr;
        }
        char tmp[0x400];
        memset(tmp, 0, sizeof(tmp));
        // ORIG 0x0861d62c：m_field8 读入 %ebx 跨第二次 at() call 保活（无槽往返
        // ⇒ P0 右值内联）；具名/register 局部反生 `mov mem,%eax; mov %eax,%ebx`
        // 两段（N18a 向）。
        sprintf(tmp, "| %d| %d",
                script->m_param1c0.m_levelUpSkills[(int)vectorIndex].at(i).m_field4,
                script->m_param1c0.m_levelUpSkills[(int)vectorIndex].at(i).m_field8);
        level_up_skill += tmp;
    }
    level_up_skill.c_str();
}

const char* CSyncScript::GetUsableCharacter(STEquipmentScript* script)
{
    // 正向早返形（ORIG +0x?? `test%al; je L_loop` + `mov $str,%eax; jmp`
    // fall-through 主路径=非零分支；`==0` 负形产 jne 翻转+尾岛换位）。
    if (script->m_fielda0)
        return "\xb8\xf0\xb5\xce";   // 모두
    for (int i = 0; i < 0xb; ++i)
    {
        if (script->m_usable[i] != 0)
        {
            return GetUserJob((ENUM_CHARACTERJOB)i);
        }
    }
    return "";
}

const char* CSyncScript::GetUsableCharacter(STStackableScript* script)
{
    // 正向早返形（ORIG +0x?? `test%al; je L_loop` + `mov $str,%eax; jmp`
    // fall-through 主路径=非零分支；`==0` 负形产 jne 翻转+尾岛换位）。
    if (script->m_fielda0)
        return "\xb8\xf0\xb5\xce";   // 모두
    for (int i = 0; i < 0xb; ++i)
    {
        if (script->m_usable[i] != 0)
        {
            return GetUserJob((ENUM_CHARACTERJOB)i);
        }
    }
    return "";
}

bool CSyncScript::LoadItemInfoMasterScript(const char* path)
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（N7/N18a 单条寄存器差 + 尾 nop）。
    // ORIG @0x08625064：446=446，P167 调用点差集 CLEAR（80=80），全帧槽集全等
    // （帧 0x90=0x90；type -0x1c / subType -0x18 / value -0x14 / extra -0x10 /
    // job -0xc + line/str/name 三 string 槽全等）。唯一残差 =
    // SWEAPONTYPE 键构造处 ORIG `mov -0x18(%ebp),%eax` 而 4.4 取 %edx（1 条
    // 寄存器名分配差）与 ORIG 尾对齐 nop（§6.1 豁免）。语义等价。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string line;
    std::string str;
    // ORIG 实证（0x08625064，size=1585）：本函数只有 3 个 _ZNSsC1Ev 调用点
    // （line/str/name）与 14 个 _ZNSsD1Ev。name 为**函数作用域单例**并在三个分支
    // 间复用（ScanStr 成功时整体覆盖），而非每分支各建一个具名局部；
    // 分支内声明会额外产生 2 个 _ZNSsC1Ev + 4 个 _ZNSsD1Ev（含 unwind 清理）。
    std::string name;
    // [b28d-solo-sync_script] P212：ORIG 槽使用计数 = -0x1c:14 / -0x18:8 /
    //   -0x14:6 / -0xc:2 ⇒ value/type/subType/extra/job 为**函数级共享**标量
    //   （分支内各声明一份会让 4.4 逐分支独立占槽，全帧槽号整体深移 0x28）。
    //   声明序 = ORIG 槽深→浅（type -0x1c / subType -0x18 / value -0x14 /
    //   extra -0x10 / job -0xc）。
    int type;
    int subType;
    int value;
    int extra;
    int job;
    // ORIG 0x08625093：循环条件在**循环体首部**以 `xor $1 / test / jne` 求反后
    // break（多出 1 个 xor + 2 个 nop），即 while(true){ if(!ScanType) break; }，
    // 而非 while(ScanType(...)) 的底部条件形态。
    while (true)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[master type]")
        {
            // ORIG 0x086250b6 段：master 族分支与 weapon/armor/item 的 type 共用
            // 同一槽 -0x1c（4.1 互斥作用域复用）⇒ 源侧复用函数级 type 变量，
            // 4.4 按首次使用序即得同槽（P212/N98 联判）。
            type = ScanInt((bool*)0);
            ScanStr(&str);
            m_map64[(unsigned char)type] = str;
        }
        else if (line == "[type addition]")
        {
            type = ScanInt((bool*)0);
            m_vec1.push_back((unsigned char)type);
        }
        else if (line == "[reverse desc]")
        {
            type = ScanInt((bool*)0);
            m_vec2.push_back((unsigned char)type);
        }
        else if (line == "[material type]")
        {
            type = ScanInt((bool*)0);
            ScanStr(&str);
            m_map7c[(unsigned char)type] = str;
        }
        else if (line == "[weapon type]")
        {
            ScanStr(&str);
            type = ScanInt((bool*)0);
            subType = ScanInt((bool*)0);
            value = ScanInt((bool*)0);
            ScanStr(&name);
            job = getCharacterJob(str.c_str());
            // ORIG 0x086252d7..0x08625318：先构造右值 SITEMTYPE(local_64)，
            // 再构造下标键 SWEAPONTYPE(local_54)——键不是具名局部，
            // 而是 operator[] 实参处的临时（gcc 右→左求值）。
            m_weaponMap[SWEAPONTYPE((ENUM_CHARACTERJOB)job, subType)] =
                SITEMTYPE(false, (unsigned char)type,
                          (unsigned char)subType,
                          (unsigned short)value, name);
        }
        else if (line == "[armor type]")
        {
            ScanStr(&str);
            type = ScanInt((bool*)0);
            subType = ScanInt((bool*)0);
            value = ScanInt((bool*)0);
            ScanStr(&name);
            // ORIG 0x08625408..0x0862545e：同上，SITEMTYPE(local_4c) 先建，
            // SARMORTYPE(local_3c) 作为 operator[] 处的临时键后建。
            m_armorMap[SARMORTYPE((unsigned char)subType, str)] =
                SITEMTYPE(false, (unsigned char)type,
                          (unsigned char)subType,
                          (unsigned short)value, name);
        }
        else if (line == "[item type]")
        {
            ScanStr(&str);
            type = ScanInt((bool*)0);
            subType = ScanInt((bool*)0);
            value = ScanInt((bool*)0);
            extra = ScanInt((bool*)0);
            ScanStr(&name);
            m_itemTypeMap[str] = SITEMTYPE(type != 0, (unsigned char)subType,
                                           (unsigned char)value,
                                           (unsigned short)extra, name);

        }
    }
    return true;
}

bool CSyncScript::insert_skill_index(STSkillScript* script, int job)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；126=126）。
    //   归位要点：`char name[0xff] = {0}`（0xff 是数组大小，旧 memset 0x100 系误读
    //   展开计数）+ `char escaped[0x2fd] = {0}`（ORIG 765 字节、基址 -0x428 对齐）。
    //   ORIG @0x0862262c。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    // ORIG 0x0862266f：name 基址 -0x12b、`mov $0xff,%ebx` 三段内联清零 ⇒
    // `char name[0xff] = {0}`（0xff 是**数组大小**，非 memset 0x100；旧转写把
    // 展开计数误当 memset 长度）。ORIG 0x086226ca：escaped 基址 -0x428、
    // `rep stos $0xbf` + 尾 1 字节 = 765 = 0x2fd 字节 ⇒ `char escaped[0x2fd]`；
    // 4.4 对 `= {0}` 同样三段展开（显式 memset 恒 call）。
    char name[0xff] = {0};
    char escaped[0x2fd] = {0};
    if (!CodePage::script2Database((char*)script->m_str4.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6e0, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), name, __PRETTY_FUNCTION__, 0x6e0);
    }
    db->escape_string(escaped, name);
    db->set_query("inSert into skill_index(job, skill_idx, skill_name) values(%d, %d, '%s')",
                  job, script->m_field0, escaped);
    // ORIG 0x086227d1：exec 结果保 bool 比较控制流（P101）。
    if (!db->exec(true))
        return false;
    return true;
}

bool CSyncScript::truncate_skill_index()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_index");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::trancate_skill_info()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_info; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_skill_info(STSkillScript* script, int job, int type)
{
    // [b28d-solo-sync_script] 判词：**语义完成、byte 层 N 封顶**（§6.2 三件套）。
    //   ORIG @0x08622e08：①gap = 1（1401 vs 1400）；②P167 调用点差集 CLEAR
    //   （258=258，含全部 CStringMaker/for_each/GetEscapeString/CStreamBuffer 调用）；
    //   ③槽位集残差 = 浅槽临时区整体 +0x1c（cMyTrace/string 物化序；牵动 0xb000
    //   缓冲基址 -0xa924 vs ORIG -0xa908 全部 29 处，整帧重排风险>收益，Main 已批复
    //   挂起）+ for_each 临时物化 1-2 处。归 N82（4.1 不按 lifetime 复用 vs 4.4 复用）
    //   + N78（memset 0xb000 展开形）。逐块人工比对：Ghidra C 段与重建代码逐句对应，
    //   无缺失分支/字段/调用。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG：localVec = m_vec44/m_vec50 交替展开（pre_required_skill 段）。
    std::vector<int> localVec;
    for (unsigned int i = 0; i < script->m_vec44.size(); ++i)
    {
        localVec.push_back(script->m_vec44[i]);
        localVec.push_back(script->m_vec50[i]);
    }
    // GetEscapeString 输出区：ORIG 栈上单块 0xb000 缓冲（入口即 memset），
    // 按 escCount*0x1000 步进取槽（共 7 次）；name/name2 转码缓冲各 0x400。
    char escapedBuf[0xb000];
    memset(escapedBuf, 0, sizeof(escapedBuf));
    int escCount = 0;
    #define SKILL_ESC_SLOT (escapedBuf + escCount++ * 0x1000)
    char name[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_str4.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7ff, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_str4.c_str(),
            __PRETTY_FUNCTION__, 0x7ff);
    }
    char name2[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_str8.c_str(), name2))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x807, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_str8.c_str(),
            __PRETTY_FUNCTION__, 0x807);
    }
    // GetEscapeString 输出区声明见函数头；流对象在 name2 处理后构造。
    CStreamBuffer sb;
    // 1)job_index 2)skill_index(m_field0) 3)module_type(type)
    sb << job;
    sb << ", " << script->m_field0;
    sb << ", " << type;
    // 4)name 5)name2（转码后 escape；GetEscapeString 结果写入槽缓冲）
    // [b28e] ORIG 0x086230ed 段形：`call GetEscapeString; mov %eax,%ebx;` 后紧跟
    // `<< ", '"` → `<< %ebx` → `<< "'"` —— **直接用返回值**、全程寄存器保活，
    // 无 escName 具名指针槽（P0/P183；具名槽令 4.4 多发 `mov %eax,-0x38(%ebp)` +
    // 回读并把 `add %edx,%eax` 折成 `lea (%edx,%eax,1)`，帧 +0x20）。
    sb << ", '" << GetEscapeString(db, name, SKILL_ESC_SLOT) << "'";
    sb << ", '" << GetEscapeString(db, name2, SKILL_ESC_SLOT) << "'";
    // 6)basic_explain(+0x10 先于 +0xc) 7)skill_explain(+0xc)
    sb << ", '" << GetEscapeString(db, script->m_str10.c_str(), SKILL_ESC_SLOT)
       << "'";
    sb << ", '" << GetEscapeString(db, script->m_strc.c_str(), SKILL_ESC_SLOT)
       << "'";
    // 8)m_vec18
    {
        // ORIG 站点形态：for_each 返回临时 + 实参临时同在一个 full-expression
        // 内，两个 D1 均在 << 链之后（0862330c→0862332f/08623351）。
        sb << ", '"
           << std::for_each(script->m_vec18.begin(), script->m_vec18.end(),
                            CStringMaker()).c_str()
           << "'";
    }
    // 9)+0x5c 10)+0x60
    sb << ", " << script->m_5c;
    sb << ", " << script->m_60;
    // 11)m_arr7c[6]
    {
        sb << ", '" << std::for_each((int*)script->m_arr7c, (int*)script->m_arr7c + 6, CStringMaker()).c_str() << "'";
    }
    // 12)growType[0..4].m_8 五段分号连接（单引号包裹）
    {
        // ORIG：五段临时对象同在一个 full-expression（0xa0..0xc8 栈槽），
        // GCC 右到左求值 ⇒ for_each 构造序 +0xdc(growType[4])→+0x9c([0])
        // （0862347f..08623645），每段 for_each 后立即 c_str；10 个临时
        // 在 << 链全部完成后成对（res_i, arg_i，i=0..4）析构（086236e8+）。
        sb << ", '"
           << std::for_each(script->m_growType[0].m_8,
                            script->m_growType[0].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[1].m_8,
                            script->m_growType[1].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[2].m_8,
                            script->m_growType[2].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[3].m_8,
                            script->m_growType[3].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[4].m_8,
                            script->m_growType[4].m_8 + 2, CStringMaker()).c_str()
           << "'";
    }
    // 13)m_vec1dc（lea 0x1dc@08623889）14)m_vecf4（lea 0xf4@0862399a）
    {
        sb << ", '" << std::for_each(script->m_vec1dc.begin(), script->m_vec1dc.end(), CStringMaker()).c_str() << "'";
    }
    {
        sb << ", '" << std::for_each(script->m_vecf4.begin(), script->m_vecf4.end(), CStringMaker()).c_str() << "'";
    }
    // 15)m_useItems11c（"%d,%d,%d"）
    {
        sb << ", '" << std::for_each(script->m_useItems11c.begin(), script->m_useItems11c.end(), CStringMaker()).c_str() << "'";
    }
    // 16)+0x30 17)+0x34
    sb << ", " << script->m_30;
    sb << ", " << script->m_34;
    // 18)localVec（pre_required_skill）
    {
        sb << ", '" << std::for_each(localVec.begin(), localVec.end(), CStringMaker()).c_str() << "'";
    }
    // 19)m_f100[2]（for_each<float*> +0x100..+0x108，"%f"）
    {
        sb << ", '" << std::for_each((float*)script->m_f100, (float*)script->m_f100 + 2, CStringMaker()).c_str() << "'";
    }
    // 20)m_arr138[2]
    {
        sb << ", '" << std::for_each((int*)script->m_arr138, (int*)script->m_arr138 + 2, CStringMaker()).c_str() << "'";
    }
    // 21)m_i108[2..3]（for_each<int*> +0x110..+0x118）
    {
        sb << ", '" << std::for_each((int*)script->m_i108 + 2, (int*)script->m_i108 + 4, CStringMaker()).c_str() << "'";
    }
    // 22)icon：esc(str160[0]),arr168[0],esc(str160[1]),arr168[1]
    // [b28e] ORIG 0x08623f30/0x08623f78：**两次 GetEscapeString 都直用返回值**
    // （先 %ebx=m_str160[1] 的结果、再 %esi=m_str160[0] 的结果，全无具名槽）；
    // 链中位置为 icon0 第 3 位 / icon1 第 6 位，GCC 右→左求值恰好复现 ORIG 的
    // 「先算 m_str160[1]、再 arr168[0]、再 m_str160[0]」调用序。
    sb << ", '" << GetEscapeString(db, script->m_str160[0].c_str(), SKILL_ESC_SLOT)
       << "," << script->m_arr168[0] << ","
       << GetEscapeString(db, script->m_str160[1].c_str(), SKILL_ESC_SLOT)
       << "," << script->m_arr168[1] << "'";
    // 23)command_key_explain：esc(m_str194)（同 P183：返回值直用，无具名槽）
    sb << ", '"
       << GetEscapeString(db, script->m_str194.c_str(), SKILL_ESC_SLOT) << "'";
    // 24)skill_command_advantage：m_240,m_244
    sb << ", '" << script->m_240 << "," << script->m_244 << "'";
    // 25)static_data：m_vec198（lea 0x198@086240e4）
    {
        sb << ", '" << std::for_each(script->m_vec198.begin(), script->m_vec198.end(), CStringMaker()).c_str() << "'";
    }
    // 26)level_info：m_vec1a8（外层 for_each<vector<float>*> → operator()(vector<float>&)）
    {
        sb << ", '" << std::for_each(script->m_vec1a8.begin(), script->m_vec1a8.end(), CStringMaker()).c_str() << "'";
    }
    // 27)start_cool_time：m_21c,m_220
    sb << ", '" << script->m_21c << "," << script->m_220 << "'";
#undef SKILL_ESC_SLOT
    db->set_query("inSert into skill_info(job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time) values(%s)",
                  sb.c_str());
    return db->exec(true);
}

bool CSyncScript::insert_quest_category(QuestScript* script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    // b28d-35：ORIG 帧 0x24c 且内联 rep stos 计数 0x40 dword = 0x100 字节
    //   （P116/P253）⇒ 两缓冲各 0x100 字节（原 0x40 使帧仅 0xcc 且 escaped
    //   在长名时有溢出风险）；`={0}` 复现内联清零（P214③）。
    char name[0x100] = {0};
    if (!CodePage::script2Database((char*)script->m_name.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6a4, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_name.c_str(),
            __PRETTY_FUNCTION__, 0x6a4);
    }
    // b28d-35：ORIG 第二次 rep stos（-0x22c = escaped）位于 script2Database
    //   之后、escape_string 之前 ⇒ 声明位置后置以复现清零点。
    char escaped[0x100] = {0};
    db->escape_string(escaped, name);
    db->set_query("inSert into quest_category(quest_idx, quest_name) values(%d, '%s')",
                  script->m_index, escaped);
    // b28d-35：ORIG 尾部为 `call exec; xor $1,%eax; test %al; je; mov $0; jmp;
    //   mov $1` ⇒ P101 形（`return db->exec(true);` 会被 4.4 折叠掉取反段）。
    if (!db->exec(true))
        return false;
    return true;
}

bool CSyncScript::truncate_quest_category()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table quest_category");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_charac_quest_category(unsigned int origin,
                                               unsigned int mapped)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into charac_quest_ref(origin_idx, mapped_idx) values(%d, %d)",
                  origin, mapped);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_charac_quest_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_quest_ref; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_exp_level_to_db(unsigned int exp, int level)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into exp_level_ref(exp, lev) values(%u, %d)", exp, level);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_exp_level_ref_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table exp_level_ref");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_item_parts_set(int index, STEquipmentPartSetPiece& piece)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("inSert into item_part_set(part_set_index, part_name, part_type, part_grade, part_rarity) values(%d, '%s',%d,%d,%d)",
                  index, piece.m_name.c_str(), piece.m_field4, piece.m_field8,
                  piece.m_fieldc);
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::trancate_item_parts_set()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table item_part_set");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_aicharacter_info_to_db(int idx,
                                                const STAICharacterCommonScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    /* w2 源形（4.4 双 je 同目标汇聚块 = ORIG 形；探针 /tmp/l5_cnd3.cpp w2）。
     * ORIG escaped 清零 rep stos（N78），显式 {0} 初始化复现。 */
    if (idx != 0 && script.m_str4.length() != 0)
    {
        static char aicharacter_info_name[0xf0];
        memset(aicharacter_info_name, 0, sizeof(aicharacter_info_name));
        char escaped[0x400] = { 0 };
        // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
        if (!CodePage::script2Database((char*)script.m_str4.c_str(), escaped))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xb95, 5)(
                "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                CodePage::database(), script.m_str4.c_str(),
                __PRETTY_FUNCTION__, 0xb95);
        }
        db->escape_string(aicharacter_info_name, escaped);
        db->set_query("inSert into dnf_aicharacter_info(idx, ai_name_kr) values(%d, '%s')",
                      idx, aicharacter_info_name);
        /* t2 形（探针 /tmp/l5_ret.cpp）：ORIG exec 后 xor $1/test 物化；
         * exec 成功 fall-through 与块外 return true 共享单一尾块（ORIG L_end 合一）。 */
        if (!db->exec(true))
            return false;
    }
    return true;
}

bool CSyncScript::truncate_aicharacter_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_aicharacter_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_town_info_to_db(const TownScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    /* w2 源形（4.4 双 je 同目标汇聚块 = ORIG 形；探针 /tmp/l5_cnd3.cpp w2）。
     * ORIG escaped 清零 rep stos（N78），显式 {0} 初始化复现。 */
    if (script.m_field0 != 0 && script.m_str4.length() != 0)
    {
        static char town_name[0xf0];
        memset(town_name, 0, sizeof(town_name));
        char escaped[0x400] = { 0 };
        // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
        if (!CodePage::script2Database((char*)script.m_str4.c_str(), escaped))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbc2, 5)(
                "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                CodePage::database(), script.m_str4.c_str(),
                __PRETTY_FUNCTION__, 0xbc2);
        }
        db->escape_string(town_name, escaped);
        db->set_query("inSert into dnf_town_info(idx, town_name_kr) values(%d, '%s')",
                      script.m_field0, town_name);
        /* t2 形（探针 /tmp/l5_ret.cpp）：ORIG exec 后 xor $1/test 物化；
         * exec 成功 fall-through 与块外 return true 共享单一尾块（ORIG L_end 合一）。 */
        if (!db->exec(true))
            return false;
    }
    return true;
}

bool CSyncScript::truncate_town_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_town_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_monster_info_to_db(const STMonsterScript& script)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    /* w2 源形（4.4 双 je 同目标汇聚块 = ORIG 形；探针 /tmp/l5_cnd3.cpp w2）。
     * ORIG escaped 清零 rep stos（N78），显式 {0} 初始化复现。 */
    if (script.m_index != 0 && script.m_name.length() != 0)
    {
        static char monster_name[0xf0];
        memset(monster_name, 0, sizeof(monster_name));
        char escaped[0x400] = { 0 };
        // ORIG：script2Database 输出到 escaped（栈），escape_string 目标与 set_query 用 static 缓冲。
        if (!CodePage::script2Database((char*)script.m_name.c_str(), escaped))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xb63, 5)(
                "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                CodePage::database(), script.m_name.c_str(),
                __PRETTY_FUNCTION__, 0xb63);
        }
        db->escape_string(monster_name, escaped);
        db->set_query("inSert into dnf_monster_info(idx, mon_name_kr) values(%d, '%s')",
                      script.m_index, monster_name);
        /* t2 形（探针 /tmp/l5_ret.cpp）：ORIG exec 后 xor $1/test 物化；
         * exec 成功 fall-through 与块外 return true 共享单一尾块（ORIG L_end 合一）。 */
        if (!db->exec(true))
            return false;
    }
    return true;
}

bool CSyncScript::truncate_monster_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_monster_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_creature_info_to_db(unsigned int id,
                                             const CreatureScript* script,
                                             const char* param3,
                                             const char* param4)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；487=487）。
    //   归位要点：artifactSlot 循环改具名数组成员视图 `script->m_artifactSlots[i]`
    //   （P291/P311：裸 `(&m_id)[20+i]` 在 4.4 恒 shl+add 三段，具名数组产 scaled
    //   disp 折叠）。ORIG @0x08624414。
    if (m_field0 != 1)
    {
        return true;
    }
    static const int BUFSIZE = 0x100;  // ORIG _ZZN11sync_script11CSyncScript26insert_creature_info_to_db…E7BUFSIZE
    // ORIG：artifact_slot 列 = '0'+(R/G/B 三槽标志) 的 3 字符串
    // （CreatureScript+0x50/0x54/0x58，asm 0x08624437..0x0862445d，
    // 低字节存 (v+'0')&0xff；缓冲 movl$0 预清零即 NUL 结尾）。
    // ORIG local_5c 为 `movl $0x0` 单指令清零（聚合初始化），不是 memset 调用。
    char artifactSlot[4] = {0};
    // ORIG：直接按 +0x50+i*4 读三槽标志（无中间数组）；上界写成 sizeof-1
    //（ORIG 0x0862444f 为 `mov i,%eax; cmp $0x2,%eax; setbe`：有符号 i 与
    // size_t 上界比较产生的转换临时，非 `cmpl $imm,mem` 直比形态）。
    for (int i = 0; i < sizeof(artifactSlot) - 1; ++i)
    {
        // ORIG 0x0862445d `add $0x14,%ecx; mov (%edx,%ecx,4),%edx`：以结构首
        // 字段 m_id(+0x00) 为基的 int 索引 [20+i]（= +0x50 槽区）。从
        // &m_artifactSlotRed 起算会被 4.4 折成 lea 0x50+shl 两段（N18 向）。
        artifactSlot[i] = (char)(script->m_artifactSlots[i] + '0');
    }
    // ORIG 0x0862448c/0x08624562/0x08624635：三处均为
    //   CStringMaker::CStringMaker(临时 functor)
    //   → std::for_each<__normal_iterator<int const*,vector<int>>, CStringMaker>
    //   → CStringMaker::operator std::string()（sret 直接产出结果串）
    //   → ~CStringMaker ×2
    // 即 ORIG 无 c_str() → std::string(const char*, allocator) 的二次构造；
    // 走 makeString(...).c_str() 会多出 _ZNSaIcEC1Ev ×3 / _ZNSsC1EPKcRKSaIcE ×3
    // （及 _ZNSaIcED1Ev ×6）这些 ORIG 不存在的 eager 构造调用点。
    const std::string piercingStr =
        std::for_each(script->m_piercing.begin(), script->m_piercing.end(),
                      CStringMaker());
    const std::string skillLevelValuesStr =
        std::for_each(script->m_skillLevelValues.begin(),
                      script->m_skillLevelValues.end(), CStringMaker());
    const std::string overSkillLevelValuesStr =
        std::for_each(script->m_overSkillLevelValues.begin(),
                      script->m_overSkillLevelValues.end(), CStringMaker());
    // ORIG：values 区 11 个 dword 就地清零（rep stos 0xb），非 memset 调用。
    const char* values[11] = {0};
    values[0] = script->m_name.c_str();
    values[1] = artifactSlot;
    values[2] = param3;
    values[3] = param4;
    values[4] = piercingStr.c_str();
    // ORIG：skill_name/skill_desc/overskill_name/overskill_desc ←
    // m_skillString（vector<string> @+0x128）前 4 项，越界项用 ""；
    // ORIG 为 4 段展开（size() ×4 + at() ×4），非循环 + operator[]。
    values[5] = script->m_skillString.size() == 0
                    ? "" : script->m_skillString.at(0).c_str();
    values[6] = script->m_skillString.size() < 2
                    ? "" : script->m_skillString.at(1).c_str();
    values[7] = script->m_skillString.size() < 3
                    ? "" : script->m_skillString.at(2).c_str();
    values[8] = script->m_skillString.size() < 4
                    ? "" : script->m_skillString.at(3).c_str();
    values[9] = skillLevelValuesStr.c_str();
    values[10] = overSkillLevelValuesStr.c_str();
    // ORIG：escaped 区 0x2c0 个 dword 就地清零（= 11*0x100 字节），非 memset 调用；
    // GetDBHandle 在清零之后才取。
    char escaped[11][BUFSIZE] = {{0}};
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG 0x08624a0e：同为 `mov i,%eax; cmp $0xa,%eax; setbe`（size_t 上界）。
    for (int i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    {
        GetEscapeString(db, values[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_creature_info(it_id, creature_id, creature_name, skill_recovery_time, overskill_recovery_time, artifact_slot, learn_overskill_level, skill_info, overskill_info, piercing, skill_name, skill_desc, overskill_name, overskill_desc, skill_level_values, overskill_level_values, evolution_creature_id, evolution_level) values(%u, %d, '%s', %d, %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d)",
            id, script->m_id, escaped[0], script->m_field1c,
            script->m_field20, escaped[1], script->m_learnOverSkillLevel,
            escaped[2], escaped[3], escaped[4], escaped[5], escaped[6],
            escaped[7], escaped[8], escaped[9], escaped[10],
            script->m_evolutionCreatureId, script->m_evolutionLevel))
    {
        return false;
    }
    // ORIG 0x08624a5f：exec 结果经 `xor $1 / test` 分支回填 0/1，
    // 而非 `return db->exec(true)` 的直接搬运。
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::truncate_creature_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_creature_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_one_item_info_master(unsigned char masterNo,
                                              unsigned short subNo,
                                              const char* name,
                                              const char* explain)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；84=84）。
    //   归位要点：P310 翻案——循环变量改 int + 显式 `(unsigned int)i < 2u`
    //   复现 ORIG 两段寄存器尾测 `mov -0xc; cmp $1; setbe`。ORIG @0x08625c54。
    static const int BUFSIZE = 0x64;  // ORIG _ZZN11sync_script11CSyncScript27insert_one_item_info_master…E7BUFSIZE
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    const char* names[2];
    // b28d-35：ORIG 在 db 取值后先对 names[0]/names[1] 各发一次
    //   `movl $0x0,-N(%ebp)`（P198① 预清两句式）⇒ 照抄预清。
    names[0] = 0;
    names[1] = 0;
    names[0] = toMbcs(name);
    names[1] = toMbcs(explain);
    // b28d-35：P253 真 bug 修正——ORIG 0x0864… 的清零为 `mov $0x32,%edx` +
    //   `rep stos`，0x32 是 **dword 计数**（0x32*4 = 0xc8 = sizeof(escaped)）；
    //   原 `memset(escaped, 0, 0x32)` 少清 0x96 字节（escaped[1] 前半未清）。
    //   改 `={0}` 同时复现 4.1 内联 rep stos（P116/P214③/P253）。
    char escaped[2][BUFSIZE] = {0};
    // [b28d-solo-sync_script] P310 翻案：ORIG 0x08625d0a 尾测为
    //   `mov -0xc(%ebp),%eax; cmp $0x1,%eax; setbe %al; test; jne`（两段寄存器形）；
    //   4.4 对 `unsigned int` 局部恒折 `cmpl $0x1,-0xc`（内存直比，少 1 条）⇒
    //   局部改 int + 显式 `(unsigned int)i < 2u` 即逐条复现。
    for (int i = 0; (unsigned int)i < 2u; ++i)
    {
        GetEscapeString(db, names[i], escaped[i]);
    }
    if (!db->set_query(
            "inSert into dnf_item_info_master(master_no, sub_no, name, master_explain) values(%u, %u, '%s', '%s')",
            (unsigned int)masterNo, (unsigned int)subNo, escaped[0], escaped[1]))
    {
        return false;
    }
    // b28d-35：同 insert_quest_category —— ORIG 尾含 `xor $1,%eax; test;
    //   je; mov $0; jmp; mov $1`（P101 形）。
    if (!db->exec(true))
        return false;
    return true;
}

bool CSyncScript::insert_item_info_master_to_db()
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（.o 阶段本地跳转/字符串重定位显示差，
    //   N124 口径）。ORIG @0x0862571a：370=370，P167 CLEAR（60=60），全帧槽集全等
    //   （-0x104/-0xa0/-0x38/-0x34/-0x30/-0x3c/-0x19 计数逐值一致）。
    //   本件修真语义 bug 三处：①name/full 缓冲 = 0x64 字节（ORIG `rep stos $0x19`
    //   是 dword 计数，旧 memset 0x19 只清 25 字节）；②weapon 分支 m_map7c 键取
    //   m_field1（旧 m_field2）且 ORIG 无 name 中间串/第 2 个 sprintf；
    //   ③第三循环内层遍历 m_vec2（+0x10），旧形误用 m_vec1（+0x4）。
    static const int BUFSIZE = 0x64;  // ORIG _ZZN11sync_script11CSyncScript29insert_item_info_master_to_dbEvE7BUFSIZE
    if (m_field0 != 1)
    {
        return true;
    }
    // ORIG 0x0862573d：两个缓冲在**函数头**清零一次（`mov $0x19,%edx; rep stos`
    // = 0x19 dword = 0x64 字节，P116/P253；旧形 memset 0x19 只清 25 字节 = 真
    // bug，且循环内声明导致 4 次 memset call）。槽序 name 浅 -0xa0 / full 深 -0x104
    // ⇒ 声明序 name 先、full 后（4.1 先声明浅）。
    char name[0x64] = {0};
    char full[0x64] = {0};
    for (std::map<SWEAPONTYPE, SITEMTYPE>::iterator it = m_weaponMap.begin();
         it != m_weaponMap.end(); ++it)
    {
        // ORIG 0x086257a9：weapon 分支**只发 1 个 sprintf**——键取
        // `m_map64[it->second.m_field1]`（pair+0x9），无 name 中间串，
        // insert 第三参直接 m_str8.c_str()（P167：旧形多 1×sprintf、
        // 多 1×m_map64::operator[]，且键字段取错 m_field2→m_field1）。
        // [b28f-SweepGroup17 真 bug] ORIG 0x086257af `add $0x64,%eax` = this+0x64
        //   （m_map64），原写 m_map7c（+0x7c）⇒ 武器名查错映射表（armor 分支的
        //   m_map7c 在 0x086258d5 = +0x1bb 处，与此段无关）。
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(),
                it->second.m_str8.c_str());
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4,
                                         it->second.m_str8.c_str(), full))
        {
            return false;
        }
    }
    for (std::map<SARMORTYPE, SITEMTYPE>::iterator it = m_armorMap.begin();
         it != m_armorMap.end(); ++it)
    {
        sprintf(name, "%s%s", m_map7c[it->second.m_field2].c_str(),
                it->second.m_str8.c_str());
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(), name);
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4, name, full))
        {
            return false;
        }
    }
    // ORIG 0x086259ef：matched 在**循环外**预清一次（begin 之前），循环内再置
    // false 一次（P198① 两句式；单句循环内声明只出 1 条 movb）。
    bool matched = false;
    for (std::map<std::string, SITEMTYPE>::iterator it = m_itemTypeMap.begin();
         it != m_itemTypeMap.end(); ++it)
    {
        // ORIG 0x08625a1b：`movzbl 0x4(%eax); xor $0x1,%eax; test %al,%al;
        // jne <nop 岛>`（bool 取反物化）⇒ 源形为否定早退；P223 复现形 =
        // goto 尾部空标签（`if(field==1){…}` 反产 test/je 直跳）。
        if (!it->second.m_field0)
        {
            goto item_next;
        }
        {
            matched = false;
            // ORIG 0x08625a31/0x08625a82：内层遍历 this+0x10 = **m_vec2**
            // （[reverse desc] 表），非 m_vec1（+0x4）——旧形字段取错（真 bug）。
            for (std::vector<unsigned char>::iterator vi = m_vec2.begin();
                 vi != m_vec2.end(); ++vi)
            {
                if (it->second.m_field1 == *vi)
                {
                    matched = true;
                    break;
                }
            }
            sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(),
                    it->second.m_str8.c_str());
            // ORIG 0x08625b0b：`movzbl -0x19; xor $0x1; test %al,%al; je <matched 臂>`
            // ⇒ 源形为 `if (!matched)`（正写 `if (matched)` 在 4.4 恒折
            // `cmpb $1,mem; je` 少 2 条）。
            if (!matched)
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 it->second.m_str8.c_str(),
                                                 full))
                {
                    return false;
                }
            }
            else
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 m_map64[it->second.m_field1].c_str(),
                                                 full))
                {
                    return false;
                }
            }
        }
    item_next:;
    }
    return true;
}

bool CSyncScript::truncate_item_info_master_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_item_info_master"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_old_equip_info_to_db()
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；P167 CLEAR
    //   9=9）。真 bug 修正：ORIG 0x08624d5d 遍历**全局**
    //   g_equipmentStatInfoMap（B 0x095000a0），而非成员 m_oldEquipMap
    //   （全仓无填充点 ⇒ 旧实现恒空、一列不写）。P167 同时给出迭代器形证据：
    //   ORIG 调 operator-> 两次（绑定 second 引用 + 取 first），旧 `(*it).second`
    //   形多 1×operator* 少 2×operator->（MISSING ptEv ×2 / EXTRA deEv ×1）。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (std::map<unsigned int, STEquipmentStatInfo>::iterator it =
             g_equipmentStatInfoMap.begin();
         it != g_equipmentStatInfoMap.end(); ++it)
    {
        // ORIG 0x08624d86：`add $0x4,%eax; mov %eax,-0x1c` ⇒ 源码把 it->second
        // 绑定为具名引用（指针缓存在 -0x1c），随后 33 个字段逐次重读该槽。
        const STEquipmentStatInfo& st = it->second;
        if (!db->set_query(
                "inSert into dnf_old_equip_info(it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state) values(%d, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)",
                it->first, st.m_hpMax, st.m_mpMax,
                st.m_phyAtt, st.m_phyDef,
                st.m_magAtt, st.m_magDef,
                st.m_equipPhyAtt, st.m_equipPhyDef,
                st.m_equipMagAtt, st.m_equipMagDef,
                st.m_refFire, st.m_refWater,
                st.m_refDark, st.m_refLight,
                st.m_refAll, st.m_refSlow,
                st.m_refFreeze, st.m_refPoison,
                st.m_refStun, st.m_refCurse,
                st.m_refBlind, st.m_refLightning,
                st.m_refStone, st.m_refSleep,
                st.m_refBurn, st.m_refWeaponBreak,
                st.m_refBleeding, st.m_refPierce,
                st.m_refStuck, st.m_refConfuse,
                st.m_refHold, st.m_refArmorBreak,
                st.m_refAllState))
        {
            return false;
        }
        if (!db->exec(true))
        {
            return false;
        }
    }
    return true;
}

bool CSyncScript::truncate_old_equip_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_old_equip_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_mapping_equip_info_to_db(std::map<int, int>& mapping)
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    /* f3 源形（begin/end 头部预求值 + jmp 进尾判 = ORIG 形）+ it++ 后置
     * （ORIG ppEi 含临时槽，探针 /tmp/l5_pp.cpp f3）。 */
    std::map<int, int>::iterator it = mapping.begin();
    std::map<int, int>::iterator map_end = mapping.end();
    while (it != map_end)
    {
        if (it->second != 0)
        {
            db->set_query("inSert into equip_mapping_info(equip_idx, mapping_idx) values(%d, %d)",
                          it->second, it->first);
            if (!db->exec(true))
            {
                return false;
            }
        }
        it++;
    }
    return true;
}

bool CSyncScript::truncate_mapping_equip_info_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table equip_mapping_info"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG 0x08625ea2：外层 prefix 为 byte，初值 1，==0xff 终止（cmpb $0xff/setne，
    // 即使用域 [1,0xfe]）；内层 grade 为 byte 0..0x64（cmpb $0x64/setbe ≤）。
    std::string name;   // ORIG：循环外构造一次，循环内 operator=
    for (unsigned char prefix = 1; prefix != 0xff; ++prefix)
    {
        for (unsigned char grade = 0; grade <= 0x64; ++grade)
        {
            name = G_CDataManager()->m_randomOption.getPrefix(prefix, grade, false);
            if (name.length() != 0)
            {
                char converted[0x400] = {0};
                // script2Database 失败仅记日志（ORIG 行 0xb31），继续入库；
                // ORIG 无 escape_string，转码结果直接入 SQL。
                if (!CodePage::script2Database((char*)name.c_str(), converted))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xb31, 5)(
                        "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                        CodePage::database(), name.c_str(), __PRETTY_FUNCTION__,
                        0xb31);
                }
                db->set_query("inSert into random_option_ref(random_option_index, random_option_value, random_option_name) values(%d,%d, '%s')",
                              prefix, grade, converted);
                if (!db->exec(true))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CSyncScript::truncate_random_option_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table random_option_ref"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

bool CSyncScript::insert_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::InsertDescTable();
}

bool CSyncScript::truncate_charac_action_point_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return APSystem::CSyncScript::TruncateDescTable();
}

bool CSyncScript::insert_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::insertItemDescTable();
}

bool CSyncScript::truncate_charac_advance_altar_item_desc()
{
    if (m_field0 != 1)
    {
        return true;
    }
    return advancealtar::SyncScript::truncateItemDescTable();
}

}  // namespace sync_script
