#include "observer.h"

int main()
{
	Subject* s = new Subject();
	Display1* d1 = new Display1(s);
	Display2* d2 = new Display2(s);
	s->notify();
}
