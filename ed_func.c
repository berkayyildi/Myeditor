static int getch(void){
    int c = 0;
    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    return c;
}

static int kbhit(void){
    int c = 0;
    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    if (c != -1) ungetc(c, stdin);
    return ((c != -1) ? 1 : 0);
}

static int kbesc(void){
    int c;
    if (!kbhit()) return KEY_ESCAPE;
    c = getch();
    if (c == '[') {
        switch (getch()) {
            case 'A':
                c = KEY_UP;
                break;
            case 'B':
                c = KEY_DOWN;
                break;
            case 'D':
                c = KEY_LEFT;
                break;
            case 'C':
                c = KEY_RIGHT;
                break;
            default:
                c = 0;
                break;
        }
    } else {
        c = 0;
    }
    if (c == 0) while (kbhit()) getch();
    return c;
}

static int kbget(void){
	int c;
	c = getch();
	return (c == KEY_ESCAPE) ? kbesc() : c;
}

//-----------------------------------------------------
// stdio.h , string.h zaten include edildi

int cursor_line=0, cursor_col=1; // current position of the cursor
int in_use=0, free_list=1;
int satirsayisi=0;
char ourFile[100];

void insert_line(int satir_no){   //n inci elemandan sonra bi satýr aça ve oraya yazý ekler 1 ile satir sayisi arasinda bi sayi alir

	if (in_use == -1){
		in_use = 0;
		textbuffer[0].link = -1;
		free_list = 1;
		return;
	}

	int ugrasacagimizindex = in_use;
	int nth_satir=0;
	while (nth_satir < satir_no-1){
		ugrasacagimizindex = textbuffer[ugrasacagimizindex].link;
		nth_satir++;
	}
	//printf("%s den sonra %s ekle \n" , textbuffer[ugrasacagimizindex].line ,yazilacak);

	int kaybolacak = textbuffer[ugrasacagimizindex].link;
	textbuffer[ugrasacagimizindex].link = free_list;
	textbuffer[free_list].link = kaybolacak;

	strcpy(textbuffer[free_list].line, " ");	//Bos olan ilk yer yaz
	satirsayisi++;	//satir sayisini 1 arttir
	free_list++;
}

void open_file(){

	char val[80] = "";
	FILE *filepointer;
	filepointer = fopen(ourFile,"r");

	if (!filepointer){
		printf("File Not Found\n\n");
		exit(0);
	}

	while (fgets(val,80,filepointer) != NULL){
		remove_new_line(val);	//Satir okurken gelen bosluk karakterini siler
		strcpy(textbuffer[satirsayisi].line, val);
		textbuffer[satirsayisi].link = satirsayisi + 1;
		satirsayisi++;
		if (satirsayisi > 19){
			printf("\033[22;0H File Longer Than 20 Line! First 20 Line Here. \033[%d;%dH", cursor_line , cursor_col);
			break;
		}
	}
	textbuffer[satirsayisi-1].link = -1;	//Baslangicta 20. elemana -1 linki verir
	free_list = satirsayisi;		//Baslangicta free yi ayarlar
	cursor_line = satirsayisi;	//Cursor cunku hep bi alta iniyo (Burdaki +1 i kaldirdim cunku fazla onun yerine okuduktan sonra 1 satir uste goturuyorum)
	cursor_col = 1;			//En az 1 olabilir

	int i;

	for (i=satirsayisi; i<19 ; i++){
		textbuffer[i].link = i+1;
	}
	textbuffer[19].link = -1;



	if (satirsayisi == 0){	//Otomatik 1 satır ekle
		in_use = 0;
		textbuffer[1].link = -1;
		free_list = 1;
		cursor_line=1;
		cursor_col=1;
		satirsayisi=1;
		printf("\033[0;0H"); //0,0 a gotur cursoru
	}

	prinlet();		//DOSYAYI OKUDUKTAN SONRA ARRAYDAN EKRANA PRINTLE


}

void save_file(){

	FILE *fp;

	fp=fopen(ourFile, "w");

	int i = in_use;
	while (i != -1){
		fprintf(fp, "%s\n",textbuffer[i].line);
		i = textbuffer[i].link;

	}

	fclose(fp);

}

void prinlet(){

	int i = in_use;
	while (i != -1){

		//printf("%d \t %d \t %s \n",i , textbuffer[i].link,  textbuffer[i].line );	//TEST ICIN
		printf("%s \n", textbuffer[i].line );	//ORJINALI
		i = textbuffer[i].link;

	}
	/*
	 printf("\033[20;0H");      // Move cursor to 20th row, 0rd column
	 printf(" \n \nIn-Use List %d " , in_use);
	 printf(" \nFree List: %d\n " , free_list);
	*/

	printf("\033[%d;%dH", cursor_line , cursor_col);	//print ettikten sonra cursorun son kaldigi

}

void arrayprinlet(){
	int i ;
	for (i = 0 ; i< 20 ; i++){
		printf("%d , %d %s \n",i , textbuffer[i].link,  textbuffer[i].line );

	}
	/*
	printf("\033[20;0H");      // Move cursor to 20th row, 0rd column
	printf(" \n \nIn-Use List %d " , in_use);
	printf(" \nFree List: %d\n " , free_list);
	*/
}


void delete_line(int silinecek_satir){
//----------------------- | Ilk Satiri Silsin Diye | -----------------------
	int ilkmibak = in_use;	//Bu index no vericek bize looptan sonra
	int nth_line_for_first_line_control=0;	//Bu da kaçıncı satir oldugunu veriyor
	while (nth_line_for_first_line_control < silinecek_satir){
		ilkmibak = textbuffer[ilkmibak].link;
        nth_line_for_first_line_control++;
	}

	if (nth_line_for_first_line_control == 1){
		int ilk_node_un_linki = textbuffer[ilkmibak-1].link;

		printf("\033[22;0H En ust satir silindi:%d \033[%d;%dH", ilk_node_un_linki, cursor_line , cursor_col);
		strcpy(textbuffer[ilkmibak-1].line, " ");
		in_use = ilk_node_un_linki;
		textbuffer[ilkmibak-1].link = free_list;
		free_list = ilkmibak-1;
		satirsayisi--;
		return;
	}
//----------------------------------------------------------------------------

	/*
	Silineni gösteren node un linki = Silinen node un linki (TEMPE AT BUNU);
	Silinenin linki = free_list;
	freelist = silinenin indexi;
	;
	*/
	//------------------------------------------------- //Silinenin bilgileri
	int ugrasacagimizindex = in_use;
	int nth_satir=0;
	while (nth_satir < silinecek_satir - 1){
		ugrasacagimizindex = textbuffer[ugrasacagimizindex].link;
        nth_satir++;
	}
	//printf("\nSilinecek node %d  \n" , textbuffer[ugrasacagimizindex].line);
	strcpy(textbuffer[ugrasacagimizindex].line, "Silindi!");	//İçindeki veriyi yok et

	int tmp_silinen_node_un_linki = textbuffer[ugrasacagimizindex].link;
	int tmp_silinen_node_un_indexi = nth_satir;

	//-------------------------------------------------- //Silineni gösterenin node bilgileri
	int siradaki_index = in_use;	//RESETLE

	
	while (textbuffer[siradaki_index].link != tmp_silinen_node_un_linki){
		siradaki_index = textbuffer[siradaki_index].link;
	}

	int bionceyedaha = siradaki_index;	//BURDA BIR GERI DAHA GITMEK GEREKIYORMUS

	while (textbuffer[siradaki_index].link != bionceyedaha){
		siradaki_index = textbuffer[siradaki_index].link;
	}

	int silinen_nodeu_gosterenin_indexi = siradaki_index;
	int silinen_nodeu_gosterenin_linki = textbuffer[siradaki_index].link;
	//printf("\033[22;0H silinen_nodeu_gosteren_node :%s \033[%d;%dH", textbuffer[siradaki_index].line, cursor_line , cursor_col);
	//--------------------------------------------------
	
	textbuffer[tmp_silinen_node_un_indexi].link = free_list;
	free_list = tmp_silinen_node_un_indexi;
	textbuffer[silinen_nodeu_gosterenin_indexi].link = tmp_silinen_node_un_linki;

	cursor_line--;									//Cursoru degiskende bir uste al
	satirsayisi--;									//Satir sayisini 1 azalt
}


void change_char(int satir_no, int stun_no, char karakter){
	int ugrasacagimizindex = in_use;
	int nth_eleman=0;
	while (nth_eleman < satir_no-1){
		ugrasacagimizindex = textbuffer[ugrasacagimizindex].link;
        nth_eleman++;
	}


	char templine[80]="";	//\0 a gerek yok strcpy zaten koyuyor onu

	strcpy(templine, textbuffer[ugrasacagimizindex].line);

    	templine[stun_no-1] = karakter;
	strcpy(textbuffer[ugrasacagimizindex].line, templine);

}


void remove_new_line(char* string){
	size_t length;
	if( (length =strlen(string) ) >0){
		if(string[length-1] == '\n')
			string[length-1] ='\0';
	}
}

