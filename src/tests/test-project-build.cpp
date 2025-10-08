#include <cstdlib>

void testProjectBuild() {
    TestSuite suite("Project build");

    int ret = system("./test-project-build.sh");

    suite.assert(ret == 0, "project build");
}

