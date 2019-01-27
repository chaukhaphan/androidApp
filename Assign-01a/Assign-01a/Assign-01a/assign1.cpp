//* FILE			: assign1.cpp
//* PROJECT			: PROG1385-19W-Sec1 Object-oriented Programming Assignment 1a
//* PROGRAMMER		: Nhung Luong 
//* FIRST VERSION	: January 11, 2018
//* DESCRIPTION		: 

#include"assessGrade.h"



int main(void)
{
	
	char input[lengthOfInput + 1] = "";
	double retCode = 0;

	// Step 1: Using a loop to assess each student's grades
	while (true)
	{
		// Prompt user enter student's assignment grades
		printf("Enter the marks or press 'X' to stop.\n");
		printf("Please specify the student's final grade or set of assignment marks >>  ");
		fgets(input, lengthOfInput, stdin);

		
		// If user want to exit the program, they enter 'X'
		retCode = exitProgram(input);
		if (retCode == EXIT)
		{
			break;
		}
		else
		{
			retCode = checkInput(input);
			if ((retCode == invalidArray) || (retCode == invalidDouble) || (retCode == invalidString))
			{
				displayErrorMessage(retCode);
			}
			else if ((retCode == incomplete) || (retCode == withdrawalAfterDrop) || (retCode == audit) ||
				(retCode == didNotAttend) || (retCode == inProcess))
			{
				displaySpecialCase(retCode);
			}
			else
			{
				displayResult(retCode);
			}
		}

		printf("\n\n");
	}
	return 0;
}