/*
 * Copyright (C) 2018
 */

#ifndef PLAYER_H
#define PLAYER_H

enum Sex
{
	MALE = 0,		// 男性
	FEMALE,		// 女性
	SHEMALE		// 人妖
}

class Character
{
	public:
		Character(const std::string& name, Sex sex, int age) :
			name_(name), sex_(sex), age_(age) {}

		virtual ~Character(){}

		std::string get_name() const { return name_; }
		Sex get_sex() const { return sex_; }
		int get_age() const { return age_; }

		virtual void move(int x, int y) = 0
	protected:
		// 角色名
		std::string name_;
		// 性别
		Sex sex_;
		// 年龄
		int age_;
};

class Player : public Character
{
	public:
		void move(int x, int y) override {}
	private:
};

#endif
