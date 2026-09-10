void ItemVendingMachine::ProcessIPGUserData(CUser* user,
                                            WongWork::IPG::SIPGData* reqData,
                                            WongWork::IPG::SIPGData* resData,
                                            Stream* stream)
{
    ProcessIPGUserDataJpnEx(user, reqData, resData, stream);
