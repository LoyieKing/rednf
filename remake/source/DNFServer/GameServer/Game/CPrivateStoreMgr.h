#ifndef DNF_GAME_PRIVATE_STORE_MGR_H_
#define DNF_GAME_PRIVATE_STORE_MGR_H_

class CUser;

namespace private_store
{
struct SIG_PRIVATE_STORE;
class IBuyer;
class ISeller;

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
};

CPrivateStoreMgr* GetInstancePrivateStoreMgr();
}

#endif
