#include "../main/math/vector.cpp"
#include "../main/math/matrix.cpp"
#include "../main/math/plane.cpp"

static void testAxisAlignedPlaneIntersection() {
    TestSuite suite("Axis-aligned plane intersection");

    AB::Plane px(AB::Vec3(1, 2, 3), AB::Vec3(1, 0, 0)); // x=1
    AB::Plane py(AB::Vec3(1, 2, 3), AB::Vec3(0, 1, 0)); // y=2
    AB::Plane pz(AB::Vec3(1, 2, 3), AB::Vec3(0, 0, 1)); // z=3

    AB::Vec3 p;
    AB::b32 intersects = findIntersection(px, py, pz, p);

    suite.assert(fabs(p.x - 1.0f) < AB::EPSILON, "Axis-aligned plane intersection");
    suite.assert(fabs(p.y - 2.0f) < AB::EPSILON, "Axis-aligned plane intersection");
    suite.assert(fabs(p.z - 3.0f) < AB::EPSILON, "Axis-aligned plane intersection");
}

void testPlaneIntersection() {
    testAxisAlignedPlaneIntersection();
}
