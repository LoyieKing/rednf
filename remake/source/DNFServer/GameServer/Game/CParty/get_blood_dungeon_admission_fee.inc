// ORIG 0x859f75e：进入血腥地下城入场费 = lottoCost + price + (inc * price) / 100。
//   price = ServerParameterScript::GetPriceAverage(charac_level)
//   inc   = ServerParameterScript::GetAdmissionIncrease(CCharacBloodDungeon::GetEnterCount)
//   lotto = ServerParameterScript::GetLottoCost()
int CParty::get_blood_dungeon_admission_fee(CUser* user)
{
    // ORIG 0859f77b `call 085bfe88`（_ZNK18CCharacBloodDungeon13GetEnterCountEv，
    // 读 +0x18）——原内联裸偏移改真实调用。
    int enterCount =
        reinterpret_cast<CCharacBloodDungeon*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1))
            ->GetEnterCount();
    // P0：ORIG 仅 enterCount/-0x14、price/-0x10、half/-0xc 三具名槽；
    // level(get_charac_level→%ebx)、inc(GetAdmissionIncrease→%ecx)、
    // lotto(GetLottoCost→%eax) 均寄存器直传内联，无栈往返。三处独立
    // G_CDataManager()（0859f78e/0859f7a7/0859f7de），re-fetch &m_serverParameter。
    int price = G_CDataManager()->m_serverParameter.GetPriceAverage(
        user->get_charac_level());
    int half = (G_CDataManager()->m_serverParameter.GetAdmissionIncrease(enterCount) *
                price) /
               100;
    return G_CDataManager()->m_serverParameter.GetLottoCost() + price + half;
// b28d-solo-CParty: AE —— P0 内联 m_pUser/CharacInfoFields（消两缓存槽）+ P196 (signed char) 有符号判定（jg）；143=143
}