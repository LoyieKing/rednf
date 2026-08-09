// auction::Character.cpp
#include <stdio.h>

#include "Character.h"
#include "Zone.h"
#include "InterHandler.h"
#include "TCPUser.h"
#include "TActiveConnect.h"
#include "ServiceFactory.h"
#include "WorkThread.h"
#include "CommonDataPool.h"
#include "ActiveNetClose.h"
#include "Message.h"
#include "InternalMsgDesign.h"
#include "GlobalInstance.h"

namespace auction {

using namespace nsl;

void Character::setCharacKey(unsigned int characKey)
{
    mCharacKey = characKey;
}

unsigned int Character::getCharacKey()
{
    return mCharacKey;
}

Character::Character()
{
    mZone = G_Zone();
    bActiveUser = false;
}

Character::~Character()
{
}

bool Character::setArea(int areaIndex)
{
    bActiveUser = false;
    mAreaIndex = areaIndex;
    return G_Zone()->mArea[areaIndex]->regist(mCharacKey, this);
}

void Character::setActiveTCPUser(nsl::ConInterface* info)
{
    conInfo = info;
    nsl::TCPUser* u = conInfo->getTCPUser();
    nsl::ISession::setTCPUser(u);
    u->setSession(this);
    bActiveUser = true;
}

void Character::setPassiveTCPUser(nsl::TCPUser* u)
{
    u->setSession(this);
    nsl::ISession::setTCPUser(u);
    bActiveUser = false;
}

void Character::setInterHandler(InterHandler* pHandler)
{
    TInterHandler = pHandler;
}

void Character::onClose(bool bActiveClosing)
{
    printf("Close Type : %d\n", (unsigned int)bActiveClosing);
    INTERNALMSG_DESTROY_CHARACTER interMsg;
    if (bActiveUser != false)
    {
        interMsg.bActiveJob = true;
        interMsg.pConInfo = conInfo;
    }
    interMsg.workIndex = 0;
    interMsg.bActiveClosing = bActiveClosing;
    interMsg.characKey = mCharacKey;
    interMsg.areaIndex = mAreaIndex;
    nsl::Message* pNextMsg = pApp->super_DataPools.getCommonDataPool(nsl::tlsThreadId)->createMessage(1);
    nsl::CMsgCell* pNextCell = pNextMsg->getCellFromMessage();
    *pNextCell << &interMsg;
    pApp->super_Threads.getWorkThread(pNextMsg->getCellFromMessage()->GetInternalMsg()->workIndex)
        ->PushTransaction(pNextMsg);
}

void Character::onDoClose()
{
    nsl::TCPUser* u = nsl::ISession::getTCPUser();
    u->setActiveSyncByWorker(true);
    nsl::G_ActiveNetClose()->pushActiveClose(u);
}

void Character::setState(IState::STATE_COMMON in)
{
    if (currentState != NULL)
    {
        currentState->exit();
        operator delete(currentState);
    }
    switch (in)
    {
    case IState::STATE_MOVE:
        currentState = new Stand;
        break;
    default:
        currentState = new Move;
        break;
    case IState::STATE_ATTACK:
        currentState = new Attack;
        break;
    case IState::STATE_CAST:
        currentState = new Cast;
        break;
    }
    currentState->enter();
}

void Character::onMove()
{
    setState(IState::STATE_MOVE);
}

void Character::onStand()
{
    setState(IState::STATE_STAND);
}

void Character::onAttack()
{
    setState(IState::STATE_ATTACK);
}

void Character::onCast()
{
    setState(IState::STATE_CAST);
}

} // namespace auction
