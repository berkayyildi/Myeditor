#ifndef MyeditorHeader
#define MyeditorHeader

//-------------------------------------
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000a
#define KEY_UP      0x0105
#define KEY_DOWN    0x0106
#define KEY_LEFT    0x0107
#define KEY_RIGHT   0x0108

static struct termios term, oterm;

static int getch(void);
static int kbhit(void);
static int kbesc(void);
static int kbget(void);

//-------------------------------------

struct node {
	char line[80];
	int link; // a subscript in textbuffer[]
};
struct node textbuffer[20];

void open_file();
void save_file();
void insert_line(int satir_no);
void delete_line(int silinecek_satir);
void change_char(int satir_no, int stun_no, char karakter);
	
void remove_new_line(char* string);	//String okurken bosluk karakteri temizler
void prinlet();				//Terminale yazan komutumuz
void arrayprinlet();			//Linkleri gormek ve Debug icin

//-------------------------------------

#endif
