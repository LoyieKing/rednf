#ifndef AUCTION_GAMEDATAPOOL_H_
#define AUCTION_GAMEDATAPOOL_H_

#include "CommonDataPool.h"
#include "ObjectPoolByBoostPool.h"
#include "TE_Entity.h"
#include "TCPUser.h"

namespace auction {
class Character;
}

class HandlerFor_TE_;

namespace nsl {

class ConInterface;
class InternalMsg;
}

class GameDataPool : public nsl::CommonDataPool
{
public:
    GameDataPool();
    virtual ~GameDataPool();
    virtual void destroyTimeEntity(void* pTimeEntity);
    auction::Character* createCharacter();
    void destroyCharacter(auction::Character* pCharactor);
    nsl::ConInterface* createConInfo(int direction, const char* remote_ip_, int remote_port_,
                                     nsl::TCPUser::ENUM_DATA_TYPE send_data_type_,
                                     nsl::TCPUser::ENUM_DATA_TYPE recv_data_type_, bool need_to_recon_);
    void destroyConInfo(nsl::ConInterface* pConInterface);
    nsl::TE_Entity<HandlerFor_TE_>* createTimeEntity();

    nsl::object_pool_by_boost_pool<auction::Character, int, int, int>* CharacterPool;
    nsl::ThreadLock CharacPoolLock;
    nsl::object_pool_by_boost_pool<nsl::ConInterface, int, int, int>* ConInterfacePool;
    nsl::object_pool_by_boost_pool<nsl::TE_Entity<HandlerFor_TE_>, int, int, int>* TimeEntityPool;
    nsl::object_pool_by_boost_pool<nsl::InternalMsg, int, int, int>* InternalMsgPool;
    int pool_check_character;
    int pool_check_conInter;
    int pool_check_timeEntity;
    int pool_check_timeInternal;
    int pool_check_interPool;
    int pool_check_cmdInter;
};

#endif // AUCTION_GAMEDATAPOOL_H_
