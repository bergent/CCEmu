#include "operator.h"

std::mutex write_lock;

// Constructor for operator
Operator::Operator(CallCenter* parentCC) 
    :_id{_id_counter++}, _parentCC{parentCC}
{}

// ID Getter
int Operator::getID() const {
    return _id;
}

// Set call as current for operator
void Operator::setCurrentCall(std::shared_ptr<Call> call) {
    _currentCall = call;
}

// Current call getter
std::shared_ptr<Call> Operator::getCurrentCall() {
    return _currentCall;
}

const std::shared_ptr<Call> Operator::getCurrentCall() const {
    return _currentCall;
}

// Thread starter for call session
void Operator::RunCall(int call_duration) {
    
    std::thread thr {&Operator::StartConversation, this, call_duration};
    thr.detach();
   
}

// Thread execution function
// Will make thread sleep for randomized time call_duration
// and pass the resulting CDREntry to CallCenter handler
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

