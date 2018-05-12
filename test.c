#include "input.h"


int temizle(void){	//Heryeri ekranda boslukla doldurur
	
	int i,j;
	for(i=0 ; i < 20 ; i++){
		for (j=0 ; j < 80 ; j++){
			printf("\033[%d;%dH",i,j);
			printf(" ");
		}
	}

	printf("\033[0;0H");	//Basa al cursoru
	prinlet();	//TEMIZLEDIKTEN SONRA VERILERI ARRAYDEN RELOAD
}

int main(void)
{
	int c;
	readFile();

	while (1) {
		c = kbget();        // Get input
		
		if (c == '+') {	// Satir eklemesi icin
			arayaekle(cursor_line);
			temizle();
			printf("\033[%d;%dH",cursor_line+1,0);	//Cursoru goruntude bir alta al
			cursor_line++;				//Cursoru degiskende bir alta al
			cursor_col=0;
		} else
		if (c == '-') {	// Satir Silmesi icin
			satirSil(cursor_line);
			temizle();
			printf("\033[%d;%dH",cursor_line-1,cursor_col);	//Cursoru goruntude bir uste al
			cursor_line--;				//Cursoru degiskende bir uste al
		
		} else
		if (c == '*') {	// DEBUG ICIN
			printf("\033[21;0H Line: %d Col: %d \033[%d;%dH",cursor_line , cursor_col , cursor_line , cursor_col);
			printf("\033[22;0H In-Use: %d Free: %d \033[%d;%dH",in_use , free_list , cursor_line , cursor_col);
			printf("\033[22;0H");
			arrayprinlet();
			printf("\033[%d;%dH", cursor_line , cursor_col);
		} else
		
		if (c == '@') {	// Dosya kaydetmesi icin
			printf("\033[21;0H");      // Move cursor to 20th row, 0rd column
			dosyaKaydet();
			printf("[FILE SAVED]");
			break;
		} else
		if (c == KEY_ENTER || c == KEY_ESCAPE) {
			break;
		} else
		if (c == KEY_LEFT) {			//CURSOR UN SAGA GITMESI AZALTIYOR
			if (cursor_col > 1){
			    printf("\033[1D");		// Move cursor one left
			    cursor_col--;
			}
		} else
		if (c == KEY_RIGHT) {
			if (cursor_col < strlen(textbuffer[cursor_line-1].line)){	//stun sayisindan daha fazla ileri gitmeye izin verme

		    	    printf("\033[1C");	// Move cursor one right
		    	    cursor_col++;
			}
		} else
		if (c == KEY_UP){
			if (cursor_line > 1){	//Yukarı gidilebilicek satir var ise
				int ust_satirin_uzunlugu = strlen(textbuffer[cursor_line-2].line);	//Ust satirin uzunlugu
				if (ust_satirin_uzunlugu < cursor_col)
					printf("\033[%d;%dH", cursor_line-1 , ust_satirin_uzunlugu);	// Move cursor one up and move col last char of up string
				else{
					printf("\033[1A");	// Move cursor one up
				}
				cursor_line--;
			}
		}else
		if (c == KEY_DOWN){
			if (cursor_line < satirsayisi){	//Asagi inilebilecek satir var ise
				int ust_satirin_uzunlugu = strlen(textbuffer[cursor_line].line);	//Asagiki satirin uzunlugu
				if (ust_satirin_uzunlugu < cursor_col){
					printf("\033[%d;%dH", cursor_line+1 , ust_satirin_uzunlugu);	// Move cursor one down and move col last char of up string
				}else{
				    printf("\033[1B");	// Move cursor one down
				}
				cursor_line++;
			}

		} else {
			putchar(c);
			karakterdegistir(cursor_line, cursor_col, c);
			cursor_col++;
		}
    	}	//WHILE KAPAT
    printf("\n");

    return 0;
}	//MAIN KAPAT
