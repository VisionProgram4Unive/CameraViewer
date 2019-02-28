#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <cstddef>

class Resolution
{
public :
    Resolution(size_t w = 0, size_t h = 0):width(w),height(h) {}
    const size_t & getWidth()  const { return width; }
    const size_t & getHeight() const { return height; }
    friend inline bool operator ==(const Resolution & lhs, const Resolution & rhs)
    {
        if( (lhs.getWidth()==rhs.getWidth()) && (lhs.getHeight()==rhs.getHeight()) )
            return true;
        else
            return false;
    }
    friend inline bool operator !=(const Resolution & lhs, const Resolution & rhs)
    {
        if(  lhs == rhs )
            return false;
        else
            return true;
    }

private :
    size_t width;
    size_t height;
};

#endif// RESOLUTION_H
