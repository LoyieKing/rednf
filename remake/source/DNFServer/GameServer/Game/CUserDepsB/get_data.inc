bool stLevelUpRewardItemScript::get_data(const LevelUpRewardItemTableKey& key,
                                         LevelUpRewardItemData& out)
{
    // b28c-L20: AE — 槽位与 ORIG 全等（it@-0x10 find sret / end 临时@-0xc）；
    // ORIG 0x081b8a15 的 `operator==` 接收者是 end 临时（this=-0xc, other=-0x10）
    // ⇒ 源码书写序为 `end() == it`；另 ORIG 0x081b8a38 调 `operator*`(deEv) 非
    // `operator->`(ptEv) ⇒ 写 `(*it).second`（b26「槽序镜像不可翻」判词作废）。
    std::map<LevelUpRewardItemTableKey, LevelUpRewardItemData>::iterator it =
        m_map0.find(key);
    if (m_map0.end() == it)
        return false;
    out = (*it).second;                            // 两 int 顺序写（+0/+4）
    return true;
