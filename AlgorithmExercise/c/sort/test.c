#include <stdio.h>

int main(int argc, char* argv[])
{
	vector_type(char*) vec = NULL;
	vector_push_back(vec, "123");
	vector_push_back(vec, "1323");
	vector_push_back(vec, "12443");
	vector_push_back(vec, "12355");

	printf("%d\n", get_vector_size(vec));

	for(char* beg = vector_begin(vec); beg != vector_end(vec); ++beg)
	{
		printf("%s\n", *beg);
	}
}
