#include "tree.h"

int main()
{
	Tree_manager* m = new Tree_manager(new Green_tree(green), 
			new Yellow_tree(yellow));
	m->display(Pos(10, 11), Pos(20, 21));
}
