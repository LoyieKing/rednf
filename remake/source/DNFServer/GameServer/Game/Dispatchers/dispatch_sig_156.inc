int Dispatcher_GoblinPadRequestCryptKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    user->getPad()->GenerateRandomData();                     // P48/P183：每次 use 重调 getPad
    PacketGuard response;
    response.put_header(1, 0xe2);
    response.put_byte(1);
    response.put_short(static_cast<unsigned short>(user->getPad()->GetKeyIdx(0)));
    response.put_short(static_cast<unsigned short>(user->getPad()->GetKeyIdx(1)));
    response.put_byte(static_cast<unsigned char>(user->getPad()->GetShakeData(0)));
    response.put_byte(static_cast<unsigned char>(user->getPad()->GetShakeData(1)));
    response.put_byte(static_cast<unsigned char>(user->getPad()->GetShakeData(2)));
    response.put_byte(static_cast<unsigned char>(user->getPad()->GetShakeData(3)));
    response.finalize(true);
    user->Send(response);
    return 0;
