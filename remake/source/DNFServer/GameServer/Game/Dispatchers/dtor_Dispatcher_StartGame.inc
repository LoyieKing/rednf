// ==================== Dispatcher_StartGame（ORIG 0x813f7b0 区）====================
// check_error @ 0x813f7de：isOpenAdvanceAltar（CDailyScheduleManager 4/10）→
//   state==3 → level>=50 → 非竞技佣兵 → 非摆摊 → 非交易 → 非分解商店 →
//   非组队 → 非 PVP 频道 → 0；各失败 1/2/3/4。
// process @ 0x813f8ce：check_error → param+0x04；成功后
//   CharacAdvanceAltarManager::checkAndCreateDefaultRidable（未建模，推断）。
// read @ 0x813f7b0：读 0xd 字节。
// send @ 0x813f92a：error==0 先 StageControl::gotoStage（推断）；发包 0x23c：
//   put_byte(1)+put_binary(param,8)；成功后 set_state(0xd)+put_int(1)+ridable 数据
//   （getAdvanceAltarDataByIndex/putRidableInfoToPacket 未建模，推断）。
Dispatcher_StartGame::~Dispatcher_StartGame() {}
