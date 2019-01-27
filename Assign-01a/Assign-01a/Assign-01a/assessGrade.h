#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#pragma warning (disable : 4996)


// Define
#define lengthOfInput 100
#define lengthOfExitString 1
#define lengthOfInputInLetter 5
#define lengthOfletterGrade 8
#define sizeOfElements 5
#define lengthOfSpecialCase 5

// Defines for invalid input and conditions
#define EXIT -5.00
#define PASS 0
#define FAIL -1
#define VALID 5
#define invalidDouble -5.0
#define invalidArray -6.0
#define invalidString -7.0

// Defines for grade
#define passingMark 54.50
#define minimumMark 0.0
#define maximumMark 100.0


// Defines for special case
#define incomplete -10.00
#define withdrawalAfterDrop -11.00
#define audit -12.00
#define didNotAttend -13.00
#define inProcess -14.00



// Function prototypes for other functions
double exitProgram(char *input);
double checkInput(char input[lengthOfInput]);
void replaceCharInStr(char *inputStr, char oldChar, char newChar);


// Function prototypes for three overloaded functions
double assessGrade(double finalMark);
double assessGrade(int assignMark1, int assignMark2, int assignMark3, int assignMarks4, int assignMark5);
double assessGrade(char inputInLetter[lengthOfInputInLetter]);

// Function prototypse for display output
void displayResult(double retCondition);
void displayErrorMessage(double retCode);
void displaySpecialCase(double retCode);

