#include "CPrivateStoreMgr.h"
#include "MoreTypes.h"

namespace private_store
{
namespace
{
CPrivateStoreMgr g_privateStoreMgr;
}

CPrivateStoreMgr::CPrivateStoreMgr() {}
CPrivateStoreMgr::~CPrivateStoreMgr() {}

CPrivateStoreMgr* GetInstancePrivateStoreMgr()
{
    return &g_privateStoreMgr;
}

int CPrivateStoreMgr::Enter(CUser*) { return 0; }
int CPrivateStoreMgr::Exit(CUser*) { return 0; }
void* CPrivateStoreMgr::FindStoreBuyer(CUser*) { return 0; }
void* CPrivateStoreMgr::FindStoreSeller(CUser*) { return 0; }
int CPrivateStoreMgr::FreeStoreBuyer(CUser*) { return 0; }
int CPrivateStoreMgr::FreeStoreSeller(CUser*) { return 0; }
int CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser*, unsigned short)
{
    if (out) *out = 0;
    return 0;
}
int CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser*, CUser*)
{
    if (out) *out = 0;
    return 0;
}
int CPrivateStoreMgr::GetStoreSeller(ISeller** out, CUser*)
{
    if (out) *out = 0;
    return 0;
}
int CPrivateStoreMgr::HandleError(CUser*, int, int, int) { return 0; }
bool CPrivateStoreMgr::IsBusyPrivateStore(CUser* user)
{
    return FindStoreSeller(user) != 0;
}
int CPrivateStoreMgr::LoadPrivateStore(CUser*) { return 0; }
int CPrivateStoreMgr::LoadPrivateStoreResponse(CUser*, SIG_PRIVATE_STORE*) { return 0; }
int CPrivateStoreMgr::GetPrivateStoreSize() { return 0; }
}
