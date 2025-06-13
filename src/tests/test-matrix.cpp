#include "test.cpp"

#include "../main/math/matrix.h"

static void testEquality() {
    TestSuite suite("Matrix equality");

    float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    AB::Mat3 a(data);
    AB::Mat3 b(data);

    suite.assert(a == b, "matrix equality");
    suite.assert((a != b) == false, "matrix equality");
}

static void testMultiplication() {
    TestSuite suite("Matix multiplication");

    AB::Mat3 a;
    AB::Mat3 b;
    AB::Mat3 c;
    a.loadIdentity();
    b.loadIdentity();
    c.loadIdentity();

    suite.assert(a * b == c, "Identity multiplication");
}

int main(int argc, char* argv[]) {
    startTests();
    {
        testEquality();
        testMultiplication();
    }
    endTests();

    return 0;
}

