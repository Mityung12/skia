/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkGeometry.h"
#include "Test.h"
#include "SkRandom.h"

static bool nearly_equal(const SkPoint& a, const SkPoint& b) {
    return SkScalarNearlyEqual(a.fX, b.fX) && SkScalarNearlyEqual(a.fY, b.fY);
}

static void testChopCubic(skiatest::Reporter* reporter) {
    /*
        Inspired by this test, which used to assert that the tValues had dups

        <path stroke="#202020" d="M0,0 C0,0 1,1 2190,5130 C2190,5070 2220,5010 2205,4980" />
     */
    const SkPoint src[] = {
        { SkIntToScalar(2190), SkIntToScalar(5130) },
        { SkIntToScalar(2190), SkIntToScalar(5070) },
        { SkIntToScalar(2220), SkIntToScalar(5010) },
        { SkIntToScalar(2205), SkIntToScalar(4980) },
    };
    SkPoint dst[13];
    SkScalar tValues[3];
    // make sure we don't assert internally
    int count = SkChopCubicAtMaxCurvature(src, dst, tValues);
    if (false) { // avoid bit rot, suppress warning
        REPORTER_ASSERT(reporter, count);
    }
}

static void test_evalquadat(skiatest::Reporter* reporter) {
    SkRandom rand;
    for (int i = 0; i < 1000; ++i) {
        SkPoint pts[3];
        for (int j = 0; j < 3; ++j) {
            pts[j].set(rand.nextSScalar1() * 100, rand.nextSScalar1() * 100);
        }
        SkScalar t = 0;
        const SkScalar dt = SK_Scalar1 / 128;
        for (int j = 0; j < 128; ++j) {
            SkPoint r0;
            SkEvalQuadAt(pts, t, &r0);
            SkPoint r1 = SkEvalQuadAt(pts, t);
            bool eq = SkScalarNearlyEqual(r0.fX, r1.fX) && SkScalarNearlyEqual(r0.fY, r1.fY);
            if (!eq) {
                SkDebugf("[%d %g] p0 [%10.8f %10.8f] p1 [%10.8f %10.8f]\n", i, t, r0.fX, r0.fY, r1.fX, r1.fY);
                REPORTER_ASSERT(reporter, eq);
            }
            t += dt;
        }
    }
}

DEF_TEST(Geometry, reporter) {
    SkPoint pts[3], dst[5];

    pts[0].set(0, 0);
    pts[1].set(100, 50);
    pts[2].set(0, 100);

    int count = SkChopQuadAtMaxCurvature(pts, dst);
    REPORTER_ASSERT(reporter, count == 1 || count == 2);

    pts[0].set(0, 0);
    pts[1].set(SkIntToScalar(3), 0);
    pts[2].set(SkIntToScalar(3), SkIntToScalar(3));
    SkConvertQuadToCubic(pts, dst);
    const SkPoint cubic[] = {
        { 0, 0, },
        { SkIntToScalar(2), 0, },
        { SkIntToScalar(3), SkIntToScalar(1), },
        { SkIntToScalar(3), SkIntToScalar(3) },
    };
    for (int i = 0; i < 4; ++i) {
        REPORTER_ASSERT(reporter, nearly_equal(cubic[i], dst[i]));
    }

    testChopCubic(reporter);
    test_evalquadat(reporter);
}
