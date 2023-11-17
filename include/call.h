#ifndef CALL_H
#define CALL_H

#include "uuid_v4.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <string>


class Call {
public:
    Call(const std::string& number);

    const std::string& getID() const;
    const std::string& getNumber() const;
    const boost::posix_time::ptime& getTimeIncoming() const;

private:
    boost::posix_time::ptime _time_incoming;

    std::string _phone_number{};
    std::string _id{};
};

#endif
