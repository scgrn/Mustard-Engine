#include "../main/math/matrix.h"

static void testMatrixEquality() {
    TestSuite suite("Matrix equality");

    float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    AB::Mat3 a(data);
    AB::Mat3 b(data);

    suite.assert(a == b, "matrix equality");
    suite.assert((a != b) == false, "matrix inequality");
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

    float data[] = {1, 4, 7, 2, 5, 8, 3, 6, 9};
    a = AB::Mat3(data);
    suite.assert(a * b == a, "Arbitrary x identity multiplication");
    suite.assert(b * a == a, "Arbitrary x identity multiplication");

    float data2[] = {9, 6, 3, 8, 5, 2, 7, 4, 1};
    b = AB::Mat3(data2);
    float data3[] = {30, 84, 138, 24, 69, 114, 18, 54, 90};
    c = AB::Mat3(data3);
    suite.assert(a * b == c, "Known values multiplication");
}

void testMatrix() {
    testMatrixEquality();
    testMultiplication();
}
