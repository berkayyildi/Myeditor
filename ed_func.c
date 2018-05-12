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
/*	int main(){	open_file();	return 0;	}*/

int cursor_line=0, cursor_col=1; // current position of the cursor
int in_use=0, free_list;
int satirsayisi=0;

char ourFile[100];



 void insert_line(int satir_no)   //n inci elemandan sonra bi satýr aça ve oraya yazý ekler 1 ile satir sayisi arasinda bi sayi alir
 {
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

	free_list++;
}

 void open_file(){

	char val[80] = "";
	int counter = 0;
	FILE *filepointer;
	filepointer = fopen(ourFile,"r");

	while (fgets(val,80,filepointer) != NULL){
		remove_new_line(val);	//Satir okurken gelen bosluk karakterini siler
		strcpy(textbuffer[counter].line, val);
		textbuffer[counter].link = counter + 1;
		counter++;
	}
	textbuffer[counter-1].link = -1;	//Baslangicta son elemana -1 linki verir
	free_list = counter;		//Baslangicta free yi ayarlar
	satirsayisi = counter;		//Baslangicta satir sayisini ayarlar
	cursor_line = counter;	//Cursor cunku hep bi alta iniyo (Burdaki +1 i kaldirdim cunku fazla onun yerine okuduktan sonra 1 satir uste goturuyorum)
	cursor_col = 1;			//En az 1 olabilir

	int i;

	for (i=counter+1; i< 20 ; i++){
		textbuffer[i-1].link = i+1;
	}
	textbuffer[19].link = -1;

	prinlet();		//DOSYAYI OKUDUKTAN SONRA ARRAYDAN EKRANA PRINTLE
	printf("\033[1A");	// Move cursor one up

}

void save_file(){


	FILE *fp;

	fp=fopen(ourFile, "w");

	int i = in_use;
	while (i != -1){
		fprintf(fp, "%s \n",textbuffer[i].line);
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

	silinecek_satir--;	//Yoksa bi altta siliyor
	/*
	Silineni gösteren node un linki = Silinen node un linki (TEMPE AT BUNU);
	Silinenin linki = free_list;
	freelist = silinenin indexi;
	;
	*/
	//------------------------------------------------- //Silinenin bilgileri
	int ugrasacagimizindex = in_use;
	int nth_satir=0;
	while (nth_satir < silinecek_satir){
		ugrasacagimizindex = textbuffer[ugrasacagimizindex].link;
        nth_satir++;
	}
	//printf("\nSilinecek node %d  \n" , textbuffer[ugrasacagimizindex].line);

	int tmp_silinen_node_un_linki = textbuffer[ugrasacagimizindex].link;

	int tmp_silinen_node_un_indexi = nth_satir;
	//-------------------------------------------------- //Silineni gösterenin node bilgileri
	int silinen_nodeu_gosterenin_indexi;
	int i ;
	for (i = 0 ; i< 20 ; i++){
		if (textbuffer[i].link == tmp_silinen_node_un_indexi){
                silinen_nodeu_gosterenin_indexi = i;
                break;
		}
	}
	// printf("\nSilinen node u gosteren node %s  \n" , textbuffer[silinen_nodeu_gosterenin_indexi].line);
	//silinen_nodeu_gosterenin_indexi diye bir veri geliyor burdan
	int silinen_nodeu_gosterenin_linki = textbuffer[silinen_nodeu_gosterenin_indexi].link;
	//--------------------------------------------------
	textbuffer[silinen_nodeu_gosterenin_indexi].link = tmp_silinen_node_un_linki;   //1. satırda istenen işlem yapıldı
	textbuffer[tmp_silinen_node_un_indexi].link = free_list;    //2. satırda istenen işlem yapıldı
    	free_list = tmp_silinen_node_un_indexi; //3. satırda istenen işlem yapıldı

	satirsayisi--;
}


void change_char(int satir_no, int stun_no, char karakter){
	int ugrasacagimizindex = in_use;
	int nth_eleman=0;
	while (nth_eleman < satir_no-1){
		ugrasacagimizindex = textbuffer[ugrasacagimizindex].link;
        nth_eleman++;
	}


	char templine[80] = "";
	templine[80] = '\0';

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

