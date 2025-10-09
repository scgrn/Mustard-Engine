#include <cstdlib>

void testProjectBuild() {
    TestSuite suite("Project build... (be patient)");

    int ret = system("./test-project-build.sh > /dev/null 2>&1");

    suite.assert(ret == 0, "project build");
}

