#include "../main/math/vector.h"

static void testVectorEquality() {
    TestSuite suite("Equality");

    AB::Vec3 a = AB::Vec3(1, 1, 1);
    AB::Vec3 b = AB::Vec3(1, 1, 1);
    suite.assert(a == b, "operator==");

    a = AB::Vec3(1, 1, 1);
    b = AB::Vec3(0, 1, 0);
    suite.assert(a != b, "operator!=");
}

void testVector() {
    testVectorEquality();
}

