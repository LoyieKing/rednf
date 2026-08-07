#ifndef HANDLERFOR_DB__H_
#define HANDLERFOR_DB__H_

#include "IHandler.h"
#include "DBConnection.h"
#include "DBConnections.h"
#include "TEA.h"
#include "MsgCell.h"
#include "DBTransactionDesign.h"
#include "TeaInitialize.h"

struct tagGAME_DB_SEND_PACKAGE;

class HandlerFor_DB_ : public nsl::IDBHandler
{
public:
    typedef unsigned int (HandlerFor_DB_::*DBHandlerFunc)(nsl::CMsgCell*);

    HandlerFor_DB_()
    {
    }
    virtual ~HandlerFor_DB_();
    void init();
    char* GetYYYYMM();
    DBHandlerFunc SearchDBHandlerFunc(int typeId);
    void DecryptPassword(const char* pInput, char* pOutput);
    void registFuncMap();
    bool GetAuctionMainFetchResult(nsl::DBConnection* db,
                                   tagAUCTION_DB_GET_REGISTED_ITEM* pContext);
    unsigned int insertPackage(nsl::DBConnection* db, int charac_no, int send_charac_no,
                               const char* send_charac_name, const char* letter_text,
                               unsigned int& letter_id, time_t insertTime);
    unsigned int insertPackageData(nsl::DBConnection* db, tagGAME_DB_SEND_PACKAGE* sendPackageInfo,
                                   unsigned int& postal_id, int mailType);
    unsigned int changeAvatarOwner(nsl::DBConnection* db, unsigned int src_item_key,
                                   unsigned int src_charac_no, unsigned int trg_charac_no);

    void TeaInitialize();
    void TeaInitialize_kor();
    void TeaInitialize_jpn();
    void TeaInitialize_china();
    void TeaInitialize_taiwan();
    void TeaInitialize_usa();

    unsigned int onAUCTION_DB_GET_REGISTED_ITEM(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_REGIST_ITEM(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_UPPER_BIDDING(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_INSERT_AVERAGE_PRICE(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_GET_AVERAGE_PRICE(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_GET_ROI_AVERAGE_PRICE(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_UPDATE_AVERAGE_PRICE(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_DELETE_ITEM(nsl::CMsgCell* pCell);
    unsigned int onGAME_DB_SEND_PACKAGE(nsl::CMsgCell* pCell);
    unsigned int onGAME_DB_SEND_PACKAGE_BY_EXPIRE(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_EXPIRE_HISTORY(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_BUYER_HISTORY(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_PING(nsl::CMsgCell* pCell);
    unsigned int onAUCTION_DB_UPDATE_ITEM(nsl::CMsgCell* pCell);

private:
    DBHandlerFunc mArrayFunc[126];
    char mSzBuffer[24576];
    nsl::CTEA mTEA;
};

#endif // HANDLERFOR_DB__H_
