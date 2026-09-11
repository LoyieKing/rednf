void ScriptManager::Init()
{
    // ORIG 0x81a13f6（59 指令）：按序 RegistScript(Singleton<X>::Get(), 0x13) 共 9 个：
    //   AvatarFixedHiddenOptionServer → AvatarRechargeServer → EmblemCompoundServer →
    //   AvatarConvertServer → CerashopAddRestrict::Manager → LevelupSupportEventManger →
    //   Arad_DataManager → CreateDnfEventSciprtServer → LevelupSupport2ndEventManger。
    // 当前依赖闭包完整的六个（保持 ORIG 相对次序；#3/#4 由 FixAradEmblemConvert 落地）：
    RegistScript(Singleton<AvatarFixedHiddenOptionServer>::Get(), 0x13);
    RegistScript(Singleton<AvatarRechargeServer>::Get(), 0x13);
    RegistScript(Singleton<EmblemCompoundServer>::Get(), 0x13);   // #3（ORIG 次序）
    RegistScript(Singleton<AvatarConvertServer>::Get(), 0x13);    // #4（ORIG 次序）
    RegistScript(Singleton<CerashopAddRestrict::Manager>::Get(), 0x13);
    RegistScript(Singleton<LevelupSupportEventManger>::Get(), 0x13);    // #6（ORIG 次序）
    RegistScript(Singleton<Arad_DataManager>::Get(), 0x13);
    RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13);   // #8（ORIG 次序）
    RegistScript(Singleton<LevelupSupport2ndEventManger>::Get(), 0x13); // #9（ORIG 次序）
}