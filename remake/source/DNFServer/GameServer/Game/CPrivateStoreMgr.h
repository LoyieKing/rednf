#ifndef DNF_GAME_PRIVATE_STORE_MGR_H_
#define DNF_GAME_PRIVATE_STORE_MGR_H_

#include <map>

class CUser;

namespace private_store
{
struct SIG_PRIVATE_STORE;
class CPrivateStore;
class IBuyer;
class ISeller;

// 私有商店管理器（ORIG 域 0x085ca3d6）。
// 布局依据 docs/class_func_reports/private_store__CPrivateStoreMgr/ 报告：
//   +0x04  CPrivateStore（管理器自带/回退 store）
//   +0x68  StaticPool<CPrivateStore,300>
//   +0x98  map<CUser*,CPrivateStore*>   —— 店主(卖家)表
//   +0xb0  map<CUser*,CPrivateStore*>   —— 买家表
//   +0xc8  map<PrivateStoreZone,list<CPrivateStore*>*>
// 当前头文件为渐进重建的最小可编译声明：stores 表已用真实 std::map 表达，
// StaticPool/zone 表等深层依赖尚未还原（见各函数报告标注）。
class CPrivateStoreMgr
{
public:
    CPrivateStoreMgr();
    ~CPrivateStoreMgr();
    int Enter(CUser*);
    int Exit(CUser*);
    void* FindStoreBuyer(CUser*);
    void* FindStoreSeller(CUser*);
    int FreeStoreBuyer(CUser*);
    int FreeStoreSeller(CUser*);
    int GetStoreBuyer(IBuyer**, CUser*, unsigned short);
    int GetStoreBuyer(IBuyer**, CUser*, CUser*);
    int GetStoreSeller(ISeller**, CUser*);
    int HandleError(CUser*, int, int, int);
    bool IsBusyPrivateStore(CUser*);
    int LoadPrivateStore(CUser*);
    int LoadPrivateStoreResponse(CUser*, SIG_PRIVATE_STORE*);
    int GetPrivateStoreSize();

private:
    std::map<CUser*, CPrivateStore*> m_storeSellers;  // ORIG +0x98
    std::map<CUser*, CPrivateStore*> m_storeBuyers;   // ORIG +0xb0
};

CPrivateStoreMgr* GetInstancePrivateStoreMgr();
}

#endif