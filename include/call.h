#ifndef CCEMU_CALL_H
#define CCEMU_CALL_H

#include "uuid_v4.h"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "cdrentry.h"
#include <string>


class Call {
public:
    Call(const std::string& number);

    const std::string& getID() const;
    const std::string& getNumber() const;
    const boost::posix_time::ptime& getTimeIncoming() const;

    CDREntry& getCDR();
    const CDREntry& getCDR() const;

    friend bool operator== (const Call& lhs, const Call& rhs) {
        return lhs.getID() == rhs.getID();
    }

private:
    boost::posix_time::ptime _time_incoming;

    CDREntry _call_cdr;
    std::string _phone_number{};
    std::string _id{};
};

#endif
