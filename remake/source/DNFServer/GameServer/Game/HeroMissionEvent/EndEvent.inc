void HeroMissionEvent::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20d, 0);
    tr("[HeroMission] End!");
    // ORIG 0x816674a：SetEventFlag(false)（写 +0x08 标志，与 IsEventing 同字段）
    SetEventFlag(false);
