#ifndef RELIABILITYDICTIONARY_H_
#define RELIABILITYDICTIONARY_H_

class Auction;

class ReliabilityDictionary
{
public:
    ReliabilityDictionary();
    virtual ~ReliabilityDictionary();

    Auction* mpAuction;  // @4
};

#endif // RELIABILITYDICTIONARY_H_
