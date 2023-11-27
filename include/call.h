#ifndef CCEMU_CALL_H
#define CCEMU_CALL_H

#include "boost/date_time/posix_time/posix_time.hpp"

#include "cdrentry.h"
#include <string>


class Call {
public:
    Call(const std::string& number, const std::string& id);

    const std::string& getID() const;
    const std::string& getNumber() const;

    CDREntry& getCDR();
    const CDREntry& getCDR() const;

    void SetQueueTime(int ms);
    const boost::posix_time::ptime& GetQueueTime();

    friend bool operator== (const Call& lhs, const Call& rhs) {
        return lhs.getID() == rhs.getID();
    }

private:
    CDREntry _call_cdr;
    std::string _phone_number{};
    std::string _id{};
    boost::posix_time::ptime _queue_time {boost::posix_time::not_a_date_time};
};

#endif
