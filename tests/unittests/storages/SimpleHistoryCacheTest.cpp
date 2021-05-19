/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include "gtest/gtest.h"
#include <limits>

#include "rtps/rtps.h"
#include "rtps/storages/SimpleHistoryCache.h"

using rtps::SimpleHistoryCache;
using rtps::SequenceNumber_t;


class SimpleHistory : public ::testing::Test{
protected:
    SimpleHistoryCache history;
    std::array<uint8_t, 10> someData{};
};

TEST_F(SimpleHistory, ReturnsUnknownSequenceNumberIfEmpty){
    EXPECT_EQ(history.getSeqNumMin(), rtps::SEQUENCENUMBER_UNKNOWN);
    EXPECT_EQ(history.getSeqNumMax(), rtps::SEQUENCENUMBER_UNKNOWN);
}


TEST_F(SimpleHistory, isNotFullAfterCreation){
    EXPECT_FALSE(history.isFull());
}

TEST_F(SimpleHistory, addChange_setsIncreasingSequenceNumbers){

    auto* change = history.addChange(someData.data(), someData.size());
    EXPECT_EQ(change->sequenceNumber, (SequenceNumber_t{0,1}));
    EXPECT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,1}));
    EXPECT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,1}));

    change = history.addChange(someData.data(), someData.size());
    EXPECT_EQ(change->sequenceNumber, (SequenceNumber_t{0,2}));
    EXPECT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,1}));
    EXPECT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,2}));
}

TEST_F(SimpleHistory, dropOldest_removesExactlyOne){
    history.addChange(someData.data(), someData.size());
    history.addChange(someData.data(), someData.size());
    history.addChange(someData.data(), someData.size());

    history.dropOldest();
    EXPECT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,2}));
    EXPECT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,3}));
}

TEST_F(SimpleHistory, removeUntil_dropsAllIfSNHigherThanMax){
    history.addChange(someData.data(), someData.size());
    history.addChange(someData.data(), someData.size());
    SequenceNumber_t maxSequenceNumber = history.getSeqNumMax();
    SequenceNumber_t SNToRemove = maxSequenceNumber; ++SNToRemove;

    history.removeUntilIncl(SNToRemove);

    EXPECT_EQ(history.getSeqNumMin(), rtps::SEQUENCENUMBER_UNKNOWN);
}

TEST_F(SimpleHistory, getChangeBySN_returnCorrectChange){
    history.addChange(someData.data(), someData.size());
    history.addChange(someData.data(), someData.size());
    SequenceNumber_t requestedSN{0,2};

    auto* change = history.getChangeBySN(requestedSN);

    ASSERT_NE(change, nullptr);
    EXPECT_EQ(change->sequenceNumber, requestedSN);
}

TEST_F(SimpleHistory, getChangeBySN_returnsNullptrIfOutOfRange){
    history.addChange(someData.data(), someData.size());
    history.addChange(someData.data(), someData.size());

    EXPECT_EQ(history.getChangeBySN({0,0}), nullptr);
    EXPECT_EQ(history.getChangeBySN({0,5}), nullptr);
}

TEST_F(SimpleHistory, removeUntil_doesntChangeHistoryIfSNLowerThanMin){
    history.addChange(someData.data(), someData.size());
    history.addChange(someData.data(), someData.size());

    history.removeUntilIncl(SequenceNumber_t{0,0});

    EXPECT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,1}));
    EXPECT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,2}));
}

class FullSimpleHistory : public ::testing::Test{
protected:
    SimpleHistoryCache history;
    std::array<uint8_t, 10> someData{};

    void SetUp() override{
        rtps::init();
        // No overflow in sequencenumber assumed
        ASSERT_LE(rtps::Config::HISTORY_SIZE - 1, std::numeric_limits<uint32_t>::max());

        for(size_t i=0; i < rtps::Config::HISTORY_SIZE; ++i){
            auto* change = history.addChange(someData.data(), someData.size());
        }
        ASSERT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,1} ));
        ASSERT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,rtps::Config::HISTORY_SIZE}));
    }
};

TEST_F(FullSimpleHistory, isFull){
    EXPECT_TRUE(history.isFull());
}

TEST_F(FullSimpleHistory, addChange_OverridesFirstIfFull){

    auto* change = history.addChange(someData.data(), someData.size());

    EXPECT_EQ(change->sequenceNumber, (SequenceNumber_t{0,rtps::Config::HISTORY_SIZE + 1}));
    EXPECT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,2} ));
    EXPECT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,rtps::Config::HISTORY_SIZE + 1}));
}

TEST_F(FullSimpleHistory, findsMostRecentElementAfterOverflowByOne){
    history.addChange(someData.data(), someData.size()); // fill additional element

    auto* change = history.addChange(someData.data(), someData.size());
    ASSERT_NE(change, nullptr);

    auto* foundChange = history.getChangeBySN(change->sequenceNumber);
    ASSERT_EQ(foundChange, change);
    EXPECT_EQ(foundChange->sequenceNumber, change->sequenceNumber);
}

TEST_F(FullSimpleHistory, removeUntil_leavesOnlyMoreRecentSNs){
    ASSERT_GE(rtps::Config::HISTORY_SIZE, 5);

    history.removeUntilIncl(rtps::SequenceNumber_t{0,3});

    EXPECT_EQ(history.getSeqNumMin(), (SequenceNumber_t{0,4}));
    EXPECT_EQ(history.getSeqNumMax(), (SequenceNumber_t{0,rtps::Config::HISTORY_SIZE}));
}

class SimpleHistoryMock : public SimpleHistoryCache{
public:
    explicit SimpleHistoryMock(SequenceNumber_t sn) : SimpleHistoryCache(sn){

    }
};

class SimpleHistoryWithHighSN : public ::testing::Test{
protected:
    std::array<uint8_t, 10> someData{};
    const uint32_t entriesUntilSNOverflow = 3;
    const uint32_t numAdditionalEntries = 4;
    const SequenceNumber_t lastSNUsed{0, std::numeric_limits<uint32_t>::max() - entriesUntilSNOverflow};
    SimpleHistoryMock history{lastSNUsed};

    void SetUp() override{
        rtps::init();
        for(uint32_t i = 0; i < entriesUntilSNOverflow + numAdditionalEntries; ++i){
            history.addChange(someData.data(), someData.size());
        }
    }
};

TEST_F(SimpleHistoryWithHighSN, removeUntil_dropsChangesWithOverflowedSN){

    SequenceNumber_t maxSequenceNumber = history.getSeqNumMax();
    SequenceNumber_t SNToRemove{maxSequenceNumber.high, maxSequenceNumber.low - 3};
    SequenceNumber_t expectedMinSN = SNToRemove;
    ++expectedMinSN;

    history.removeUntilIncl(SNToRemove);

    EXPECT_EQ(history.getSeqNumMin(), expectedMinSN);
}

TEST_F(SimpleHistoryWithHighSN, getChangeBySN_worksWithSNOverflow){

    SequenceNumber_t requestedSN = history.getSeqNumMax();

    auto* change = history.getChangeBySN(requestedSN);

    ASSERT_NE(change, nullptr);
    EXPECT_EQ(change->sequenceNumber, requestedSN);
}



