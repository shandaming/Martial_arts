/*
 * Copyright (C) 2018
 */

#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <string>

enum Color
{
	green,
	yellow
};

struct Pos
{
	Pos(int x, int y) : x(x), y(y) {}

	int x;
	int y;
};

class Tree
{
	public:
		explicit Tree(const Color c) : color_(c) {}

		virtual void draw() = 0;
	protected:
		Color color_;
};

class Green_tree : public Tree
{
	public:
		explicit Green_tree(const Color c) : Tree(c) {}

		void draw() override
		{
			std::cout << color_ << " Green tree\n";
		}
};

class Yellow_tree : public Tree
{
	public:
		explicit Yellow_tree(const Color c) : Tree(c) {}

		void draw() override
		{
			std::cout << color_ << " Yellow tree\n";
		}
};

class Tree_manager
{
	public:
		Tree_manager(Tree* green_tree, Tree* yellow_tree) :
			green_tree_(green_tree), yellow_tree_(yellow_tree) {}

		void display(const Pos& green_tree_pos, const Pos& yellow_tree_pos)
		{
			pos_.push_back(green_tree_pos);
			pos_.push_back(yellow_tree_pos);

			green_tree_->draw();
			std::cout << "position: " << pos_[0].x << " " << pos_[0].y 
				<< "\n";

			yellow_tree_->draw();
			std::cout << "Position: " << pos_[1].x << " " << pos_[1].y
				<< std::endl;
		}
	private:
		std::vector<Pos> pos_;

		Tree* green_tree_;
		Tree* yellow_tree_;
};

#endif
