#include <gtest/gtest.h>
#include "SettlementEngine.h"

// --- TEST CASES ---

// 1. Basic Settlement Check
TEST(SettlementEngineTest, DirectTransaction) {
    vector<Bank> banks(3);
    // RBI at 0
    banks[0] = {"RBI", 0}; banks[0].optimizedModes.set();
    
    // A owes 100, B is owed 100. Both use GPay (Bit 0).
    banks[1] = {"A", -100}; banks[1].optimizedModes.set(0);
    banks[2] = {"B", 100};  banks[2].optimizedModes.set(0);

    // Should be 1 transaction (A -> B)
    int tx = optimizedSolver(3, banks);
    EXPECT_EQ(tx, 1);
}

// 2. Incompatible Modes (Routing Verification)
TEST(SettlementEngineTest, RoutesViaRBI_WhenModesMismatch) {
    vector<Bank> banks(3);
    banks[0] = {"RBI", 0}; banks[0].optimizedModes.set();

    // A uses GPay (Bit 0), B uses Paytm (Bit 2)
    banks[1] = {"A", -100}; banks[1].optimizedModes.set(0);
    banks[2] = {"B", 100};  banks[2].optimizedModes.set(2);

    // Should count as 2 ops (A->RBI, RBI->B)
    int tx = optimizedSolver(3, banks);
    EXPECT_EQ(tx, 2);
}

// 3. Circular Debt Optimization
TEST(SettlementEngineTest, CircularDebtResolvesToZero) {
    // A->B->C->A means net balance is 0 for everyone.
    vector<Bank> banks(3);
    banks[0] = {"A", 0};
    banks[1] = {"B", 0};
    banks[2] = {"C", 0};

    int tx = optimizedSolver(3, banks);
    EXPECT_EQ(tx, 0);
}

// 4. Large Scale Stability (Mini-Stress Test)
TEST(SettlementEngineTest, HandlesLargeInputsWithoutCrash) {
    int N = 1000;
    vector<Bank> banks(N);
    for(int i=0; i<N; i++) {
        banks[i] = {"B"+to_string(i), 0};
        banks[i].optimizedModes.set(0);
    }
    // Net zero system
    int tx = optimizedSolver(N, banks);
    EXPECT_EQ(tx, 0);
}

// 5. Sharding Logic Verification
TEST(SystemDesignTest, ShardingIsDeterministic) {
    string bankName = "HDFC_Mumbai_Branch";
    int shard1 = getBankShard(bankName);
    int shard2 = getBankShard(bankName);
    
    // Same name must ALWAYS hit the same shard
    EXPECT_EQ(shard1, shard2);
    // Ensure result is within bounds [0,3]
    EXPECT_TRUE(shard1 >= 0 && shard1 < 4);
}

// Main entry point for GTest
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}