#include "call.h"

Call::Call(const std::string& number) 
    : _phone_number{ number }
{
    UUIDv4::UUIDGenerator<std::mt19937_64> uuid_generator;
    UUIDv4::UUID uuid { uuid_generator.getUUID() };
    _id = uuid.str();
}

const std::string& Call::getID() const {
    return _id;
}

const std::string& Call::getNumber() const {
    return _phone_number;
}