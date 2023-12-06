#include "gtest/gtest.h"
#include "call.h"

#include <memory>

namespace {
    
    class CallTest : public ::testing::Test {
    protected:
        void SetUp() {
            call = std::make_unique<Call>(number, id);
        }
    
        std::unique_ptr<Call> call;
        std::string number {"+74012805805"};
        std::string id {"11bf5b37-e0b8-42e0-8dcf-dc8c4aefc000"};
    };

    TEST_F(CallTest, GetIDReturnValueEqualInitial) {
        ASSERT_EQ(call->getID(), id);
    }

    TEST_F(CallTest, GetNumberReturnValueEqualInitial) {
        ASSERT_EQ(call->getNumber(), number);
    }

    TEST_F(CallTest, SetQueueTimeChangesQueueTime) {
        int time {50};
        auto initial_time {call->GetQueueTime()};

        call->SetQueueTime(time);

        ASSERT_NE(call->GetQueueTime(), initial_time);
    }

    TEST_F(CallTest, CDRIDEqualCallID) {
        ASSERT_EQ(call->getCDR().call_id, call->getID());
    }

    TEST_F(CallTest, CDRNumberEqualCallNumber) {
        ASSERT_EQ(call->getCDR().phone_number, call->getNumber());
    }

}