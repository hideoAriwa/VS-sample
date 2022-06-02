/* Ccomplex.h

    Ccomplex Number Library - Include File
    class Ccomplex:  declarations for Ccomplex numbers.

    Copyright (c) Borland International 1990,1991
    All Rights Reserved.

All function names, member names, and operators have been borrowed
from AT&T C++, except for the addition of:

    friend Ccomplex __cdecl acos(Ccomplex&);
    friend Ccomplex __cdecl asin(Ccomplex&);
    friend Ccomplex __cdecl atan(Ccomplex&);
    friend Ccomplex __cdecl log10(Ccomplex&);
    friend Ccomplex __cdecl tan(Ccomplex&);
    friend Ccomplex __cdecl tanh(Ccomplex&);
    Ccomplex __cdecl operator+();
    Ccomplex __cdecl operator-();
*/

#define PI 3.1415926535789

#ifndef _MYCOMPLEX_INCLUDED
#define _MYCOMPLEX_INCLUDED

#ifndef __cplusplus
#error Must use C++ for the type Ccomplex.
#endif

#if !defined( __COMPLEX_H )
#define _COMPLEX_H


#if !defined( __MATH_H )
#include <math.h>
#endif



class Ccomplex {

public:
    // constructors
        Ccomplex(double __re_val, double __im_val=0);
    Ccomplex();

    // Ccomplex manipulations
    friend double __cdecl real(Ccomplex&);   // the real part
    friend double __cdecl imag(Ccomplex&);   // the imaginary part
    friend Ccomplex __cdecl conj(Ccomplex&);  // the Ccomplex conjugate
    friend double __cdecl norm(Ccomplex&);   // the square of the magnitude
    friend double __cdecl arg(Ccomplex&);    // the angle in the plane

    // Create a Ccomplex object given polar coordinates
    friend Ccomplex __cdecl polar(double __mag, double __angle=0);

    // Overloaded ANSI C math functions
    friend double  __cdecl abs(Ccomplex&);
    friend Ccomplex __cdecl acos(Ccomplex&);
    friend Ccomplex __cdecl asin(Ccomplex&);
    friend Ccomplex __cdecl atan(Ccomplex&);
    friend Ccomplex __cdecl cos(Ccomplex&);
    friend Ccomplex __cdecl cosh(Ccomplex&);
    friend Ccomplex __cdecl exp(Ccomplex&);
    friend Ccomplex __cdecl log(Ccomplex&);
    friend Ccomplex __cdecl log10(Ccomplex&);
    friend Ccomplex __cdecl pow(Ccomplex& __base, double __expon);
    friend Ccomplex __cdecl pow(double __base, Ccomplex& __expon);
    friend Ccomplex __cdecl pow(Ccomplex& __base, Ccomplex& __expon);
    friend Ccomplex __cdecl sin(Ccomplex&);
    friend Ccomplex __cdecl sinh(Ccomplex&);
    friend Ccomplex __cdecl sqrt(Ccomplex&);
    friend Ccomplex __cdecl tan(Ccomplex&);
    friend Ccomplex __cdecl tanh(Ccomplex&);

    // Binary Operator Functions
    friend Ccomplex __cdecl operator+(Ccomplex&, Ccomplex&);
    friend Ccomplex __cdecl operator+(double, Ccomplex&);
        friend Ccomplex __cdecl operator+(Ccomplex&, double);
    friend Ccomplex __cdecl operator-(Ccomplex&, Ccomplex&);
    friend Ccomplex __cdecl operator-(double, Ccomplex&);
        friend Ccomplex __cdecl operator-(Ccomplex&, double);
    friend Ccomplex __cdecl operator*(Ccomplex&, Ccomplex&);
        friend Ccomplex __cdecl operator*(Ccomplex&, double);
    friend Ccomplex __cdecl operator*(double, Ccomplex&);
    friend Ccomplex __cdecl operator/(Ccomplex&, Ccomplex&);
        friend Ccomplex __cdecl operator/(Ccomplex&, double);
    friend Ccomplex __cdecl operator/(double, Ccomplex&);
    friend int __cdecl operator==(Ccomplex&, Ccomplex&);
    friend int __cdecl operator!=(Ccomplex&, Ccomplex&);

    Ccomplex& operator=(Ccomplex&);
    Ccomplex& operator=(double);

    Ccomplex& __cdecl operator+=(Ccomplex&);
    Ccomplex& __cdecl operator+=(double);
    Ccomplex& __cdecl operator-=(Ccomplex&);
    Ccomplex& __cdecl operator-=(double);
    Ccomplex& __cdecl operator*=(Ccomplex&);
    Ccomplex& __cdecl operator*=(double);
    Ccomplex& __cdecl operator/=(Ccomplex&);
    Ccomplex& __cdecl operator/=(double);
    Ccomplex __cdecl operator+();
    Ccomplex __cdecl operator-();

// Implementation
private:
        double re, im;
};


// Inline Ccomplex functions

inline Ccomplex::Ccomplex(double __re_val, double __im_val)
{
    re = __re_val;
    im = __im_val;
}

inline Ccomplex::Ccomplex()
{
/* if you want your Ccomplex numbers initialized ...
    re = im = 0;
*/
}

inline Ccomplex __cdecl Ccomplex::operator+()
{
    return *this;
}

inline Ccomplex __cdecl Ccomplex::operator-()
{
    return Ccomplex(-re, -im);
}


// Definitions of compound-assignment operator member functions

inline Ccomplex& __cdecl Ccomplex::operator+=(Ccomplex& __z2)
{
    re += __z2.re;
    im += __z2.im;
    return *this;
}

inline Ccomplex& __cdecl Ccomplex::operator+=(double __re_val2)
{
    re += __re_val2;
    return *this;
}

inline Ccomplex& __cdecl Ccomplex::operator-=(Ccomplex& __z2)
{
    re -= __z2.re;
    im -= __z2.im;
    return *this;
}

inline Ccomplex& __cdecl Ccomplex::operator-=(double __re_val2)
{
    re -= __re_val2;
    return *this;
}

inline Ccomplex& __cdecl Ccomplex::operator*=(Ccomplex& __z2)
{        
	Ccomplex Med(re,im);
    re = Med.re * __z2.re - Med.im * __z2.im;
    im = Med.re * __z2.im + Med.im * __z2.re;
    return *this;
}
inline Ccomplex& __cdecl Ccomplex::operator/=(Ccomplex& __z2)
{        
	Ccomplex Med(re,im);
    re = (Med.re * __z2.re + Med.im * __z2.im)/(__z2.re*__z2.re+__z2.im*__z2.im);
    im = (-Med.re * __z2.im + Med.im * __z2.re)/(__z2.re*__z2.re+__z2.im*__z2.im);
    return *this;
}
inline Ccomplex& __cdecl Ccomplex::operator*=(double __re_val2)
{
    re *= __re_val2;
    im *= __re_val2;
    return *this;
}

inline Ccomplex& __cdecl Ccomplex::operator/=(double __re_val2)
{
    re /= __re_val2;
    im /= __re_val2;
    return *this;
}

inline Ccomplex& Ccomplex::operator=(Ccomplex& __z2)
{
    re = __z2.re;
    im = __z2.im;
    return *this;
}


inline Ccomplex& Ccomplex::operator=(double __re_val2)
{
    re = __re_val2;
    im = 0;
    return *this;
}



// Definitions of non-member Ccomplex functions

inline double __cdecl real(Ccomplex& __z)
{
    return __z.re;
}

inline double __cdecl imag(Ccomplex& __z)
{
    return __z.im;
}
inline double __cdecl norm(Ccomplex& __z)
{
    return fabs(__z.re*__z.re)+fabs(__z.im*__z.im);
}

inline Ccomplex __cdecl conj(Ccomplex& __z)
{
    return Ccomplex(__z.re, -__z.im);
}

inline Ccomplex __cdecl exp(Ccomplex& __z)
{
	if(__z.im == 0)
		return Ccomplex(exp(__z.re),0);
    return Ccomplex(exp(__z.re) * cos(-__z.im), (__z.im/__z.im)*exp(__z.re)*sin(-__z.im));
}

inline Ccomplex __cdecl polar(double __mag, double __angle)
{
    return Ccomplex(__mag*cos(__angle), __mag*sin(__angle));
}

// Definitions of non-member binary operator functions

inline Ccomplex __cdecl operator+(Ccomplex& __z1, Ccomplex& __z2)
{
    return Ccomplex(__z1.re + __z2.re, __z1.im + __z2.im);
}

inline Ccomplex __cdecl operator*(Ccomplex& __z1, Ccomplex& __z2)
{
    return Ccomplex(__z1.re * __z2.re - __z1.im * __z2.im,
    				__z1.re * __z2.im + __z1.im * __z2.re);
}

inline Ccomplex __cdecl operator/(Ccomplex& __z1, Ccomplex& __z2)
{
    return Ccomplex((__z1.re * __z2.re + __z1.im * __z2.im)/(__z2.re * __z2.re + __z2.im * __z2.im),
    				(-__z1.re * __z2.im + __z1.im * __z2.re)/(__z2.re * __z2.re + __z2.im * __z2.im));
}

inline Ccomplex __cdecl operator/(double __re_val1, Ccomplex& __z2)
{
    return Ccomplex(__re_val1 *__z2.re/(__z2.re * __z2.re + __z2.im * __z2.im),
    				-__re_val1 *__z2.im/(__z2.re * __z2.re + __z2.im * __z2.im));
}

inline Ccomplex __cdecl operator+(double __re_val1, Ccomplex& __z2)
{
    return Ccomplex(__re_val1 + __z2.re, __z2.im);
}

inline Ccomplex __cdecl operator+(Ccomplex& __z1, double __re_val2)
{
    return Ccomplex(__z1.re + __re_val2, __z1.im);
}


inline Ccomplex __cdecl operator-(Ccomplex& __z1, Ccomplex& __z2)
{
    return Ccomplex(__z1.re - __z2.re, __z1.im - __z2.im);
}

inline Ccomplex __cdecl operator-(double __re_val1, Ccomplex& __z2)
{
    return Ccomplex(__re_val1 - __z2.re, -__z2.im);
}

inline Ccomplex __cdecl operator-(Ccomplex& __z1, double __re_val2)
{
    return Ccomplex(__z1.re - __re_val2, __z1.im);
}

inline Ccomplex __cdecl operator*(Ccomplex& __z1, double __re_val2)
{
    return Ccomplex(__z1.re*__re_val2, __z1.im*__re_val2);
}

inline Ccomplex __cdecl operator*(double __re_val1, Ccomplex& __z2)
{
    return Ccomplex(__z2.re*__re_val1, __z2.im*__re_val1);
}

inline Ccomplex __cdecl operator/(Ccomplex& __z1, double __re_val2)
{
    return Ccomplex(__z1.re/__re_val2, __z1.im/__re_val2);
}

inline int __cdecl operator==(Ccomplex& __z1, Ccomplex& __z2)
{
    return __z1.re == __z2.re && __z1.im == __z2.im;
}

inline int __cdecl operator!=(Ccomplex& __z1, Ccomplex& __z2)
{
    return __z1.re != __z2.re || __z1.im != __z2.im;
}



#endif
#endif  // __COMPLEX_H
