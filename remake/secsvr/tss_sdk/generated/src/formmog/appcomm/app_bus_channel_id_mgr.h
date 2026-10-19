// Auto-generated header stub from DWARF info
// Original path: src/formmog/appcomm/app_bus_channel_id_mgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_APPCOMM_APP_BUS_CHANNEL_ID_MGR_H_H_
#define SECSVR_SRC_FORMMOG_APPCOMM_APP_BUS_CHANNEL_ID_MGR_H_H_

#include <stdint.h>
#include <sys/types.h>

// sizeof = 24
struct BusChannelIdMgr { // line 15
private:
uint32_t used_svr_id_;
bool is_init_;
unsigned int max_channel_id_;
uint32_t *channel_info_;
ZEN_File_Lock *lock_;
bool is_use_pid_;
public:
BusChannelIdMgr(); // line 17
~BusChannelIdMgr(); // line 18
int32_t init(unsigned int arg0); // line 27
int get_bus_channel_id(); // line 34
int get_bus_channel_id(unsigned int arg0); // line 35
private:
int init_bus_channel_info_list(); // line 38
int read_channel_info_from_file_without_lock(FILE *arg0); // line 41
int write_channel_info_to_file_without_lock(FILE *arg0); // line 42
int count_channel_info_size() const; // line 45
int find_bus_channel_id_from_mem(unsigned int arg0); // line 48
int alloc_bus_channel_id_from_mem(unsigned int arg0); // line 51
int alloc_bus_channel_id_according_pid(unsigned int arg0); // line 53
void free_svr_id(); // line 55
bool is_valid_channel_id(int arg0); // line 57
BusChannelIdMgr(const BusChannelIdMgr &arg0); // line 78
BusChannelIdMgr & operator=(const BusChannelIdMgr &arg0); // line 79
};

#endif // SECSVR_SRC_FORMMOG_APPCOMM_APP_BUS_CHANNEL_ID_MGR_H_H_
