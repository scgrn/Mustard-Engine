class XorshiftPRNGTest {
public:
    // Test seeding
    void testSeed();
    void testSeed(u32 seed);

    // Test random number generation
    void testRnd();
    void testRnd(u32 n);
    void testRnd(i32 lb, i32 ub);

    // Test repeatability
    void testRepeatability();

    // Test distribution
    void testDistribution();

    // Test correlation
    void testCorrelation();

    void testEntropy();
    
    // Test range
    void testRange();

    // Test performance
    void testPerformance();

    // Test error handling
    void testErrorHandling();
};

