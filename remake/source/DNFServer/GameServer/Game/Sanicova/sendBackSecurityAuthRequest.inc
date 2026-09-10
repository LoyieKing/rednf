void CUser::sendBackSecurityAuthRequest()
{
    // b28d-37 [N78/P60 封顶，ORIG 0x86?]：ORIG 内联 rep stos 清零 vs 4.4 循环/调用形（P116/N78 族）；236/230。
    short question[3];      // -0x94/-0x92/-0x90（问题四元组前三个）
    short answers[29];      // -0x8e..（第四元组起，29×short 缓冲）
    for (int i = 0; i < 3; ++i)
        question[i] = 0;
    for (int i = 0; i < 29; ++i)
        answers[i] = 0;
    question[0] = -1;

    int encryptedLen = 0x800;                       // -0x24
    unsigned char encrypted[2048];                  // -0x894
    for (int i = 0; i < 0x800; ++i)
        encrypted[i] = 0;

    WongWork::CSecurityCard* card = getSecurityCard();
    card->generateQuestion(question[0], question[1], question[2],
                           *(short*)&answers[0]);

    char algo = 1;                                  // -0x1e
    do
    {
        algo = (char)get_rand_int(0xe);
    } while (algo == 0 || algo == 5);

    Sanicova::CPad* pad = getPad();
    pad->GenerateRandomData();
    unsigned int cipherIdx = pad->GetSelectedCipherIdx() & 0xffff;

    int cipherKey = (int)algo;
    Cipher* cipher =
        GlobalData::s_secu_cipher_keystring_manager_[cipherIdx].cipher;
    cipher->Encrypt(cipherKey, (unsigned char*)question, 8, encrypted,
                    &encryptedLen);
    char certFlag = card->getCertFlag();            // -0x1d

    PacketGuard packet;
    packet.put_header(0, 0x98);
    packet.put_byte((int)algo);
    pad = getPad();
    packet.put_short((int)(pad->GetKeyIdx(0) & 0xffff));
    pad = getPad();
    packet.put_short((int)(pad->GetKeyIdx(1) & 0xffff));
    for (int i = 0; i < 4; ++i)
    {
        pad = getPad();
        packet.put_byte((int)(pad->GetShakeData(i) & 0xff));
    }
    packet.put_int(encryptedLen);
    packet.put_binary((char*)encrypted, encryptedLen);
    packet.put_byte((int)certFlag);
    packet.finalize(true);
    Send(packet);

    card = getSecurityCard();
    if (card->isActivate())
    {
        card->setRequestState(WongWork::CSecurityCard::eRequestStateAuth);
    }
    else
    {
        pad = getPad();
        if (pad->isActivate())
            pad->setRequestState(Sanicova::CPad::eRequestStateAuth);
    }
