#include "CSyncSlangFilter.h"

#include <new>
#include <string.h>
#include <ext/hash_map>

#include "DBMgr.h"
#include "GameTypes.h"
#include "GlobalData.h"
#include "MySQL.h"

namespace OS_API
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
}

class SlangNode
{
public:
    SlangNode();
    ~SlangNode();
    SlangNode* AddChild(unsigned int ch);
    SlangNode* FindChild(unsigned int ch) const;
    bool IsEndNode() const;
    void SetEndNode(bool v);

private:
    __gnu_cxx::hash_map<unsigned int, SlangNode*> children_;
    bool end_;
};

SlangNode::SlangNode()
{
    end_ = 0;
}

// [b28d-solo-CSyncSlangFilter] AE：P50 先声明后赋值（ORIG 有独立 iterator::C1Ev，初始化式会被 4.4 折叠）+ P141 无条件 delete 复现 ORIG 空指针守卫；ORIG 0x0862e92a。
SlangNode::~SlangNode()
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it;
    it = children_.begin();
    for (; it != children_.end(); ++it)
    {
        delete it->second;
    }
    children_.clear();
}

SlangNode* SlangNode::FindChild(unsigned int ch) const
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::const_iterator it = children_.find(ch);
    if (it != children_.end())
        return it->second;
    return 0;
}

// [b28d-solo-CSyncSlangFilter] AE：键类型改 pair<const unsigned int, SlangNode*>（消 pair<uint,..> 转换构造）+ new 内联进 insert 实参（消具名 node 槽，P226/P278 匿名临时，槽集 it/end/insert/pair/new 与 ORIG 逐槽全等）；ORIG 0x0862ea5a。
SlangNode* SlangNode::AddChild(unsigned int ch)
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it = children_.find(ch);
    if (it == children_.end())
    {
        it = children_.insert(std::pair<const unsigned int, SlangNode*>(ch, new SlangNode())).first;
    }
    return it->second;
}

bool SlangNode::IsEndNode() const
{
    return end_;
}

void SlangNode::SetEndNode(bool v)
{
    end_ = v;
}

CSyncSlangFilter::IMPL::IMPL()
{
    root = new SlangNode();
}

CSyncSlangFilter::IMPL::~IMPL()
{
    if (root)
    {
        delete root;
        root = 0;
    }
}

CSyncSlangFilter* CSyncSlangFilter::instance = 0;

CSyncSlangFilter::CSyncSlangFilter()
    : impl_(new IMPL())
    , implName_(new IMPL())
{
}

CSyncSlangFilter::~CSyncSlangFilter()
{
    if (impl_)
    {
        delete impl_;
        impl_ = 0;
    }
    if (implName_)
    {
        delete implName_;
        implName_ = 0;
    }
}

CSyncSlangFilter* CSyncSlangFilter::GetInstance()
{
    if (instance == 0)
        instance = new CSyncSlangFilter;
    return instance;
}

void CSyncSlangFilter::FreeInstance()
{
    if (instance)
    {
        delete instance;
        instance = 0;
    }
}

int CSyncSlangFilter::match(const std::string& name) const
{
    if (name.empty())
        return 0;
    SlangNode* node = impl_->root;
    unsigned i = 0;
    for (i = 0; i < name.size(); ++i)
    {
        unsigned ch = (unsigned char)name[i];
        node = node->FindChild(ch);
        if (node == 0)
            return 0;
        if (node->IsEndNode())
            return (int)(i + 1);
    }
    return 0;
}

int CSyncSlangFilter::MatchName(const std::string& name) const
{
    if (name.empty())
        return 0;
    SlangNode* node = implName_->root;
    unsigned i = 0;
    for (i = 0; i < name.size(); ++i)
    {
        unsigned ch = (unsigned char)name[i];
        node = node->FindChild(ch);
        if (node == 0)
            return 0;
        if (node->IsEndNode())
            return (int)(i + 1);
    }
    return 0;
}

bool CSyncSlangFilter::HasSlang(const std::string& name)
{
    unsigned i;
    i = 0;
    for (i = 0; i < name.size(); ++i)
    {
        readGuard<rwLock> guard(&lock_);
        if (match(name.substr(i, name.size() - i)) != 0)
            return 1;
    }
    return 0;
}

bool CSyncSlangFilter::HasSlangName(const std::string& name)
{
    unsigned i;
    i = 0;
    for (i = 0; i < name.size(); ++i)
    {
        readGuard<rwLock> guard(&lock_);
        if (MatchName(name.substr(i, name.size() - i)) != 0)
            return 1;
    }
    return 0;
}

// [b28d-solo-CSyncSlangFilter] AE：sub 去具名块改全表达式实参 + stars/allocator 匿名临时（P221/P226；同时把两析构置于 i+=n 之前，复现 ORIG 块序与槽位）；ORIG 0x0862e46c。
std::string CSyncSlangFilter::Filter(const std::string& str)
{
    std::string out(str);
    unsigned i;
    unsigned n;
    i = 0;
    for (; i < str.size(); )
    {
        readGuard<rwLock> guard(&lock_);
        n = (unsigned)match(str.substr(i, str.size() - i));
        if (n != 0)
        {
            out.replace(i, n, std::string(n, '*', std::allocator<char>()));
            i += n;
        }
        else
            ++i;
    }
    return out;
}

// [b28d-solo-CSyncSlangFilter] N：P75/N-G —— snprintf 首参 buf 装载时刻跨版本差（4.1 在 c_str 后立即 lea→%edx 并跨 varargs 保活，4.4 末位 lea→%eax）；(char*)buf 与逗号形实测同（&buf[0] 历史结论亦同）；ORIG 0x0862e72a。
void CSyncSlangFilter::AddSlang(const std::string& str)
{
    char buf[0x101] = {0};
    OS_API::snprintf(buf, 0x100, "%s", str.c_str());
    buf[0x100] = 0;
    SlangNode* node = impl_->root;
    if ((unsigned char)buf[0] == 0xbb &&
        (unsigned char)buf[1] == 0xd8 &&
        (unsigned char)buf[2] == 0xc3)
    {
        unsigned skip0 = 0;
        unsigned skip1 = 0;
        (void)skip0;
        (void)skip1;
    }
    unsigned i = 0;
    for (i = 0; i < str.size(); ++i)
    {
        writeGuard<rwLock> guard(&lock_);
        node = node->AddChild((unsigned char)buf[i]);
    }
    node->SetEndNode(true);
}

// [b28d-solo-CSyncSlangFilter] N：P75/N-G —— 同 AddSlang（snprintf 首参装载时刻跨版本差）；ORIG 0x0862e276。
void CSyncSlangFilter::AddSlangName(const std::string& str)
{
    char buf[0x101] = {0};
    OS_API::snprintf(buf, 0x100, "%s", str.c_str());
    buf[0x100] = 0;
    SlangNode* node = implName_->root;
    unsigned i = 0;
    for (i = 0; i < str.size(); ++i)
    {
        writeGuard<rwLock> guard(&lock_);
        node = node->AddChild((unsigned char)buf[i]);
    }
    node->SetEndNode(true);
}

bool CSyncSlangFilter::AddSlangList()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slang from slang_list");
    if (db->exec(true) != 1)
        return 0;
    unsigned i = 0;
    for (; i < (unsigned)db->get_n_rows(); ++i)
    {
        if (db->fetch() != 1)
            return 0;
        char buf[0x33] = {0};
        if (db->get_str(0, buf, 0x33) != 1)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0xbc, 5);
            t("CSyncSlangFilter::AddSlangList");
            return 0;
        }
        {
            std::string tmp(buf, std::allocator<char>());
            AddSlang(tmp);
        }
    }
    return 1;
}

bool CSyncSlangFilter::AddSlangListName()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slang from slang_list_name");
    if (db->exec(true) != 1)
        return 0;
    unsigned i = 0;
    for (; i < (unsigned)db->get_n_rows(); ++i)
    {
        if (db->fetch() != 1)
            return 0;
        char buf[0x33] = {0};
        if (db->get_str(0, buf, 0x33) != 1)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0xd9, 5);
            t("CSyncSlangFilter::AddSlangList");
            return 0;
        }
        {
            std::string tmp(buf, std::allocator<char>());
            AddSlangName(tmp);
        }
    }
    return 1;
}
