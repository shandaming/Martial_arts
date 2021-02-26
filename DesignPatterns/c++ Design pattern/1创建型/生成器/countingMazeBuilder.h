#ifndef COUNTINGMAZEBUILDER_H
#define COUNTINGMAZEBUILDER_H

class CountingMazeBuilder:public MazeBuilder
{
	public:
		explicit CountingMazeBuilder():doors(0),rooms(0){}

		virtual void buildMaze();
		virtual void buildRoom(int n) {++rooms;}
		virtual void buildDoor(int r1,int r2) {++doors;}
		virtual void addWall(int r,Direction);

		void getCounts(int& r,int& d) const {r=rooms;d=doors;}
	private:
		int doors,rooms;
};

#endif
