#include "gtest/gtest.h"
#include "randomizer.h"

#include <memory>

namespace {

    class RandomizerTest : public ::testing::Test {
    protected:
        void SetUp() {
           randomizer = std::make_unique<Randomizer>(min_queue, max_queue,
                                                     min_call, max_call);
        }

        std::unique_ptr<Randomizer> randomizer;
        int min_queue = 0;
        int max_queue = 100;
        int min_call = 0;
        int max_call = 0;
    };

    TEST_F(RandomizerTest, GetQueueTimeGreatOrEqualMinQueueTime) {
        ASSERT_GE(randomizer->getQueueTime(), min_queue);
    }

    TEST_F(RandomizerTest, GetQueueTimeLessOrEqualMaxQueueTime) {
        ASSERT_LE(randomizer->getQueueTime(), max_queue);
    }

    TEST_F(RandomizerTest, GetCallTimeGreatOrEqualMinCallTime) {
        ASSERT_GE(randomizer->getCallTime(), min_call);
    }

    TEST_F(RandomizerTest, GetCallTimeLessOrEqualMaxCallTime) {
        ASSERT_LE(randomizer->getCallTime(), max_call);
    }
}