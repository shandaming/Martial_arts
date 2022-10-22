#ifndef MAZE_H
#define MAZE_H

class Maze
{
	public:
		explicit Maze();

		void addRoom(Room* r);
		Room* roomNo(int room) const;
	private:
		//...
};

#endif
