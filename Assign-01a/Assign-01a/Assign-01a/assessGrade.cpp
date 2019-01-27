/* ====================================================================*/
/* PURPOSE : This file contains all of the various                     */
/*           functions used to calculate the grade of each student     */
/* ====================================================================*/


#include"assessGrade.h"





/*========================================================================*/
/* FUNCTION		: replaceCharInStr                                        */
/* DESCRIPTION	: This function is used to convert oldchar in to newchar  */
/* PARAMETERS	: char *inputStr : the input array                        */
/*				: char oldChar : a char that we want to be replaced       */
/*				: char newchar : a char that we want to replaced          */
/* RETURNS		: N/A                                                     */
/*========================================================================*/
void replaceCharInStr(char *inputStr, char oldChar, char newChar)
{
	char *pChar = NULL;
	pChar = strchr(inputStr, oldChar);

	while (pChar != NULL)
	{
		*pChar = newChar;
		pChar = strchr(inputStr, oldChar);
	}
}






/* =======================================================================*/
/* FUNCTION : checkInput(finalMark)                                       */
/* PURPOSE  : This function is used to check input and parse input into   */
/*            three accessGrade functions                                 */
/* INPUTS   : input: input string entered                                 */
/* RETURNS  : N/A                                                         */
/* =======================================================================*/
double checkInput(char input[lengthOfInput])
{
	int i = 0;
	char period = '.';
	char blankSpace = ' ';
	double finalMark = 0;
	int assignMark1 = 0;
	int assignMark2 = 0;
	int assignMark3 = 0;
	int assignMark4 = 0;
	int assignMark5 = 0;

	replaceCharInStr(input, '\n', '\0');


	// There are three cases of input:
	//	 1. In case of entering a double, the double mark contains a period between numbers,
	//		then we just convert this mark entered into double.
	//   2. In case of entering five assignment marks, there are five int numbers in the input, 
	//      then we can get five marks
	//	 3. The rest case would be that users enter letter
	if (strchr(input, period) != NULL)
	{
		finalMark = atof(input);
		{
			finalMark = assessGrade(finalMark);
		}
		
	}
	else if (sscanf(input, "%d %d %d %d %d", &assignMark1, &assignMark2, &assignMark3, &assignMark4, &assignMark5))
	{
		finalMark = assessGrade(assignMark1, assignMark2, assignMark3, assignMark4, assignMark5);
		
	}
	else
	{
		finalMark = assessGrade(input);
	}
	
	return finalMark;
}









/* =======================================================================*/
/* FUNCTION : assessgrade(finalMark)                                      */
/* PURPOSE  : This function takes double for final mark of a student to   */
/*            check that is fails or pass                                 */
/* INPUTS   : finalMark : the final mark of student                       */
/* RETURNS  : retcode                                                     */
/* =======================================================================*/
double assessGrade(double finalMark)
{
	double retCode = 0;    

	if (finalMark >= minimumMark && finalMark <= maximumMark)
	{
		retCode = finalMark;
	}
	else
	{
		retCode = invalidDouble;
	}
	
	
	return retCode;
}






/* =========================================================================================*/
/* FUNCTION : assessgrade(assignMark1, assignMark2, assignMark3, assignMarks4, assignMark5) */
/* PURPOSE  : This function takes five interger parameters as assignment marks,             */
/*            then calculate the final marks                                                */
/* INPUTS   : assignMark1 : the first assignment's mark of student                          */
/*            assignMark2 : the second assignment's mark of student                         */
/*            assignMark3 : the third assignment's mark of student                          */
/*            assignMark4 : the fourth assignment's mark of student                         */
/*            assignMark5 : the fifth assignment's mark of student                          */
/* RETURNS  : retcode                                                                       */
/* =========================================================================================*/
double assessGrade(int assignMark1, int assignMark2, int assignMark3, int assignMark4, int assignMark5)
{
	double averageMark = 0;
	int sum = 0;
	double retCode = 0;

	if ((assignMark1 < minimumMark || assignMark1 > maximumMark)
		|| (assignMark2 < minimumMark || assignMark2 > maximumMark)
		|| (assignMark3 < minimumMark || assignMark3 > maximumMark)
		|| (assignMark4 < minimumMark || assignMark4 > maximumMark)
		|| (assignMark5 < minimumMark || assignMark5 > maximumMark))
	{
		retCode = invalidArray;
	}
	else
	{
		sum = assignMark1 + assignMark2 + assignMark3 + assignMark4 + assignMark5;
		averageMark = (double)(sum / 5);
		retCode = averageMark;
	}
	

	return retCode;
}





/* =======================================================================*/
/* FUNCTION : assessgrade(finalMark)                                      */
/* PURPOSE  : This function takes double for final mark of a student to   */
/*            check that is fails or pass                                 */
/* INPUTS   : finalMark : the final mark of student                       */
/* RETURNS  : retcode                                                     */
/* =======================================================================*/
double assessGrade(char inputInLetter[lengthOfInputInLetter])
{
	char letterGrade[lengthOfletterGrade + 1][sizeOfElements + 1] = { "A+", "A", "B+", "B", "C+", "C", "D", "F" }; // Array of letter grade
	char specialCase[lengthOfSpecialCase + 1][sizeOfElements + 1] = { "I", "Q", "AU", "DNA", "I/P" };              // Array of special cases

	double equivalentMark[lengthOfletterGrade + 1] = { 95.00,85.00,77.00,72.00,67.00,62.00,57.00,50.00 };          // Array of final marks
	double specialCaseInNum[lengthOfSpecialCase] = {incomplete,withdrawalAfterDrop,audit,didNotAttend,inProcess};  // Array of special case

	int i = 0;
	int j = 0;
	double retCode = 0;


	// Compare input with two demetional array of string ( letterGrade)
	//    1. If input is similar with one of the elements of first array, this would be valid input
	//       , then retCode would equal numberical equivalents
	//    2. If input is similar with one of the element of second array, this would be also valid input and
	//       retCode would equal specialCaseInNum
	//    3. If input is not similar with one of the elements of array, this would be invalid input
	//       , then retCode would equal invalidString
	for (i = 0; i < lengthOfletterGrade; i++)
	{
		if (strcmp(inputInLetter, letterGrade[i]) == 0)
		{
			retCode = equivalentMark[i];
			break;
		}
		else
		{
			
			for (j = 0; j <= lengthOfSpecialCase; j++)
			{
				if (strcmp(inputInLetter, specialCase[i]) == 0)
				{
					retCode = specialCaseInNum[i];
					break;
				}
				
				else
				{
					retCode = invalidString;
				}
			}
		}
	}
	
	return retCode;
}




/* ===============================================================================*/
/* FUNCTION : displayResult                                                       */
/* PURPOSE  : This function is used to print the result of the student            */
/* INPUTS   : double retCode : the return condition from the assessGrade function */
/* RETURNS  : N/A                                                                 */
/* ============================================================================== */

void displayResult(double retCode)
{
	if (retCode >= passingMark && retCode <= maximumMark)
	{
		printf("Student achieved %.2lf %% which is a PASS condition.\n", retCode);
	}
	else if (retCode >= minimumMark && retCode < passingMark)
	{
		printf("Student achieved %.2lf %% which is a FAIL condition.\n", retCode);
	}
}





/* ===============================================================================*/
/* FUNCTION : displayErrorMessage                                                 */
/* PURPOSE  : This function is used to display error message                      */
/* INPUTS   : double retCode : the return condition from the assessGrade function */
/* RETURNS  : N/A                                                                 */
/* ============================================================================== */
void displayErrorMessage(double retCode)
{
	if (retCode == invalidArray)
	{
		printf("User specified a INVALID \"array\".\n");
	}
	else if (retCode == invalidDouble)
	{
		printf("User specified a INVALID \"double\" type grade\n");
	}
	else if (retCode == invalidString)
	{
		printf("User specified a INVALID \"string\" type grade.\n");
	}
}




/* ===============================================================================*/
/* FUNCTION : displayResult                                                       */
/* PURPOSE  : This function is used to print the result of the student            */
/* INPUTS   : double retCode : the return condition from the assessGrade function */
/* RETURNS  : N/A                                                                 */
/* ============================================================================== */
void displaySpecialCase(double retCondition)
{
	if (retCondition == incomplete)
	{
		printf("Student has Special Situation : I (Incomplete Condition)\n");
	}
	else if (retCondition == withdrawalAfterDrop)
	{
		printf("Student has Special Situation : Q (Withdrawal after drop/refund date Condition)\n");
	}
	else if (retCondition == audit)
	{
		printf("Student has Special Situation : AU (Audit Condition)\n");
	}
	else if (retCondition == didNotAttend)
	{
		printf("Student has Special Situation : DNA (Did Not Attend Condition)\n");
	}
	else if (retCondition == inProcess)
	{
		printf("Student has Special Situation : IP (In Process Condition)\n");
	}
}




/* ===============================================================================*/
/* FUNCTION : exitProgram                                                         */
/* PURPOSE  : This function is used to exitProgram                                */
/* INPUTS   : double retCode : the return condition from the assessGrade function */
/* RETURNS  : retCode                                                             */
/* ============================================================================== */
double exitProgram(char *input)
{
	double retCode = 0;
	char exitString[lengthOfExitString + 1] = "X";

	if (strncmp(exitString, input, 1) == 0)
	{
		retCode = EXIT;
	}
	return retCode;
}


