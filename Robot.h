#include <stdio.h>
#include <string.h>

#pragma once

#pragma warning (disable:4996)

#define SIZE 9

struct QuestionAnswer { const char* question; const char* answer;};
struct RobotoAnswer { bool haveAnswer;char answer[100]; char action[25]; };

class Robot
{
public:
	bool GetRobotAnswer(RobotoAnswer &ans, char* Tquestion);
	Robot();
	~Robot();
	
private:
	char* question;
	const char robotName[9] = "@Themis@";
	void CopyAnswer(const char* in,  char* out);
	void SetQuestion(char* Tquestion);
};

