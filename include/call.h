#include "uuid_v4.h"
#include <string>

class Call {
public:
    Call(const std::string& number);

    const std::string& getID() const;
    const std::string& getNumber() const;

private:
    std::string _phone_number{};
    std::string _id{};
};