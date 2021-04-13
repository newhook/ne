/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/

#include <ge/Config.h>
#include <ge/Rectangle2.h>
#include <algorithm>

using namespace ge;
using namespace std;

Rectangle2 Rectangle2::intersect(const Rectangle2& other) {
    int x1 = max(getX(), other.getX());
    int y1 = max(getY(), other.getY());
    int x2 = min(getX() + getWidth(), other.getX() + other.getWidth());
    int y2 = min(getY() + getHeight(), other.getY() + other.getHeight());
    return Rectangle2(x1, y1, x2 - x1, y2 - y1);
}

Rectangle2 Rectangle2::intersect(const Rectangle2& src1, const Rectangle2& src2) {
    int x1 = max(src1.getX(), src2.getX());
    int y1 = max(src1.getY(), src2.getY());
    int x2 = min(src1.getX() + src1.getWidth(), src2.getX() + src2.getWidth());
    int y2 = min(src1.getY() + src1.getHeight(), src2.getY() + src2.getHeight());
    return Rectangle2(x1, y1, x2 - x1, y2 - y1);
}

