#include "ed_hdr.h"
#include "ed_func.c"

int temizle(void){	//Heryeri ekranda boslukla doldurur
	printf("\033[0;0H");	//Basa al cursoru
	int i,j;

	for(i=0 ; i < 21 ; i++){
		for (j=0 ; j < 80 ; j++){
			printf("\033[%d;%dH",i,j);
			printf(" ");
		}
		printf("|");
	}
	printf("\n--------------------------------------------------------------------------------");
	printf("\033[0;0H");	//Basa al cursoru

}

int main(int argc, char *argv[]){

	strcpy (ourFile, argv[1]);	//Ilk parametreyi okunacakdosya olarak belirle
	temizle();
	open_file();

	int c;

	while (1) {
		c = kbget();        // Get input
		
		if (c == '+') {	// Satir eklemesi icin
			if(satirsayisi < 20){
				insert_line(cursor_line);
				temizle();
				cursor_line++;				//Cursoru degiskende bir alta al
				cursor_col=1;
				prinlet();
				
			}
		} else
		if (c == '-') {	// Satir Silmesi icin
			if(satirsayisi > 0){
				delete_line(cursor_line);
				temizle();
				prinlet();
			}
		
		} else
		if (c == '*') {	// DEBUG ICIN
			printf("\033[23;0H -- Line: %d | Col: %d | (In-Use: %d | Free: %d) | Num of Lines: %d -- \033[%d;%dH",cursor_line , cursor_col , in_use , free_list, satirsayisi , cursor_line , cursor_col);
			printf("\033[24;0H");
			arrayprinlet();
			printf("\033[%d;%dH", cursor_line , cursor_col);
		} else
		
		if (c == '@') {	// Dosya kaydetmesi icin
			printf("\033[21;0H");      // Move cursor to 20th row, 0rd column
			save_file();
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
			if (cursor_col < strlen(textbuffer[cursor_line-1].line)+1){	//stun sayisindan daha fazla ileri gitmeye izin verme (Bu kontrol dinamik değil :( )
		    	    printf("\033[1C");	// Move cursor one right
		    	    cursor_col++;
			}
		} else
		if (c == KEY_UP){
			if (cursor_line > 1){	//Yukarı gidilebilicek satir var ise
				int ust_satirin_uzunlugu = strlen(textbuffer[cursor_line-2].line) +1 ;	//Ust satirin uzunlugu + 1 fazlasına git
				if (ust_satirin_uzunlugu < cursor_col){
					cursor_col = ust_satirin_uzunlugu;
					printf("\033[%d;%dH", cursor_line-1 , cursor_col);	// Move cursor one up and move col last char of up string
					
				}else{
					printf("\033[1A");	// Move cursor one up
				}
				cursor_line--;
				
			}
		}else
		if (c == KEY_DOWN){
			if (cursor_line < satirsayisi){	//Asagi inilebilecek satir var ise
				int alt_satirin_uzunlugu = strlen(textbuffer[cursor_line].line) +1;	//Asagiki satirin uzunlugu + 1 fazlasına git
				if (alt_satirin_uzunlugu < cursor_col){
					cursor_col = alt_satirin_uzunlugu;
					printf("\033[%d;%dH", cursor_line+1 , cursor_col);	// Move cursor one down and move col last char of up string
				}else{
				    printf("\033[1B");	// Move cursor one down
				}
				cursor_line++;
				
			}

		} else {
			if (cursor_col < 78){
				putchar(c);
				change_char(cursor_line, cursor_col, c);
				cursor_col++;
			}
		}
    	}	//WHILE KAPAT
    printf("\n");

    return 0;
}	//MAIN KAPAT
