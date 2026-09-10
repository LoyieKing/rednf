// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSyncSlangFilter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CSyncSlangFilter.h"

#include <new>
#include <string.h>
#include <ext/hash_map>

// [T2b-GI · P338/P327] 补 <iostream> 引入 file-scope 动态初始化 __ioinit，恢复本 TU
// `_GLOBAL__I_` 发射；键名目标 `_ZN16CSyncSlangFilter8instanceE`（ORIG 0x862e8df）。
// 零语义改动。
#include <iostream>

#include "DBMgr.h"
#include "GameTypes.h"
#include "GlobalData.h"
#include "MySQL.h"

namespace OS_API
{
#include "CSyncSlangFilter/snprintf.inc"
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

// [T2b-GI · P327] 定义前移：ORIG 该 TU 首个强外部实体 = 类静态成员 instance
// （GI 键名 `_GLOBAL__I__ZN16CSyncSlangFilter8instanceE`，ORIG 0x862e8df）。
// 零初始化对象（非动态初始化）⇒ 移动不改变构造序。
CSyncSlangFilter* CSyncSlangFilter::instance = 0;

#include "CSyncSlangFilter/SlangNode.inc"
{
    end_ = 0;
}

// [b28d-solo-CSyncSlangFilter] AE：P50 先声明后赋值（ORIG 有独立 iterator::C1Ev，初始化式会被 4.4 折叠）+ P141 无条件 delete 复现 ORIG 空指针守卫；ORIG 0x0862e92a。
#include "CSyncSlangFilter/dtor_SlangNode.inc"
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it;
    it = children_.begin();
    for (; it != children_.end(); ++it)
    {
        delete it->second;
    }
    children_.clear();
}

#include "CSyncSlangFilter/FindChild.inc"
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::const_iterator it = children_.find(ch);
    if (it != children_.end())
        return it->second;
    return 0;
}

// [b28d-solo-CSyncSlangFilter] AE：键类型改 pair<const unsigned int, SlangNode*>（消 pair<uint,..> 转换构造）+ new 内联进 insert 实参（消具名 node 槽，P226/P278 匿名临时，槽集 it/end/insert/pair/new 与 ORIG 逐槽全等）；ORIG 0x0862ea5a。
#include "CSyncSlangFilter/AddChild.inc"
{
    __gnu_cxx::hash_map<unsigned int, SlangNode*>::iterator it = children_.find(ch);
    if (it == children_.end())
    {
        it = children_.insert(std::pair<const unsigned int, SlangNode*>(ch, new SlangNode())).first;
    }
    return it->second;
}

#include "CSyncSlangFilter/IsEndNode.inc"
{
    return end_;
}

#include "CSyncSlangFilter/SetEndNode.inc"
{
    end_ = v;
}

#include "CSyncSlangFilter/IMPL.inc"
{
    root = new SlangNode();
}

#include "CSyncSlangFilter/dtor_IMPL.inc"
{
    if (root)
    {
        delete root;
        root = 0;
    }
}

#include "CSyncSlangFilter/CSyncSlangFilter.inc"
    : impl_(new IMPL())
    , implName_(new IMPL())
{
}

#include "CSyncSlangFilter/dtor_CSyncSlangFilter.inc"
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

#include "CSyncSlangFilter/GetInstance.inc"
{
    if (instance == 0)
        instance = new CSyncSlangFilter;
    return instance;
}

#include "CSyncSlangFilter/FreeInstance.inc"
{
    if (instance)
    {
        delete instance;
        instance = 0;
    }
}

#include "CSyncSlangFilter/match.inc"
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

#include "CSyncSlangFilter/MatchName.inc"
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

#include "CSyncSlangFilter/HasSlang.inc"
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

#include "CSyncSlangFilter/HasSlangName.inc"
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
#include "CSyncSlangFilter/Filter.inc"
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
#include "CSyncSlangFilter/AddSlang.inc"
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
#include "CSyncSlangFilter/AddSlangName.inc"
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

#include "CSyncSlangFilter/AddSlangList.inc"
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

#include "CSyncSlangFilter/AddSlangListName.inc"
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
