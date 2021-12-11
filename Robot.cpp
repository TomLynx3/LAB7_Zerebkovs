#include "Robot.h"




static QuestionAnswer questionNanswers[SIZE] = {
	{"Who?","I'm a Themis.Your virtual assistant.\r\nTo talk with me please add @Themis@ before any question.\r\nType \"@Themis@Help\" to get all possible questions"},
	{"How old are you?","I was born on 1 January 1970"},
	{"Help","I can answer on following questions or do following commands\r\nHow old are you?\r\nWhat is your name?\r\nChange text color\r\n"},
	{"What is your name?","My name is Themis"},
	{"Change text color","Please choose one color from following colors:\r\nBlack\r\nRed\r\nBlue\r\nAnd type @Themis@\"Color name\""},
	{"Black"},
	{"Red"},
	{"Blue"},
	{"I can't understand you :(. Please check possible questions with @Themis@Help"}};

bool Robot::GetRobotAnswer(RobotoAnswer &ans, char* Tquestion)
{
	SetQuestion(Tquestion);

	char* found;

	found = strstr(question, robotName);

	if (found) {

		char questionBuf[300];

		sscanf(question, "@Themis@%[^\n]", questionBuf);

		
		for (int i = 0; i < SIZE; i++) {
			
			QuestionAnswer item = questionNanswers[i];

			char* match = strstr(questionBuf, item.question);

			if (match) {

				if (i == 5 || i == 6 || i == 7 || i==8) {


					CopyAnswer(item.question, ans.action);
					return true;
				}
				
				CopyAnswer(item.answer, ans.answer);

				ans.haveAnswer = true;
				
				return true;
			}

		}

		ans.haveAnswer = true;
		CopyAnswer(questionNanswers[8].question, ans.answer);
		return true;
	}

	found = strstr(question, "Who?");

	if (found) {
		
		CopyAnswer(questionNanswers[0].answer,ans.answer );
		ans.haveAnswer = true;
		
		return true;
	}

	return false;
}

Robot::Robot() {



}


Robot::~Robot()
{
	delete question;
}

void Robot::CopyAnswer(const char* in,  char* out)
{
	int i = 0;

	while (in[i] != '\0') {
		out[i] = in[i];

		i++;
	}

	out[i] = '\0';
}

void Robot::SetQuestion(char* Tquestion)
{
	char* message = new char[300];

	sscanf(Tquestion, "%*[^:]%*1c%[^\n]", message);

	question = message;
}
