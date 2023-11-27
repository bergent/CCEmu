#include "operator.h"

std::mutex write_lock;

Operator::Operator(CallCenter* parentCC) 
    :_id{_id_counter++}, _parentCC{parentCC}
{}

int Operator::getID() const {
    return _id;
}

void Operator::setCurrentCall(std::shared_ptr<Call> call) {
    _currentCall = call;
}

std::shared_ptr<Call> Operator::getCurrentCall() {
    return _currentCall;
}

const std::shared_ptr<Call> Operator::getCurrentCall() const {
    return _currentCall;
}

void Operator::RunCall(int call_duration) {
    
    std::thread thr {&Operator::StartConversation, this, call_duration};
    thr.detach();
   
}

void Operator::StartConversation(int call_duration) {
    std::cout << "Conversation for Operator #" << _id << " with number " << _currentCall->getNumber() << " started.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(call_duration));
    std::cout << "Conversation for Operator #" << _id << " with number " << _currentCall->getNumber() << " finished.\n";

    _currentCall->getCDR().release_time = boost::posix_time::microsec_clock::local_time();

    std::unique_lock lock(write_lock);

    if (_parentCC) {
        _parentCC->SetOperatorReady(this);
        _parentCC->EndActiveSession(_currentCall->getNumber());
        _parentCC->WriteCDR(_currentCall->getCDR());
    }
    
    lock.unlock();

    _currentCall = nullptr;
}

