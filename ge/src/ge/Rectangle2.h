/**
*
* Copyright (c) 2014 Matthew Newhook. All rights reserved.
*
*/
#pragma once
#include <ge/Config.h>

namespace ge {
class Rectangle2 {         // TODO implements Savable, Externalizable {
public:

    /**
     * Constructor creates a new <code>Rectangle2</code> with using the given x,y,width and height values.
     *
     */
    Rectangle2(int x = 0, int y = 0, int width = 0, int height = 0) : _x(x), _y(y), _width(width), _height(height){
    }

    ~Rectangle2() {
    }

    int getX() const {
        return _x;
    }

    /**
     * @param x
     *            the new x coordinate of the origin of this rectangle
     */
    void setX(int x) {
        _x = x;
    }

    int getY() const {
        return _y;
    }

    /**
     * @param y
     *            the new y coordinate of the origin of this rectangle
     */
    void setY(int y) {
        _y = y;
    }

    int getWidth() const {
        return _width;
    }

    /**
     * @param width
     *            the new width of this rectangle
     */
    void setWidth(int width) {
        _width = width;
    }

    int getHeight() const {
        return _height;
    }

    /**
     * @param height
     *            the new height of this rectangle
     */
    void setHeight(int height) {
        _height = height;
    }

    Rectangle2& set(int x, int y, int width, int height) {
        _x = x;
        _y = y;
        _width = width;
        _height = height;
        return *this;
    }

    Rectangle2 intersect(const Rectangle2& other);

    static Rectangle2 intersect(const Rectangle2& src1, const Rectangle2& src2);

    /**
     * @return the string representation of this rectangle.
     */
    /* TODO
       @Override
       public String toString() {
            return "com.ardor3d.math.Rectangle2 [origin: " + _x + ", " + _y + "  width: " + _width + " height: " + _height
     + "]";
       }
     */

    /**
     * @param op
     *            the object to compare for equality
     * @return true if this rectangle and the provided rectangle have the same origin and dimensions
     */
    bool operator==(const Rectangle2& op) const {
        return (_x == op._x) && (_y == op._y) && (_width == op._width) && (_height == op._height);
    }

    bool operator!=(const Rectangle2& op) const {
        return !(*this == op);
    }

    // /////////////////
    // Methods for Savable
    // /////////////////

    /* TODO
       public void write(final OutputCapsule capsule) throws IOException {
            capsule.write(_x, "x", 0);
            capsule.write(_y, "y", 0);
            capsule.write(_width, "width", 0);
            capsule.write(_height, "height", 0);
       }

       public void read(final InputCapsule capsule) throws IOException {
            _x = capsule.readInt("x", 0);
            _y = capsule.readInt("y", 0);
            _width = capsule.readInt("width", 0);
            _height = capsule.readInt("height", 0);
       }

       public Class<? extends Rectangle2> getClassTag() {
            return this.getClass();
       }
     */

    // /////////////////
    // Methods for Externalizable
    // /////////////////

    /**
     * Used with serialization. Not to be called manually.
     *
     * @param in
     *            ObjectInput
     * @throws IOException
     * @throws ClassNotFoundException
     */
    /* TODO
       public void readExternal(final ObjectInput in) throws IOException, ClassNotFoundException {
            setX(in.readInt());
            setY(in.readInt());
            setWidth(in.readInt());
            setHeight(in.readInt());
       }
     */

    /**
     * Used with serialization. Not to be called manually.
     *
     * @param out
     *            ObjectOutput
     * @throws IOException
     */
    /* TODO
       public void writeExternal(final ObjectOutput out) throws IOException {
            out.writeInt(_x);
            out.writeInt(_y);
            out.writeInt(_width);
            out.writeInt(_height);
       }
     */

private:

    int _x;
    int _y;
    int _width;
    int _height;
};
}
