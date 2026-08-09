// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_data_with_version.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_DATA_WITH_VERSION_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_DATA_WITH_VERSION_H_H_

#include <bits/stl_list.h>
#include <stddef.h>

// sizeof = 12
struct ZenDataWithVersion<unsigned int,SecagentGameExe,std::equal_to<unsigned int> > { // line 26
private:
// anonymous struct, sizeof = 8
struct {
    protected:
    _List_node<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> * _M_create_node(const CNode &arg0); // line 446
    public:
    void list(const /*anon struct*/ int &arg0); // line 468
    void list(unsigned int arg0, const CNode &arg1, const /*anon struct*/ int &arg2); // line 480
    void list(const /*anon struct*/ int &arg0); // line 491
    /*anon struct*/ int & operator=(const /*anon struct*/ int &arg0); // line 120
    void assign(unsigned int arg0, const CNode &arg1); // line 544
    /*anon struct*/ int get_allocator() const; // line 570
    _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> begin(); // line 579
    _List_const_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> begin() const; // line 588
    _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> end(); // line 597
    _List_const_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> end() const; // line 606
    reverse_iterator<std::_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> > rbegin(); // line 615
    reverse_iterator<std::_List_const_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> > rbegin() const; // line 624
    reverse_iterator<std::_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> > rend(); // line 633
    reverse_iterator<std::_List_const_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> > rend() const; // line 642
    bool empty() const; // line 651
    size_t size() const; // line 656
    size_t max_size() const; // line 661
    void resize(unsigned int arg0, CNode arg1); // line 105
    CNode & front(); // line 683
    const CNode & front() const; // line 691
    CNode & back(); // line 699
    const CNode & back() const; // line 711
    void push_front(const CNode &arg0); // line 730
    void pop_front(); // line 746
    void push_back(const CNode &arg0); // line 760
    void pop_back(); // line 775
    _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> insert(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, const CNode &arg1); // line 85
    void insert(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, unsigned int arg1, const CNode &arg2); // line 806
    _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> erase(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0); // line 95
    _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> erase(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg1); // line 871
    void swap(/*anon struct*/ int &arg0); // line 888
    void clear(); // line 898
    void splice(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, /*anon struct*/ int &arg1); // line 915
    void splice(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, /*anon struct*/ int &arg1, _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg2); // line 931
    void splice(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, /*anon struct*/ int &arg1, _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg2, _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg3); // line 953
    void remove(const CNode &arg0); // line 174
    void unique(); // line 191
    void merge(/*anon struct*/ int &arg0); // line 211
    void reverse(); // line 1051
    void sort(); // line 238
    protected:
    void _M_fill_assign(unsigned int arg0, const CNode &arg1); // line 142
    void _M_fill_insert(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, unsigned int arg1, const CNode &arg2); // line 1123
    void _M_transfer(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg1, _List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg2); // line 1132
    void _M_insert(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0, const CNode &arg1); // line 1137
    void _M_erase(_List_iterator<ZenDataWithVersion<unsigned int, SecagentGameExe, std::equal_to<unsigned int> >::CNode> arg0); // line 1145
    public:
    void ~list();
} data_list_;
size_t max_reserve_version_;
public:
void ZenDataWithVersion(unsigned int arg0); // line 67
void ~ZenDataWithVersion(); // line 73
const SecagentGameExe * get_data(const unsigned int &key) const; // line 97
const SecagentGameExe * get_newest_data() const; // line 114
const unsigned int * get_newest_key() const; // line 126
bool add_new_data(const unsigned int &key, auto_ptr<SecagentGameExe> &obj); // line 169
bool add_new_data_cover_of(const unsigned int &arg0, auto_ptr<SecagentGameExe> &arg1); // line 138
void set_max_reserve_version(unsigned int arg0); // line 90
};
extern /*anon struct*/ int iter; // line 99
extern /*anon struct*/ int node; // line 178

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_DATA_WITH_VERSION_H_H_
