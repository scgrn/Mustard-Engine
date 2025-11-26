#include <iostream>

static int totalTestsRun = 0;
static int totalTestsFailed = 0;

class TestSuite {
    public:
        TestSuite(const std::string& name) {
            testsRun = 0;
            testsFailed = 0;
            std::cout << "TESTING: " << name << std::endl;
        }

        ~TestSuite() {
            totalTestsRun += testsRun;
            totalTestsFailed += testsFailed;
        }

        void assert(bool condition, const std::string& message) {
            testsRun++;
            if (!condition) {
                testsFailed++;
                std::cout << "Assert failed: " << message << std::endl;
            }
        }

    private:
        int testsRun;
        int testsFailed;
};

#include "test-rng.cpp"
#include "test-vector.cpp"
#include "test-matrix.cpp"
#include "test-plane-intersection.cpp"
#include "test-project-build.cpp"

int main(int argc, char* argv[]) {
    std::cout << "============= Running tests =============" << std::endl;

    testRng();
    testVector();
    testMatrix();
    testPlaneIntersection();
    testProjectBuild();

    std::cout << "============= Tests complete ============" << std::endl;
    std::cout << "Total tests run: " << totalTestsRun << std::endl;
    if (totalTestsFailed > 0) {
        std::cout << totalTestsFailed << " tests failed!" << std::endl;
        return 1;
    }
    std::cout << "All tests passed!" << std::endl;

    return 0;
}

