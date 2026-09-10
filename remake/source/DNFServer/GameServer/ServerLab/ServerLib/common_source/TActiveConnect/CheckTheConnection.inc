void ActiveConManager::CheckTheConnection(ConInterface* conInfo)
{
    // ORIG: while (getTCPUser()->isDisconnected() || getTCPUser()->isAboutToDisconnect())
    // dual getTCPUser, condition-at-top (jmp to check), no local bool
    while (conInfo->getTCPUser()->isDisconnected() ||
           conInfo->getTCPUser()->isAboutToDisconnect())
    {
        conInfo->setTCPUser(RequestConnect(conInfo));
    }
