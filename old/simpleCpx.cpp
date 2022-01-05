#include <simpleCpx.h>


cpx* cpx::iter(cpx C){
	return new cpx( a*a-C.a*C.a, 2*a*C.a );
};
