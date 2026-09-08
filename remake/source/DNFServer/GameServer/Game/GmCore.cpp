// GmCore.cpp — game_master 命令体系核心框架（第1批）。
// 还原：IGameMaster/CJunior/CSenior 责任链、CCommand 基类、CParser::parse、
//       CCommandMap 双索引注册表、CGameMasterMgr 分发入口。
// 全部函数体对照 df_game_r ORIG 汇编（docs/class_func_reports/game_master__* 与 objdump）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "PacketDispatcher.h"

extern void trim(std::string& s);   // ORIG _Z4trimRSs @0x0829f068

namespace game_master
{

// ---- IGameMaster ----


void IGameMaster::HandleRequest(CCommand* cmd)  // 0x082a7ed6
{
    // b26 NEAR 封顶:虚调用 vtable 项寄存器 ORIG 4.1 用 %ecx、4.4 恒 %eax
    // (负例 N7,临时/引用/限定名均不可翻),余逐条相等。
    if (m_next != 0)
    {
        m_next->HandleRequest(cmd);             // ORIG：虚调用 vtable[0]
    }
}



void CSeniorGameMaster::HandleRequest(CCommand* cmd)  // 0x084a8c44
{
    // 链尾：校验通过即执行，失败静默丢弃。
    // b26 NEAR 封顶:N7(虚调用 callee 寄存器 %ecx↔%eax 互换)。
    if (cmd->HasSeniorPermission())             // ORIG：vtable[3]，test %al
    {
        cmd->executeTemplate();
    }
}



void CJuniorGameMaster::HandleRequest(CCommand* cmd)  // 0x084a8c06
{
    // b26 NEAR 封顶:N7(虚调用 callee 寄存器 %edx↔%eax 互换)。
    if (cmd->HasJuniorPermission())             // ORIG：vtable[2]，test %al
    {
        cmd->executeTemplate();
    }
    else
    {
        IGameMaster::HandleRequest(cmd);        // 非虚转发 m_next（ORIG 直呼基类实现）
    }
}

// ---- CCommand ----


void CCommand::SetParser(CParser* parser)       // 0x08234972
{
    m_parser = parser;
}

CUser* CCommand::GetUser()                      // 0x084b3fba
{
    return m_parser->m_user;                    // parser+0x04
}

void CCommand::executeTemplate()                // 0x084b3f90
{
    // b26 NEAR 封顶:N7(两处虚调用 callee 寄存器 %edx↔%eax 互换)。
    parse();                                    // ORIG：先 vtable[1]
    execute();                                  // 后 vtable[0]
}

// ---- CParser ----
CParser::CParser()                              // 0x082353a4
    : m_cmdName(), m_args(), m_line(), m_extra()
{
    // ORIG 仅构造 3 个 string 与 +0x11c vector；m_user/m_pos/m_id/m_buf 不初始化
    //（由 GM 入口填充，ORIG 行为如此）。
}

CParser::~CParser()                             // 0x08235446
{
}

// [b28d-solo-GmCore] 具名块级局部（head/tail）令 4.4 槽序反转为 -0xc/-0x10；改
// **匿名表达式临时**（substr 结果直接赋给成员）后 4.4 复现 ORIG 的 -0x10/-0xc
// 与块尾析构点（探针 fF/fH 实证；具名→匿名是槽向旋钮，语义不变）。
void CParser::parse()                           // 0x084a8ab6
{
    m_line = m_buf;                             // operator=(string&, const char*)
    m_pos = static_cast<int>(m_line.find(" ", 0));
    // ORIG：substr 的 sret 临时直接赋给成员（head@-0x10 先、tail@-0xc 后），块尾即毁。
    {
        m_cmdName = m_line.substr(0, m_pos);
    }
    {
        m_args = m_line.substr(m_pos + 1, std::string::npos);
    }
    trim(m_cmdName);
    m_id = atoi(m_cmdName.c_str());
}

// ---- CCommandMap ----
CCommandMap::CCommandMap()                     // 0x082a7f64
    : m_byId(),
      m_byName(10, std::hash<std::string>(), std::equal_to<std::string>())
{
    // ORIG：unordered_map 以 bucket_count=10 直接构造。
}

// [b28d-solo-GmCore] AE | ORIG 0x082a7d88 | 双容器（map+unordered_map）迭代器析构
// 循环逐条全等（含两段 EH landing）；show_func_diff ndiff=0，仅 .o 未解析重定位
// 显示差（call 目标）属 AE 归一范围。
CCommandMap::~CCommandMap()                    // 0x082a7d88
{
    // ORIG 双遍历：id 侧与名字侧各按迭代器取 second，判空后 delete。
    for (std::map<int, CCommand*>::iterator it = m_byId.begin(); it != m_byId.end(); ++it)
    {
        CCommand* cmd = it->second;
        if (cmd != 0)
        {
            delete cmd;
        }
    }
    for (std::unordered_map<std::string, CCommand*>::iterator it = m_byName.begin();
         it != m_byName.end(); ++it)
    {
        CCommand* cmd = it->second;
        if (cmd != 0)
        {
            delete cmd;
            cmd = 0;
        }
    }
}

void CCommandMap::add(int id, CCommand* cmd)    // 0x082a7c76
{
    m_byId.insert(std::make_pair(id, cmd));
}

void CCommandMap::add(std::string name, CCommand* cmd)  // 0x082a7cdc
{
    // ORIG pair ctor 模板 C1IRSsRS3_EEOT_OT0_（引用绑定）直接构造 value_type 临时，
    // 由 insert(RKS5_) 按引用消费 ⇒ 禁 make_pair（值 pair 额外一次拷贝构造+析构）。
    m_byName.insert(std::pair<const std::string, CCommand*>(name, cmd));
}

CCommand* CCommandMap::get(int id)              // 0x084b3fc8
{
    std::map<int, CCommand*>::iterator it = m_byId.find(id);
    if (it != m_byId.end())
    {
        return it->second;
    }
    return 0;
}

CCommand* CCommandMap::get(std::string& name)   // 0x084b402c
{
    std::unordered_map<std::string, CCommand*>::iterator it = m_byName.find(name);
    if (it != m_byName.end())
    {
        return it->second;
    }
    return 0;
}

// ---- CGameMasterMgr ----
IDispatcher* CGameMasterMgr::GetDispatcher(int code)   // 0x084a8a9a
{
    return G_PacketDispatcher()->get_dispatcher(code);
}

void CGameMasterMgr::HandleRequest(CCommand* cmd)      // 0x084a8c6e
{
    // b26 NEAR 封顶:N7(虚调用 callee 寄存器 %ecx↔%eax 互换)。
    m_junior->HandleRequest(cmd);                       // ORIG：junior vtable[0]
}

CCommand* CGameMasterMgr::GetCommand(int id)           // 0x084a8c92
{
    return m_cmdMap.get(id);
}

CCommand* CGameMasterMgr::GetCommand(std::string& name)  // 0x084a8cb0
{
    return m_cmdMap.get(name);
}

} // namespace game_master
