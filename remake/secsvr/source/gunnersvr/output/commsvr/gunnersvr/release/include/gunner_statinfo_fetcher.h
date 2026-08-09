// Auto-generated header stub from DWARF info
// Original path: output/commsvr/gunnersvr/release/include/gunner_statinfo_fetcher.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_STATINFO_FETCHER_H_H_
#define SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_STATINFO_FETCHER_H_H_

#include "output/commlib/framework/release/include/comm_service_info.h"
#include <bits/stl_vector.h>
#include <time.h>

// sizeof = 24
struct GunnerStatInfo { // line 22
public:
SERVICES_ID svr_info_;
time_t copy_time_;
// anonymous struct, sizeof = 12
struct {
    public:
    void vector(const /*anon struct*/ int &arg0); // line 202
    void vector(unsigned int arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1, const /*anon struct*/ int &arg2); // line 215
    void vector(const /*anon struct*/ int &arg0); // line 232
    void ~vector(); // line 271
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 133
    void assign(unsigned int arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 298
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > begin(); // line 332
    __normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > begin() const; // line 341
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > end(); // line 350
    __normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > end() const; // line 359
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rbegin(); // line 368
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rbegin() const; // line 377
    reverse_iterator<__gnu_cxx::__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rend(); // line 386
    reverse_iterator<__gnu_cxx::__normal_iterator<const ZEN_STATUS_ITEM_WITHNAME*, std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > > rend() const; // line 395
    size_t size() const; // line 401
    size_t max_size() const; // line 406
    void resize(unsigned int arg0, ZEN_STATUS_ITEM_WITHNAME arg1); // line 421
    size_t capacity() const; // line 434
    bool empty() const; // line 443
    void reserve(unsigned int arg0); // line 69
    ZEN_STATUS_ITEM_WITHNAME & operator[](unsigned int arg0); // line 479
    const ZEN_STATUS_ITEM_WITHNAME & operator[](unsigned int arg0) const; // line 494
    protected:
    void _M_range_check(unsigned int arg0) const; // line 500
    public:
    ZEN_STATUS_ITEM_WITHNAME & at(unsigned int arg0); // line 519
    const ZEN_STATUS_ITEM_WITHNAME & at(unsigned int arg0) const; // line 537
    ZEN_STATUS_ITEM_WITHNAME & front(); // line 548
    const ZEN_STATUS_ITEM_WITHNAME & front() const; // line 556
    ZEN_STATUS_ITEM_WITHNAME & back(); // line 564
    const ZEN_STATUS_ITEM_WITHNAME & back() const; // line 572
    ZEN_STATUS_ITEM_WITHNAME * data(); // line 583
    const ZEN_STATUS_ITEM_WITHNAME * data() const; // line 587
    void push_back(const ZEN_STATUS_ITEM_WITHNAME &arg0); // line 602
    void pop_back(); // line 623
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 93
    void insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, unsigned int arg1, const ZEN_STATUS_ITEM_WITHNAME &arg2); // line 657
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > erase(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0); // line 110
    __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > erase(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, __normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg1); // line 122
    void swap(/*anon struct*/ int &arg0); // line 733
    void clear(); // line 748
    protected:
    void _M_fill_assign(unsigned int arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 171
    void _M_fill_insert(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, unsigned int arg1, const ZEN_STATUS_ITEM_WITHNAME &arg2); // line 311
    void _M_insert_aux(__normal_iterator<ZEN_STATUS_ITEM_WITHNAME*,std::vector<ZEN_STATUS_ITEM_WITHNAME, std::allocator<ZEN_STATUS_ITEM_WITHNAME> > > arg0, const ZEN_STATUS_ITEM_WITHNAME &arg1); // line 249
} statinfo_array_;
~GunnerStatInfo();
GunnerStatInfo();
};
// sizeof = 24
struct GunnerStatinfoFetcher { // line 33
private:
GunnerStatInfo stat_info_;
public:
int get_info(const char *arg0, GunnerStatInfo &arg1); // line 36
int dump(const char *arg0); // line 39
unsigned int dump_timeinfo(const char *arg0); // line 42
private:
int fetch_svrinfo_from_filename(const char *arg0, GunnerStatInfo &arg1); // line 44
public:
~GunnerStatinfoFetcher();
GunnerStatinfoFetcher();
};

#endif // SECSVR_OUTPUT_COMMSVR_GUNNERSVR_RELEASE_INCLUDE_GUNNER_STATINFO_FETCHER_H_H_
