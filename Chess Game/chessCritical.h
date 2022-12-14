
#include "uiInteract.h"   // for Interface
#include "uiDraw.h"       // for draw*
#include <set>            // for STD::SET
#include <cassert>        // for ASSERT
#include <fstream>        // for IFSTREAM
#include <string>         // for STRING


using namespace std;

class CriticalMethods
{
public:

/***********************************************
 * Row Column
 * Simple row/column pair
 ************************************************/
struct RC
{
   int row;
   int col;
};

/****************************************************
 * IS NOT WHITE
 * Is the current location valid and the piece is either
 * black (uppercase) or space
 ***************************************************/
inline bool isNotWhite(const char* board, int row, int col)
{
   // not white if we are off the board or if we are looking at a space
   if (row < 0 || row >= 8 || col < 0 || col >= 8)
      return false;
   char piece = board[row * 8 + col];

   return piece == ' ' || (piece >= 'A' && piece <= 'Z');
}

/****************************************************
 * IS  WHITE
 * Is the current location valid and the piece is white
 ***************************************************/
inline bool isWhite(const char* board, int row, int col)
{
   // not white if we are off the board or if we are looking at a space
   if (row < 0 || row >= 8 || col < 0 || col >= 8)
      return false;
   char piece = board[row * 8 + col];

   return (piece >= 'a' && piece <= 'z');
}

/****************************************************
 * IS NOT BLACK
 * Is the current location valid and the piece is either
 * white (lowercase) or space
 ***************************************************/
inline bool isNotBlack(const char* board, int row, int col)
{
   // not white if we are off the board or if we are looking at a space
   if (row < 0 || row >= 8 || col < 0 || col >= 8)
      return false;
   char piece = board[row * 8 + col];

   return piece == ' ' || (piece >= 'a' && piece <= 'z');
}

/****************************************************
 * IS  BLACK
 * Is the current location valid and the piece is black
 ***************************************************/
inline bool isBlack(const char* board, int row, int col)
{
   // not white if we are off the board or if we are looking at a space
   if (row < 0 || row >= 8 || col < 0 || col >= 8)
      return false;
   char piece = board[row * 8 + col];

   return (piece >= 'A' && piece <= 'Z');
}
/*********************************************************
 * GET POSSIBLE MOVES
 * Determine all the possible moves for a given chess piece
 *********************************************************/
set <int> getPossibleMoves(const char* board, int location)
{
   set <int> possible;

   // return the empty set if there simply are no possible moves
   if (location < 0 || location >= 64 || board[location] == ' ')
      return possible;
   int row = location / 8;  // current location row
   int col = location % 8;  // current location column
   int r;                   // the row we are checking
   int c;                   // the column we are checking
   bool amBlack = isBlack(board, row, col);

   //
   // PAWN
   //
   if (board[location] == 'P')
   {
      c = col;
      r = row - 2;
      if (row == 6 && board[r * 8 + c] == ' ')
         possible.insert(r * 8 + c);  // forward two blank spaces
      r = row - 1;
      if (r >= 0 && board[r * 8 + c] == ' ')
         possible.insert(r * 8 + c);  // forward one black space
      c = col - 1;
      if (isWhite(board, r, c))
         possible.insert(r * 8 + c);    // attack left
      c = col + 1;
      if (isWhite(board, r, c))
         possible.insert(r * 8 + c);    // attack right
      // what about en-passant and pawn promotion
   }
   if (board[location] == 'p')
   {
      c = col;
      r = row + 2;
      if (row == 1 && board[r * 8 + c] == ' ')
         possible.insert(r * 8 + c);  // forward two blank spaces
      r = row + 1;
      if (r < 8 && board[r * 8 + c] == ' ')
         possible.insert(r * 8 + c);    // forward one blank space
      c = col - 1;
      if (isBlack(board, r, c))
         possible.insert(r * 8 + c);      // attack left
      c = col + 1;
      if (isBlack(board, r, c))
         possible.insert(r * 8 + c);      // attack right
      // what about en-passant and pawn promotion
   }

   //
   // KNIGHT
   //
   if (board[location] == 'N' || board[location] == 'n')
   {
      RC moves[8] =
      {
               {-1,  2}, { 1,  2},
      {-2,  1},                    { 2,  1},
      {-2, -1},                    { 2, -1},
               {-1, -2}, { 1, -2}
      };
      for (int i = 0; i < 8; i++)
      {
         r = row + moves[i].row;
         c = col + moves[i].col;
         if (amBlack && isNotBlack(board, r, c))
            possible.insert(r * 8 + c);
         if (!amBlack && isNotWhite(board, r, c))
            possible.insert(r * 8 + c);
      }
   }

   //
   // KING
   //
   if (board[location] == 'K' || board[location] == 'k')
   {
      RC moves[8] =
      {
         {-1,  1}, {0,  1}, {1,  1},
         {-1,  0},          {1,  0},
         {-1, -1}, {0, -1}, {1, -1}
      };
      for (int i = 0; i < 8; i++)
      {
         r = row + moves[i].row;
         c = col + moves[i].col;
         if (amBlack && isNotBlack(board, r, c))
            possible.insert(r * 8 + c);
         if (!amBlack && isNotWhite(board, r, c))
            possible.insert(r * 8 + c);
      }
      // what about castling?
   }

   //
   // QUEEN
   //
   if (board[location] == 'Q' || board[location] == 'q')
   {
      RC moves[8] =
      {
         {-1,  1}, {0,  1}, {1,  1},
         {-1,  0},          {1,  0},
         {-1, -1}, {0, -1}, {1, -1}
      };
      for (int i = 0; i < 8; i++)
      {
         r = row + moves[i].row;
         c = col + moves[i].col;
         while (r >= 0 && r < 8 && c >= 0 && c < 8 &&
            board[r * 8 + c] == ' ')
         {
            possible.insert(r * 8 + c);
            r += moves[i].row;
            c += moves[i].col;
         }
         if (amBlack && isNotBlack(board, r, c))
            possible.insert(r * 8 + c);
         if (!amBlack && isNotWhite(board, r, c))
            possible.insert(r * 8 + c);
      }
   }

   //
   // ROOK
   //
   if (board[location] == 'R' || board[location] == 'r')
   {
      RC moves[4] =
      {
                  {0,  1},
         {-1, 0},         {1, 0},
                  {0, -1}
      };
      for (int i = 0; i < 4; i++)
      {
         r = row + moves[i].row;
         c = col + moves[i].col;
         while (r >= 0 && r < 8 && c >= 0 && c < 8 &&
            board[r * 8 + c] == ' ')
         {
            possible.insert(r * 8 + c);
            r += moves[i].row;
            c += moves[i].col;
         }
         if (amBlack && isNotBlack(board, r, c))
            possible.insert(r * 8 + c);
         if (!amBlack && isNotWhite(board, r, c))
            possible.insert(r * 8 + c);
      }
   }

   //
   // BISHOP
   //
   if (board[location] == 'B' || board[location] == 'b')
   {
      RC moves[4] =
      {
         {-1,  1}, {1,  1},
         {-1, -1}, {1, -1}
      };
      for (int i = 0; i < 4; i++)
      {
         r = row + moves[i].row;
         c = col + moves[i].col;
         while (r >= 0 && r < 8 && c >= 0 && c < 8 &&
            board[r * 8 + c] == ' ')
         {
            possible.insert(r * 8 + c);
            r += moves[i].row;
            c += moves[i].col;
         }
         if (amBlack && isNotBlack(board, r, c))
            possible.insert(r * 8 + c);
         if (!amBlack && isNotWhite(board, r, c))
            possible.insert(r * 8 + c);
      }
   }

   return possible;
}


/*********************************************
 * MOVE
 * Execute one movement. Return TRUE if successful
 *********************************************/
bool move(char* board, int positionFrom, int positionTo)
{
   // do not move if a move was not indicated
   if (positionFrom == -1 || positionTo == -1)
      return false;
   assert(positionFrom >= 0 && positionFrom < 64);
   assert(positionTo >= 0 && positionTo < 64);


   // find the set of possible moves from our current location
   set <int> possiblePrevious = getPossibleMoves(board, positionFrom);

   // only move there is the suggested move is on the set of possible moves
   if (possiblePrevious.find(positionTo) != possiblePrevious.end())
   {
      board[positionTo] = board[positionFrom];
      board[positionFrom] = ' ';
      return true;
   }

   return false;

}


};