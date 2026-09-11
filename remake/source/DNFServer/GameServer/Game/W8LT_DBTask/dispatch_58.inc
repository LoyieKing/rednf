// ORIG 0x08445aa6：GetDBHandle(1,0) → GetOutBuffer<SIG_HADES_PUNISH>。
// end_time 串：days==0 时 memcpy "'9999-12-31 23:59:59'"（0x16 含引号与 nul），
// 否则 sprintf "adddate(now(),interval %d day)"。apply_flag = flag==0 ? 1 : 2；
// punish_type==1 → punish_value=3，否则 0xc。update member_punish_info；
// exec 失败或无影响行则 insert（失败返 0）。punish_type==1 选长韩文 reason
// （0xa6，ORIG DAT_08c57930）否则短串（0x3d，DAT_08c579d8）。CSystemTime
// getCurSec → localtime_r → strftime "%Y" 年表名。insert
// member_punish_info_history_%Y（失败返 0）。punish_value==3 或
// (==0xc && apply_flag==1) 时再 insert member_punish_info_ars_mail（失败返 0）。
// 全成返 1。
bool DB_HadesPunish::dispatch(int userIdx, int slot, Stream* stream)
