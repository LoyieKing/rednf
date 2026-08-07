// GameDataPool.cpp
#include "GameDataPool.h"
#include "Character.h"
#include "TActiveConnect.h"
#include "InternalMsg.h"
#include "HandlerFor_TE_.h"

using namespace nsl;

static unsigned int uniqueCharacKey = 0;

GameDataPool::GameDataPool()
{
    CharacterPool = new object_pool_by_boost_pool<auction::Character, int, int, int>(0x800);
    ConInterfacePool = new object_pool_by_boost_pool<ConInterface, int, int, int>(0x20);
    TimeEntityPool = new object_pool_by_boost_pool<TE_Entity<HandlerFor_TE_>, int, int, int>(0x20);
    InternalMsgPool = new object_pool_by_boost_pool<InternalMsg, int, int, int>(0x20);
    pool_check_character = 0;
    pool_check_conInter = 0;
    pool_check_timeEntity = 0;
    pool_check_timeInternal = 0;
    pool_check_interPool = 0;
    pool_check_cmdInter = 0;
}

GameDataPool::~GameDataPool()
{
}

auction::Character* GameDataPool::createCharacter()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(CharacPoolLock);
    auction::Character* r = CharacterPool->construct();
    if (r == NULL)
    {
        return NULL;
    }
    unsigned int characKey = uniqueCharacKey;
    uniqueCharacKey = uniqueCharacKey + 1;
    r->setCharacKey(characKey);
    pool_check_character = pool_check_character + 1;
    return r;
}

void GameDataPool::destroyCharacter(auction::Character* pCharactor)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(CharacPoolLock);
    CharacterPool->free(pCharactor);
    pool_check_character = pool_check_character - 1;
}

ConInterface* GameDataPool::createConInfo(int direction, const char* remote_ip_, int remote_port_,
                                          TCPUser::ENUM_DATA_TYPE send_data_type_,
                                          TCPUser::ENUM_DATA_TYPE recv_data_type_, bool need_to_recon_)
{
    ConInterface* r = ConInterfacePool->construct();
    r->setInfo(direction, remote_ip_, remote_port_, send_data_type_, recv_data_type_, need_to_recon_);
    pool_check_conInter = pool_check_conInter + 1;
    return r;
}

void GameDataPool::destroyConInfo(ConInterface* pConInterface)
{
    ConInterfacePool->free(pConInterface);
    pool_check_conInter = pool_check_conInter - 1;
}

TE_Entity<HandlerFor_TE_>* GameDataPool::createTimeEntity()
{
    TE_Entity<HandlerFor_TE_>* r = TimeEntityPool->construct();
    InternalMsg* pArg = InternalMsgPool->construct();
    r->setArg(pArg);
    pool_check_timeEntity = pool_check_timeEntity + 1;
    pool_check_timeInternal = pool_check_timeInternal + 1;
    return r;
}

void GameDataPool::destroyTimeEntity(void* pTimeEntity)
{
    InternalMsg* p = ((ITimeEntity*)pTimeEntity)->getArg();
    InternalMsgPool->free(p);
    TimeEntityPool->free((TE_Entity<HandlerFor_TE_>*)pTimeEntity);
    pool_check_timeEntity = pool_check_timeEntity - 1;
    pool_check_timeInternal = pool_check_timeInternal - 1;
}
