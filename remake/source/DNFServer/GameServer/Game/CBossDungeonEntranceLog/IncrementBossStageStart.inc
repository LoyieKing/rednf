// ORIG 0x081564f0：命中 → ++stageStartCount(+0)；未命中 → memset0 后 insert
// {stageStartCount=1, 其余 0}。
void CBossDungeonEntranceLog::IncrementBossStageStart(CUser& user,
