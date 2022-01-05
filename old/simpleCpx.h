#ifndef SIMPLEC_H
#define SIMPLEC_H


class cpx{

// constructor
	cpx ( float a, float b ): a(a), b(b){};

// real and imaginary values
	float a;
	float b;

// iterate as a julia point
	cpx* iter(cpx C);
};

#endif
