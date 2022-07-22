#include <conio.h>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#define maxNumQueen 13
#define chessBoardOffsetX 50
#define chessBoardOffsetY 5
#define rowsInCell 3
#define colInCell 5
#define coordWarningX  4
#define coordWarningY  0
#define lenStrWarning  46
#define colorGray BACKGROUND_RED |BACKGROUND_GREEN | BACKGROUND_BLUE
#define blackOnWhite BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
#define colorWhite BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE
using namespace std;


class Queue
{
private:
   struct Node
   {
      byte* combination;
      Node* next;
      Node* previous;
   };
   Node* Head;
   Node* Tail;
   Node* current;
   int cur;
public:
   Queue() :Head(nullptr), Tail(nullptr), current(nullptr), cur(0) {};
   ~Queue();
   void addTail(byte*, byte);
   byte* get(int, int);
   void removeHead();
};

void menu(HANDLE, Queue*);
int createList(Queue*, byte);
void generateCombination(byte*, byte, int&, int&, Queue*);
bool fight(byte*, byte x, byte y);
void printText(HANDLE, COORD, const char*, WORD);
void printLabelWithInput(HANDLE, COORD, const char*, int);
void showResultPlacement(HANDLE, byte*, byte, int, int);
void chessBoard(byte, HANDLE);
void gotoXY(HANDLE, COORD);
void showQueen(HANDLE, byte*, byte);
void clearPartScreen(HANDLE, COORD, byte, WORD);
int protectionInputData(HANDLE, COORD, int);
char* getDynStr(int*);
void saveComb(HANDLE, byte*, byte, int);

int main()
{
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   SetConsoleTitle(L"Задача о ферзях");
   SetConsoleTextAttribute(hStdOut, colorGray);
   system("CLS");
   SMALL_RECT windowSize = { 0,0,120 - 1,52 - 1 };
   SetConsoleWindowInfo(hStdOut, true, &windowSize);
   COORD bufferSize = { 120,52 };
   SetConsoleScreenBufferSize(hStdOut, bufferSize);
   Queue Head;
   menu(hStdOut, &Head);
   return 0;
}
//-----------------------------------------------------------------------------------------------------
Queue::~Queue()
{
   removeHead();
}
//-----------------------------------------------------------------------------------------------------
void Queue::addTail(byte* combination, byte numberQueen)
{
   if (Head && Tail)
   {
      Node* tmp;
      if (tmp = (Node*)malloc(sizeof(Node)))
      {
         tmp->combination = (byte*)calloc(numberQueen, sizeof(byte));
         for (int i = 0; i < numberQueen; i++)
            *((tmp->combination) + i) = *(combination + i + 1);
         tmp->next = nullptr;
         Tail->next = tmp;
         tmp->previous = Tail;
         Tail = tmp;
      }
   }
   else
   {
      Node* tmp;
      if (tmp = (Node*)malloc(sizeof(Node)))
      {
         tmp->combination = (byte*)calloc(numberQueen, sizeof(byte));
         for (int i = 0; i < numberQueen; i++)
            *((tmp->combination) + i) = *(combination + i + 1);
         tmp->next = nullptr;
         tmp->previous = nullptr;
         Head = tmp;
         Tail = Head;
         cur = 1;

      }
   }
}
//-----------------------------------------------------------------------------------------------------
byte* Queue::get(int selectCombination, int numberCombination)
{
   if (Head)
   {
      if (selectCombination == cur)
      {
         current = Head;
         if (current)
            return current->combination;
         else
            return nullptr;
      }

      else if (selectCombination == cur + 1)
      {
         cur++;
         current = current->next;
         if (current)
            return current->combination;
         else
            return nullptr;
      }

      else if (selectCombination == cur - 1)
      {
         cur--;
         current = current->previous;
         if (current)
            return current->combination;
         else
            return nullptr;
      }

      if (selectCombination < numberCombination / 2) //which pointer is the search element closest to
      {
         int count = 1;
         Node* temp = Head;
         while (temp)
         {
            if (count == selectCombination)
            {
               current = temp;
               return temp->combination;
            }

            count++;
            temp = temp->next;
         }
      }
      else
      {
         int count = numberCombination;
         Node* temp = Tail;
         while (temp)
         {
            if (count == selectCombination)
            {
               current = temp;
               return temp->combination;
            }

            count--;
            temp = temp->previous;
         }
      }
   }
   return nullptr;
}
//-----------------------------------------------------------------------------------------------------
void Queue::removeHead()
{
   while (Head)
   {
      Node* temp = Head->next;
      free(Head);
      Head = temp;
   }
}
//-----------------------------------------------------------------------------------------------------
void menu(HANDLE hStdOut, Queue* head)
{
   //default values
   byte numberQueen = 8, postNumberQueen = 8;
   int numberCombination = createList(head, numberQueen);
   int selectedCombination = 1;
   char ch = 1;
   bool flagExit = false;
   byte* comb = (*head).get(selectedCombination, numberCombination);

   printLabelWithInput(hStdOut, { 4,1 }, "(I)nput number of Queen: ", 8);
   printLabelWithInput(hStdOut, { 4,3 }, "(C)ombination: ", 1);
   printText(hStdOut, { 55,0 }, "Number of combination: ", colorGray);
   printText(hStdOut, { 55,2 }, "Placement: ", colorGray);
   printText(hStdOut, { 4,5 }, "(S)ave", colorGray);
   printText(hStdOut, { 4,7 }, "(E)xit", colorGray);
   showResultPlacement(hStdOut, comb, numberQueen, selectedCombination, numberCombination);

   do
   {
      ch = _getch(); //reading a key
      if (ch == 0103 || ch == 0143)//keys 'C' or 'c' for selecting a combination
      {
         if (numberCombination)
         {
            selectedCombination = protectionInputData(hStdOut, { 19,3 }, numberCombination);
            comb = (*head).get(selectedCombination, numberCombination);
            showResultPlacement(hStdOut, comb, numberQueen, selectedCombination, numberCombination);
            clearPartScreen(hStdOut, { coordWarningX,coordWarningY }, lenStrWarning, colorGray);
         }
         else
         {
            clearPartScreen(hStdOut, { 28,3 }, 11, colorGray);
            printText(hStdOut, { 28,3 }, "Impossible!", BACKGROUND_INTENSITY | BACKGROUND_RED);
            Sleep(1000);
            clearPartScreen(hStdOut, { 28,3 }, 11, colorGray);
         }
      }

      if (ch == 0111 || ch == 0151)//keys 'I' or 'i' for entering the number of queens
      {
         postNumberQueen = numberQueen;
         numberQueen = protectionInputData(hStdOut, { 29,1 }, maxNumQueen);

         if (postNumberQueen != numberQueen)
         {
            head->removeHead();
            numberCombination = createList(head, numberQueen);

            if (numberCombination)
               selectedCombination = 1;
            else
               selectedCombination = 0;

            SetConsoleCursorPosition(hStdOut, { 19, 3 });
            SetConsoleTextAttribute(hStdOut, blackOnWhite);
            cout << selectedCombination << setw(5) << " ";
            SetConsoleTextAttribute(hStdOut, colorGray);

            comb = (*head).get(selectedCombination, numberCombination);
            showResultPlacement(hStdOut, comb, numberQueen, selectedCombination, numberCombination);
         }
         clearPartScreen(hStdOut, { coordWarningX,coordWarningY }, lenStrWarning, colorGray);
      }

      if (ch == 0123 || ch == 0163)//keys 'S' or 's'
      {
         saveComb(hStdOut, comb, numberQueen, selectedCombination);
      }

      if (ch == 0105 || ch == 0145) //keys 'E' or 'e'
      {
         flagExit = true;
      }
   } while (!flagExit);
   SetConsoleTextAttribute(hStdOut, colorGray);
   system("CLS");
}
//-----------------------------------------------------------------------------------------------------
int createList(Queue* head, byte numberQueen)
{
   register int count = 0, point = 1;
   byte* arrComb;
   if (arrComb = (byte*)calloc(numberQueen, sizeof(byte)))
   {
      for (int i = 0; i <= numberQueen; i++)
      {
         *(arrComb + i) = 0;
      }
      generateCombination(arrComb, numberQueen, point, count, head);
      return count;
   }
   return 0;
}
//-----------------------------------------------------------------------------------------------------
void generateCombination(byte* timeArrComb, byte numberQueen, int& index, int& count, Queue* head)
{
   if (!index)
      return;
   else
   {
      while (1)
      {
         *(timeArrComb + index) += 1;
         if (*(timeArrComb + index) > numberQueen)  //outside the chessboard
         {
            index--;
            return;
         }

         if (fight(timeArrComb, *(timeArrComb + index), index))  //the queen is not under battle
         {
            if (index == numberQueen) //reached the end and the combination turned out
            {
               index--;
               count++;
               (*head).addTail(timeArrComb, numberQueen);
               return;
            }
            else  //move the next queen
            {
               index++;
               *(timeArrComb + index) = 0;
               generateCombination(timeArrComb, numberQueen, index, count, head);
            }
         }
      }
   }
}
//-----------------------------------------------------------------------------------------------------
bool fight(byte* arr, byte x, byte y)
{
   for (byte i = 1; i < y; i++)
   {
      if (*(arr + i) == x || abs(x - *(arr + i)) == y - i)
         return false;
   }
   return true;
}
//-----------------------------------------------------------------------------------------------------
void printText(HANDLE hStdOut, COORD pos, const char* text, WORD color)
{
   SetConsoleTextAttribute(hStdOut, color);
   SetConsoleCursorPosition(hStdOut, pos);
   cout << text;
}
//-----------------------------------------------------------------------------------------------------
void printLabelWithInput(HANDLE hStdOut, COORD pos, const char* text, int num)
{
   SetConsoleTextAttribute(hStdOut, colorGray);
   SetConsoleCursorPosition(hStdOut, pos);
   cout << text;
   SetConsoleTextAttribute(hStdOut, blackOnWhite);
   cout << num << setw(5) << " ";
   SetConsoleTextAttribute(hStdOut, colorGray);
   cout << setw(12) << " ";
}
//-----------------------------------------------------------------------------------------------------
void showResultPlacement(HANDLE hStdOut, byte* comb, byte numberQueen, int selectedСomb, int numberCombination)
{
   gotoXY(hStdOut, { 78,0 });
   cout << numberCombination << setw(4) << ' ';
   chessBoard(numberQueen, hStdOut); //draw a chessboard
   if (comb)
   {
      char a = 'a';
      gotoXY(hStdOut, { 66,2 });
      for (byte j = 0; j < numberQueen; j++) //displaying the placement
      {
         cout << a++ << (int)*(comb + j);
         if (j != numberQueen - 1)
            cout << ", ";
      }
      cout << setw((maxNumQueen - numberQueen + 1) * 4) << ' ';
      showQueen(hStdOut, comb, numberQueen); //drawing the placement of queens
   }
   else
   {
      gotoXY(hStdOut, { 66,2 });
      cout << setw((maxNumQueen) * 4 + 2) << ' ';
   }
}
//-----------------------------------------------------------------------------------------------------
void chessBoard(byte numberQueen, HANDLE hStdOut)
{
   char codeA = 'A';
   const WORD whiteOnRed = BACKGROUND_RED | FOREGROUND_INTENSITY |
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
   COORD Pos = { chessBoardOffsetX , chessBoardOffsetY };
   gotoXY(hStdOut, Pos);
   SetConsoleTextAttribute(hStdOut, colorGray);
   for (byte i = 0; i <= maxNumQueen * rowsInCell + 1; i++) //erasing the old chessboard
   {
      cout << setw(69) << ' ';
      gotoXY(hStdOut, { Pos.X,++Pos.Y });
   }
   //move to the right edge depending on the number of queens
   gotoXY(hStdOut, { Pos.X = chessBoardOffsetX + (maxNumQueen - numberQueen) * colInCell,Pos.Y = chessBoardOffsetY });
   SetConsoleTextAttribute(hStdOut, whiteOnRed);
   cout << "  ";

   for (byte i = 0; i < numberQueen; i++)//output the cell numbers at the top
   {
      cout << setw(3) << right << i + 1 << "  ";
   }
   cout << "  ";
   gotoXY(hStdOut, { Pos.X,++Pos.Y });

   for (byte i = 0; i < numberQueen; i++)
      for (byte j = 0; j < rowsInCell; j++)
      {
         if (j == 1) //in the frame print the letters
         {
            SetConsoleTextAttribute(hStdOut, whiteOnRed);
            cout << setw(2) << left << codeA++;
         }

         else
         {
            SetConsoleTextAttribute(hStdOut, whiteOnRed);
            cout << "  ";
         }

         for (byte k = 0; k < numberQueen; k++)  //draw cells with a color depending on the coordinates
         {
            if ((i + k) % 2)
            {
               SetConsoleTextAttribute(hStdOut, colorWhite);
               cout << setw(colInCell) << " ";
            }

            else
            {
               SetConsoleTextAttribute(hStdOut, BACKGROUND_RED);
               cout << setw(colInCell) << " ";
            }
         }

         SetConsoleTextAttribute(hStdOut, BACKGROUND_RED);
         cout << "  ";
         gotoXY(hStdOut, { Pos.X,++Pos.Y });
      }

   cout << setw(colInCell * numberQueen + 4) << " ";
   SetConsoleTextAttribute(hStdOut, colorGray);
}
//-----------------------------------------------------------------------------------------------------
void gotoXY(HANDLE hStdOut, COORD pos)
{
   SetConsoleCursorPosition(hStdOut, pos);
}
//-----------------------------------------------------------------------------------------------------
void showQueen(HANDLE hStdOut, byte* combination, byte numberQueen)
{
   const int column = numberQueen * colInCell, rows = numberQueen * rowsInCell;
   short offsetX = chessBoardOffsetX + (maxNumQueen - numberQueen) * colInCell - 1;
   COORD Pos = { offsetX,chessBoardOffsetY + 2 };
   gotoXY(hStdOut, Pos);

   for (byte i = 0; i < numberQueen; i++)
   {
      gotoXY(hStdOut, { Pos.X += (*(combination + i)) * colInCell,Pos.Y });
      if ((i + *(combination + i)) % 2)
         SetConsoleTextAttribute(hStdOut, BACKGROUND_RED);
      else
         SetConsoleTextAttribute(hStdOut, colorWhite);
      cout << "*";
      gotoXY(hStdOut, { --Pos.X,--Pos.Y });
      cout << "\\|/";
      gotoXY(hStdOut, { ++Pos.X,++Pos.Y });
      gotoXY(hStdOut, { Pos.X = offsetX,Pos.Y += rowsInCell });
   }
   SetConsoleTextAttribute(hStdOut, colorGray);
}
//-----------------------------------------------------------------------------------------------------
void clearPartScreen(HANDLE hStdOut, COORD pos, byte lenClear, WORD color)
{
   SetConsoleCursorPosition(hStdOut, pos);
   SetConsoleTextAttribute(hStdOut, color);
   cout << setw(lenClear) << ' ';
}


//-----------------------------------------------------------------------------------------------------
int protectionInputData(HANDLE hStdOut, COORD pos, int maxNum)
{
   int n = 0;
   int lenTmp = 0;

   COORD Pos = { coordWarningX,coordWarningY }; //erasing the warning text
   clearPartScreen(hStdOut, Pos, lenStrWarning, colorGray);
   fflush(stdin);
   clearPartScreen(hStdOut, pos, 6, blackOnWhite);
   SetConsoleCursorPosition(hStdOut, pos);

   char* tmp = getDynStr(&lenTmp);
   tmp[lenTmp] = '\0';

   while (sscanf_s(tmp, "%d", &n) != 1 || n < 1 || n > maxNum) //repeat until we get a number
   {
      clearPartScreen(hStdOut, Pos, lenStrWarning, colorGray);
      if (n < 1)
         printText(hStdOut, Pos, "\aIncorrect input or negative number or zero", BACKGROUND_INTENSITY | BACKGROUND_RED);
      else if (n > maxNum)
      {
         printText(hStdOut, Pos, "\aNumber greater than ", BACKGROUND_INTENSITY | BACKGROUND_RED);
         cout << maxNum;
         n = -1;
      }

      clearPartScreen(hStdOut, pos, 6, blackOnWhite); //erasing the input
      clearPartScreen(hStdOut, { pos.X + 6, pos.Y }, 12, colorGray);
      SetConsoleTextAttribute(hStdOut, blackOnWhite);
      SetConsoleCursorPosition(hStdOut, pos);

      tmp = getDynStr(&lenTmp);
   }
   clearPartScreen(hStdOut, Pos, lenStrWarning, colorGray); //erasing the warning text and output a positive message
   printText(hStdOut, Pos, "Successfully!", BACKGROUND_GREEN);
   SetConsoleTextAttribute(hStdOut, colorGray);
   return n;
}
//-----------------------------------------------------------------------------------------------------
char* getDynStr(int* len)
{
   char* tmpStr = NULL;
   if (tmpStr = (char*)malloc(sizeof(char)))
   {
      char ch = getchar();
      *len = 0;
      int capacity = 1;
      while (ch != '\n')
      {
         *(tmpStr + ((*len)++)) = ch;
         if (*len >= capacity) //increasing the string by 2 times
         {
            capacity <<= 1;
            tmpStr = (char*)realloc(tmpStr, capacity);
         }
         ch = getchar();
      }
   }
   return tmpStr;
}
//-----------------------------------------------------------------------------------------------------

void saveComb(HANDLE hStdOut, byte* comb, byte numberQueen, int selectedCombination)
{
   byte tmp = numberQueen;
   char nameFile[26] = "save/num_  sel_     .txt";
   for (int i = 1; i >= 0; i--)
   {
      *(nameFile + 9 + i) = 48 + tmp % 10;
      tmp /= 10;
   }

   for (int i = 4; i >= 0; i--)
   {
      *(nameFile + 15 + i) = 48 + selectedCombination % 10;
      selectedCombination /= 10;
   }
   FILE* output;

   if (!fopen_s(&output, nameFile, "w") && comb)
   {
      for (int k = 0; k < numberQueen; k++)
      {
         for (int j = 1; j <= numberQueen; j++)
            if (*(comb + k) == j)
               fprintf(output, "*");
            else
               fprintf(output, ".");
         fprintf(output, "\n");
      }
      fprintf(output, "\n\n");

      char a = 'a';
      for (byte j = 0; j < numberQueen; j++)
      {
         fprintf(output, "%c%d", a++, (int)*(comb + j));
         if (j != numberQueen - 1)
            fprintf(output, ", ");
      }
      fclose(output);

      clearPartScreen(hStdOut, { 11,5 }, 15, colorGray);
      printText(hStdOut, { 11,5 }, "Successfully!", BACKGROUND_GREEN);
      Sleep(1000);
      clearPartScreen(hStdOut, { 11,5 }, 15, colorGray);
   }
   else
   {
      clearPartScreen(hStdOut, { 11,5 }, 15, colorGray);
      printText(hStdOut, { 11,5 }, "Unsuccessfully!", BACKGROUND_INTENSITY | BACKGROUND_RED);
      Sleep(1000);
      clearPartScreen(hStdOut, { 11,5 }, 15, colorGray);
      if (output)
         fclose(output);
   }
   SetConsoleTextAttribute(hStdOut, colorGray);
}
