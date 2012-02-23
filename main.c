#include<stdio.h>
#include<conio2.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>

struct Aluno{
	int matricula, anoIngresso;
	char nome[50], curso[50];
};

struct Disciplina{
	int codigoDisciplina, carga;
	char nome[50];
};

struct Nota{
	int matricula, codigoDisciplina, ano, semestre, frequencia, apagado;
	float nota1, nota2, nota3, nota4;
};

struct ConsNota{
    int codDisc, semestre, frequencia;
    float n1, n2, n3, n4;
    struct ConsNota *prox;
}; typedef struct ConsNota CN;

void createMenu(char menu[][20],int tipo,int posicao,int opcao);
void closeArchive(FILE *file);
void clearLine(int line);
void printHead(char *string);
void saveAluno(struct Aluno a);
void saveDisciplina(struct Disciplina d);
void rescreverNota(struct Nota n, char * string);
void addDisciplina(void);
void addAluno(void);
void addNota(void);
void aboutUs(void);
void editNota(struct Aluno a,struct Disciplina d, struct Nota n);
void consultaDisciplina(void);
void consultaReprovados(void);
void listarAlunos(int codigoDisciplina,int semestre);
void imprime(CN *l, char *nome);
void consultaNota(void);
void imprimirAlunos(void);
void imprimirHistorico(void);

FILE *loadArchive(char *address,char *mode);
CN *alocar(CN *l, struct Nota n);
CN *findDisc(int mat);

int startMenu(void);
int countId(char *address,int option);
int choiceStruct(int option);
int validAno(int matricula);
int validHour(int codigo);
int validMatricula(void);
int validCodigo(void);
int returnSize(char *address);
int printNota(struct Aluno a, struct Disciplina d, struct Nota n);
int findMat(char *nome);

float validNota(char * string);
float calcularMedia(struct Nota n);

struct Aluno setAluno(void);
struct Aluno checkAluno(void);
struct Aluno searchAluno(int matricula);
struct Nota searchNota(int matricula, int codigoDisciplina, int ano, int semestre);
struct Nota checkNota(struct Aluno a, struct Disciplina d);
struct Nota setNota(struct Aluno a,struct Disciplina d, int ano,int semestre);
struct Disciplina checkDisciplina(void);
struct Disciplina searchDisc(char nome[50]);
struct Disciplina searchDisciplina(int codigoDisciplina);
struct Disciplina setDisciplina(void);

int main(){
	return startMenu();
}

void printHead(char *string){
	textcolor(WHITE);
	printf("\n\t\t-------------------------------------\n");
	printf("\t\t CADASTRO DE %s \n", string);
	printf("\t\t-------------------------------------\n");
}

/*Função para validar o ano de ingresso. Só sendo possível
  aceitar ano apartir de 2000.
*/
int validAno(int matricula){
	int ano;
	p1: clearLine(2);
    printHead("ALUNOS");
	printf("\t\tMatricula: %d\n\n\n", matricula);
	do{
		printf("\t\tAno de Ingresso:\n\t\t");
        scanf("%d",&ano);
        if((ano < 2000) || (ano > 2013)){
            printf("\t\tAno Invalido! -> Ano a partir de 2000");
            getch();
			goto p1;
        }
    }while((ano < 2000) || (ano > 2013));
	return ano;
}

/*Função utilizada para validar a entrada da carga horaria das disci-
  plinas. Valores: 60, 90 e 450 horas no caso de TCC.
*/
int validHour(int codigo){
	int hour;
	p1: clearLine(2);
	printHead("DISCIPLINAS");
	printf("\t\tCodigo Disciplina: %d\n\n\n", codigo);
	do{
	    printf("\t\tCarga Horaria da Disciplina:\n\t\t");
        scanf("%d",&hour);
        if((hour != 60) && (hour != 90) && (hour != 450)){
            printf("\t\tCarga Horaria Invalida! -> 60, 90 ou 450");
            getch();
			goto p1;
        }
	}while((hour != 60) && (hour != 90) && (hour != 450));
	return hour;
}

/*Função utilizada para validar a entrada da matricula, pois impede que o
  usuário digite um valor negativo ou maior do que o numero de alunos
  cadastrados.
*/
int validMatricula(){
	int matricula, ultMatricula = countId("db/aluno_db",0);
	p1: clearLine(2);
	printHead("NOTAS");
	do{
        gotoxy(1,10); clreol(); gotoxy(1,9);
        printf("\t\tDigite a matricula do aluno:\n\t\t");
        scanf("%d",&matricula);
        if(matricula < 1 || matricula > ultMatricula){
            printf("\t\tErro! Matricula Inexistente.");
            getch();
			gotoxy(1,11); clreol();
			gotoxy(1,9); clreol();
        }
	}while(matricula < 1 || matricula > ultMatricula);
	return matricula;
}

/*Função utilizada para validar a entrada de notas nas disciplinas.
  Não permite a entrada de notas negativas ou acima de 10.
*/
float validNota(char * string){
	float nota;
	p1: clearLine(16);
	gotoxy(1,16);
	textcolor(WHITE); printf("\n\t\tInforme a %s Nota: ", string);
    do{
        textcolor(YELLOW); scanf("%f", &nota);
        if((nota < 0) || (nota > 10)){
            textcolor(WHITE); printf("\t\tErro! Nota invalida.");
            getch();
            goto p1;
        }
    }while((nota < 0) || (nota > 10));
	return nota;
}

/*Função utilizada para validar a entrada do codigo da disciplina,
  pois impede que o usuário digite um valor negativo ou maior do
  que o numero de disciplinas cadastradas.
*/
int validCodigo(){
    int disciplina, ultCodigo = countId("db/disciplina_db",1);
    do{
        gotoxy(1,12); clreol(); gotoxy(1,11);
        printf("\t\tDigite o codigo da disciplina:\n\t\t");
        scanf("%d",&disciplina);
        if(disciplina < 1 || disciplina > ultCodigo){
            printf("\t\tErro! Disciplina Inexistente.");
            getch(); gotoxy(1,13); clreol();
            gotoxy(1,11); clreol();
        }
    }while(disciplina < 1 || disciplina > ultCodigo);
	return disciplina;
}

/*
	Função para Buscar um aluno de acordo com a sua matricula
*/

struct Aluno searchAluno(int matricula){
	struct Aluno a;
	FILE *file;
	int origem;
	file = loadArchive("db/aluno_db","r");
	origem = ((matricula-1) * sizeof(struct Aluno));
	fseek(file,origem,SEEK_SET);
	fread(&a,sizeof(struct Aluno),1, file);
	closeArchive(file);
	return a;
}


/*
	Função para buscar uma disciplina, de acordo com o seu codigo
*/
struct Disciplina searchDisciplina(int codigoDisciplina){
	struct Disciplina d;
	FILE *file;
	int origem;
	file = loadArchive("db/disciplina_db","r");
	origem = ((codigoDisciplina-1) * sizeof(struct Disciplina));
	fseek(file,origem,SEEK_SET);
	fread(&d,sizeof(struct Disciplina),1, file);
	closeArchive(file);
	return d;
}

/*
	Função que busca nota, atravez da matricula, codigo, ano e semestre
*/

struct Nota searchNota(int matricula, int codigoDisciplina, int ano, int semestre){
	struct Nota n;
	int size, i;
	size = countId("db/notas_db",2) - 1;
	if(size != 0){ //Se o arquivo nao estiver vazio
		int origem = 0;
		struct Nota n[size];
		FILE *file = loadArchive("db/notas_db","r");
		for(i=0;i<size;i++){
			fseek(file,origem,SEEK_SET);
			origem = origem + sizeof(struct Nota);
			fread(&n[i],sizeof(struct Nota),1,file);
			if(((matricula == n[i].matricula)&&(n[i].apagado == 0)) && ((codigoDisciplina == n[i].codigoDisciplina) && (ano == n[i].ano)) && (semestre == n[i].semestre)){
				return n[i];
			}
		}
		closeArchive(file);
		getchar();
	}
	n.semestre = 999;
	return n;
}

/*
	Função para contar a quantidade de ocorrencias de um certo tipo de estrutura
	em um arquivo que foi determinado.
*/

int countId(char *address, int option){
	int size, t;
	size = returnSize(address);
	t = choiceStruct(option);
	size = (size/t) + 1;
	return size;
}

/*
	Função para retonar o tamanho da estrutura escolhida
*/

int choiceStruct(int option){
	int size;
	if(option == 0){
		size = sizeof(struct Aluno);
	}else if(option == 1){
		size = sizeof(struct Disciplina);
	}else if(option == 2){
		size = sizeof(struct Nota);
	}
	return size;
}

/*
	Função para salvar a estrutura aluno no arquivo
*/
void saveAluno(struct Aluno a){
	FILE *file;
	file = loadArchive("db/aluno_db","ab");
	fwrite(&a, sizeof(struct Aluno), 1, file);
	closeArchive(file);
}

/*
	Função para salvar a estrutura disciplina no arquivo
*/
void saveDisciplina(struct Disciplina d){
	FILE *file;
	file = loadArchive("db/disciplina_db","ab");
	fwrite(&d, sizeof(struct Disciplina), 1, file);
	closeArchive(file);
}

/*
	Função que inicia o programa
*/
int startMenu(){
	int opcao=0, tecla, i;
	char menu[][20] = {"Cadastrar", "Consultar", "Imprimir", "Sobre", "Sair" };
	char menuCadastro[][20] = { "Alunos","Disciplinas","Notas"};
    char menuConsulta[][20] = { "Nota por Aluno","Nota por Disciplina","Reprovados" };
    char menuImprimir[][20] = { "Listar Alunos","Historico" };
	while(1){
        createMenu(menu,0,0,opcao);
		textbackground(BLACK);
		tecla = getch();
		if(tecla == 224){
			tecla = getch();
			if((tecla == 75) && (opcao > 0)){
				opcao--;
			}else if((tecla == 77) && (opcao < 4)){
				opcao++;
			}
		}
        if((tecla == 13) && (opcao == 4)){
			return 0;
        }else if((tecla == 13) && (opcao == 0)){
			createMenu(menuCadastro,1,2,opcao);
        }else if((tecla == 13)&&(opcao == 1)){
			createMenu(menuConsulta,1,13,opcao);
        }else if((tecla == 13)&&(opcao == 2)){
			createMenu(menuImprimir,1,24,opcao);
        }else if((tecla == 13) && (opcao == 3)){
			aboutUs();
		}
	}
}

/*
	Função para desenhar menus horizontais e verticais dinamicamente
*/
void createMenu(char menu[][20],int tipo,int posicao, int opcao){
	int tecla, i, opcao2=0, tamanho = sizeof(menu);
	struct Aluno a;
	struct Disciplina d;
	struct Nota n;
	char v;
	tamanho = tamanho - 1;
	if(posicao > 23){
		tamanho = tamanho - 1;
	}
	if(tipo == 1){
		while(1){
			for(i=0;i<tamanho;i++){
				if(opcao2 == i ){
					textbackground(DARKGRAY); textcolor(WHITE);
				}else{
					textbackground(YELLOW); textcolor(BLACK);
				}
				gotoxy(posicao,i+2);
				printf(" %-20s\n", menu[i]);
			}
			textbackground(BLACK);
			tecla = getch();
			if(tecla == 224){
				tecla = getch();
				if((tecla == 72) && (opcao2 > 0)) opcao2--;
				else if((tecla == 80) && (opcao2 < tamanho)) opcao2++;
			}
			if((tecla == 13) && ((opcao == 0) && (opcao2 == 0))){
				addAluno();
			}else if ((tecla == 13) && ((opcao == 0) && (opcao2 == 1))){
				addDisciplina();
			}else if ((tecla == 13) && ((opcao == 0) && (opcao2 == 2))){
				addNota();
			}else if ((tecla == 13) && ((opcao == 1) && (opcao2 == 0))){
				consultaNota();
			}else if ((tecla == 13) && ((opcao == 1) && (opcao2 == 1))){
				consultaDisciplina();
			}else if ((tecla == 13) && ((opcao == 1) && (opcao2 == 2))){
				consultaReprovados();
			}else if ((tecla == 13) && ((opcao == 2) && (opcao2 == 0))){
				imprimirAlunos();
			}else if ((tecla == 13) && ((opcao == 2) && (opcao2 == 1))){
				imprimirHistorico();
			}
			if(tecla == 27){
				 break;
			}
		}
	}else{
		system("CLS");
		for(i=0; i<5; i++){
			if(opcao == i){
				textbackground(YELLOW);
				textcolor(BLACK);
			}else{
				textbackground(DARKGRAY);
				textcolor(WHITE);
			}
			printf(" %s ", menu[i]);
		}
		printf("                                   ");
	}
}

/*
	Função para adicionar disciplina
*/
void addDisciplina(){
	struct Disciplina d;
	char v;
	int valor = 0;
	while(valor != 1){
		d = setDisciplina();
		saveDisciplina(d);
		textcolor(YELLOW); printf("\n\t\tDeseja adicionar outra disciplina? [S/N]");
		scanf("%c",&v);
		if((v == 'N')||(v == 'n')){
			valor = 1;
		}
	}
	textcolor(WHITE);
	clearLine(2);
}

/*
	Função para adicionar aluno
*/
void addAluno(){
	struct Aluno a;
	char v;
	int valor = 0;
	while(valor != 1){
		a = setAluno();
		saveAluno(a);
		textcolor(YELLOW); printf("\n\t\tDeseja adicionar outro aluno? [S/N]\n\t\t:: ");
		scanf("%c",&v);
		if((v == 'N')||(v == 'n')){
			valor = 1;
		}
	}
	textcolor(WHITE);
	clearLine(2);
}

/*
	Função para limpar a tela
*/
void clearLine(int line){
	int i;
	for(i=line; i<25; i++){
		gotoxy(1,i);
		clreol();
		textbackground(BLACK);
		printf("                                          ");
	}
	gotoxy(1,3);
}

/*
	Função para setar as informações em uma estrutura aluno
*/
struct Aluno setAluno(){
	struct Aluno a;
	a.matricula = countId("db/aluno_db",0);
	a.anoIngresso = validAno(a.matricula);
	getchar();
	printf("\t\tNome do Aluno:\n\t\t");
	gets(a.nome);
	printf("\t\tCurso:\n\t\t");
	gets(a.curso);
	return a;
}

/*
	Função para setar as informações em uma estrutura disciplina
*/
struct Disciplina setDisciplina(){
	struct Disciplina d;
	d.codigoDisciplina = countId("db/disciplina_db",1);
	d.carga = validHour(d.codigoDisciplina);
	getchar();
	printf("\t\tNome da Discilina:\n\t\t");
	gets(d.nome);
	return d;
}

/*
	Função para determinar o tamanho de um arquivo
*/
int returnSize(char *address){
	int size;
	FILE *file = loadArchive(address,"r");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	closeArchive(file);
	return size;
}

/*
	Função para verificar se o aluno existe ou não
*/
struct Aluno checkAluno(){
	struct Aluno a;
	int matricula;
	char valor;
	while(1){
		matricula = validMatricula();
		a = searchAluno(matricula);
		textcolor(YELLOW);
		printf("\n\t\t[ %s ]", a.nome);
		getchar();
		textcolor(WHITE);
		printf("\n\t\tEste e o aluno solicitado? [S/N]");
		scanf("%c", &valor);
		clreol();
		if((valor == 'S')||(valor == 's')){
            clearLine(11);
			return a;
		}
	}
}

/*
	Função para verificar se a disciplina existe ou não
*/
struct Disciplina checkDisciplina(){
	struct Disciplina d;
	int codigo;
	char valor;
	while(1){
		codigo = validCodigo();
		d = searchDisciplina(codigo);
		textcolor(YELLOW);
		printf("\n\t\t[ %s ]",d.nome);
		getchar();
		textcolor(WHITE);
		printf("\n\t\tEsta e a Disciplina solicitada? [S/N] ");
		scanf("%c", &valor);
		if((valor == 'S') || (valor == 's')){
            clearLine(13);
			return d;
		}
	}
}

/*
	Função para setar os valores de uma estrutura Nota
*/
struct Nota setNota(struct Aluno a, struct Disciplina d, int ano, int semestre){
	struct Nota n;
	n.ano = ano;
	n.semestre = semestre;
	n.matricula = a.matricula;
	n.codigoDisciplina = d.codigoDisciplina;
	n.apagado = 0;
	clearLine(7);
	gotoxy(1,7);
	textcolor(WHITE); printf("\n\t\tAno: ");
	textcolor(YELLOW); printf("%d", n.ano);
	textcolor(WHITE); printf("\n\t\tAluno: ");
	textcolor(YELLOW); printf("%s\n", a.nome);
	textcolor(WHITE); printf("\n\t\tDisciplina: ");
	textcolor(YELLOW); printf("%s\n\n", d.nome);
	textcolor(WHITE); printf("\n\t\tInforme a Frequencia: ");
	textcolor(YELLOW); scanf("%d", &n.frequencia);
	n.nota1 = validNota("Primeira");
	n.nota2 = validNota("Segunda");
	n.nota3 = validNota("Terceira");
	float media = calcularMedia(n);
	if(media < 7){
		n.nota4 = validNota("Quarta");
	}
	return n;
}

/*
	Função para salvar a estrutura Nota no arquivo
*/
void saveNota(struct Nota n){
	FILE *file = loadArchive("db/notas_db","ab");
	fwrite(&n, sizeof(struct Nota), 1, file);
	closeArchive(file);
}

/*
	Função para imprimir na tela os valores de uma determinada nota
	e retornando uma açao a qual se deseja aplicar nela
*/
int printNota(struct Aluno a, struct Disciplina d, struct Nota n){
	int opcao;
	clearLine(7);
	gotoxy(1,7);
	textcolor(WHITE); printf("\n\t\tNome: ");
    textcolor(YELLOW); printf("%s ",a.nome);
    textcolor(WHITE); printf("- Matricula: ");
    textcolor(YELLOW); printf("%d\n",a.matricula);
    textcolor(WHITE); printf("\t\tDisciplina: ");
    textcolor(YELLOW); printf("%s ",d.nome);
    textcolor(WHITE); printf("- Codigo: ");
    textcolor(YELLOW); printf("%d\n",d.codigoDisciplina);
    textcolor(WHITE); printf("\t\tAno: ");
    textcolor(YELLOW); printf("%d ",n.ano);
    textcolor(WHITE); printf("- Semestre: ");
    textcolor(YELLOW); printf("%d ",n.semestre);
    textcolor(WHITE); printf("- Frequencia: ");
    textcolor(YELLOW); printf("%d\n",n.frequencia);
    textcolor(WHITE); printf("\t\tPrimeira Nota: ");
    textcolor(YELLOW); printf("%f ",n.nota1);
    textcolor(WHITE); printf("- Segunda Nota: ");
    textcolor(YELLOW); printf("%f\n",n.nota2);
    textcolor(WHITE); printf("\t\tTerceira Nota: ");
    textcolor(YELLOW); printf("%f ",n.nota3);
    textcolor(WHITE); printf("- Quarta Nota: ");
    textcolor(YELLOW); printf("%f",n.nota4);
	printf("\n\n\t\t1) Apagar nota\n\t\t2) Editar Nota\n\t\t3) Sair\n\n\t\t:: ",n.apagado);
	scanf("%d",&opcao);
	clearLine(2);
	return opcao;
}

/*
	Função para sobrescrever uma nota ja existente no arquivo
*/
void rescreverNota(struct Nota n, char * string){
	int size, i, origem = 0;
	size = countId("db/notas_db",2) - 1;
	struct Nota a[size];
	FILE *file = loadArchive("db/notas_db","r");
	for(i=0; i<size; i++){
		fseek(file,origem,SEEK_SET);
		origem = origem + sizeof(struct Nota);
		fread(&a[i],sizeof(struct Nota),1,file);
		if(((n.matricula == a[i].matricula) && (n.semestre == a[i].semestre)) && ((n.codigoDisciplina == a[i].codigoDisciplina) && (n.ano == a[i].ano))){
			closeArchive(file);
			file = loadArchive("db/notas_db","wb");
			origem = origem - sizeof(struct Nota);
			fseek(file,origem,SEEK_SET);
			fwrite(&n, sizeof(struct Nota), 1, file);
			textcolor(YELLOW); printf("\n\t\tNota %s com Sucesso!\n\t\t", string);
			textcolor(BLACK); system("pause");
		}
	}
	textcolor(WHITE);
	closeArchive(file);
}

/*
	Função para imprimir valores de uma ocorrencia de Nota,
	tambem escolhendo uma opcao para uma determinada açao sobre essa nota
*/
int headEditNota(struct Aluno a,struct Disciplina d, struct Nota n){
	int opcao;
	textcolor(WHITE); printf("\n\t\tAluno: ");
	textcolor(YELLOW); printf("%s",a.nome);
	textcolor(WHITE); printf("\n\t\tDisciplina: ");
	textcolor(YELLOW); printf("%s",d.nome);
	textcolor(WHITE); printf("\n\t\tFrequencia: ");
	textcolor(YELLOW); printf("%d",n.frequencia);
	textcolor(WHITE); printf("\n\t\tNota 1: ");
	textcolor(YELLOW); printf("%f - ",n.nota1);
	textcolor(WHITE); printf("Nota 2: ");
	textcolor(YELLOW); printf("%f",n.nota2);
	textcolor(WHITE); printf("\n\t\tNota 3: ");
	textcolor(YELLOW); printf("%f - ",n.nota3);
	textcolor(WHITE); printf("Nota 4: ");
	textcolor(LIGHTRED); printf("%f",n.nota4);
	textcolor(WHITE); printf("\n\t__________________________________________________________");
	printf("\n\n\t\tEditar:\n\n\t\t1) Nota 1\t2) Nota 2\n\t\t3) Nota 3\t4) Nota 4\n\t\t5) Frequencia\t6) Salvar\n\n\t\t:: ");
	scanf("%d", &opcao);
	return opcao;
}

/*
	Função para editar Nota
*/
void editNota(struct Aluno a, struct Disciplina d, struct Nota n){
	clearLine(7);
	int opcao;
	while(opcao != 6){
		opcao = headEditNota(a,d,n);
		if(opcao == 1){
			n.nota1 = validNota("Primeira");
		}else if(opcao == 2){
			n.nota2 = validNota("Segunda");
		}else if(opcao == 3){
			n.nota3 = validNota("Terceira");
		}else if(opcao == 4){
			n.nota4 = validNota("Quarta");
		}else if(opcao == 5){
			printf("\t\tDigite a nova frequencia: ");
			scanf("%d",&n.frequencia);
		}
		clearLine(2);
	}
	rescreverNota(n,"editada");
}

/*
	Função para verificar se a nota existe ou não
*/
struct Nota checkNota(struct Aluno a, struct Disciplina d){
	struct Nota n;
	int ano, opcao, semestre;
	gotoxy(1,13);
	printf("\n\t\tInforme o Ano: ");
	scanf("%d",&ano);
	printf("\n\t\tInforme o Semestre: ");
	scanf("%d",&semestre);
	n = searchNota(a.matricula,d.codigoDisciplina, ano, semestre);
	if(n.semestre == 999){
		n = setNota(a,d,ano,semestre);
		saveNota(n);
	}else{
		while(opcao != 3){
			opcao = printNota(a,d,n);
			if(opcao == 1){
				n.apagado = 1;
				rescreverNota(n,"excluida");
			}else if(opcao == 2){
				editNota(a,d,n);
				opcao = 3;
			}else if(opcao == 3){
				clearLine(2);
			}
		}
	}
	return n;
}

//Função para contar tempo.
void wait(int segundos){
  clock_t endwait;
  endwait = clock() + segundos * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

//Função que mostra informações da dupla.
void aboutUs(){
    int n;
	printf("\n\n\t\tC O M P O N E N T E S:\n\n");
	textcolor(YELLOW);
	printf("\t\t- Getulio Paulino");
	printf("\n\t\t- Jucieny Dantas\n\n");
    for(n=5; n>0; n--){
        gotoxy(40,9);printf("%d\n",n);
        wait(1);
    }
}

/*
	Função para adicionar uma nota
*/
void addNota(){
	struct Nota n;
	struct Aluno a;
	struct Disciplina d;
	int valor = 0;
	char v;
	while(valor != 1){
		a = checkAluno();
		d = checkDisciplina();
		n = checkNota(a,d);
		clearLine(2);
		getchar();
		printf("\n\t\tDeseja adicionar uma nova nota? [S/N]\n\t\t:: ");
		scanf("%c",&v);
		if((v == 'N') || (v == 'n')){
			clearLine(2);
			valor = 1;
		}
	}
}

//Função utilizada para abrir arquivos.
FILE *loadArchive(char *address,char *mode){
	FILE *file = fopen(address, mode);
	if(!file){
		clearLine(2);
		printf("\n\n\t\tErro! Problema na abertura do arquivo\n\n");
	}
	return file;
}

//Função utilizada para fechar arquivos.
void closeArchive(FILE *file){
	if(fclose(file)){
		clearLine(2);
		printf("\n\n\t\tErro! Problema para fechar o arquivo\n\n");
	}
}

//Função para achar a matricula do aluno pelo nome recebido.
int findMat(char *nome){
    struct Aluno a;
    int retorno, mat, cont=0;
    FILE *file = loadArchive("db/aluno_db","rb");
    retorno = fread(&a, sizeof(struct Aluno), 1, file);
    while(retorno){
        if(!strcasecmp(a.nome,nome)){
            mat = a.matricula;
            cont++;
        }
        retorno = fread(&a, sizeof(struct Aluno), 1, file);
    }
    if(!cont){
        printf("\n\nNome n%co cadastrado!", 198);
        return 0;
    }
    closeArchive(file);
    return mat;
}

/*Função que cria estrutura de lista encadeada para armazenar
  as informações recebidas:
  Código da disciplina, notas de 1 a 4, semestre e frequencia
*/
CN *alocar(CN *l, struct Nota n){
    CN *novo = (CN *) malloc(sizeof(CN));
    novo->codDisc = n.codigoDisciplina;
    novo->n1 = n.nota1;
    novo->n2 = n.nota2;
    novo->n3 = n.nota3;
    novo->n4 = n.nota4;
    novo->semestre = n.semestre;
    novo->frequencia = n.frequencia;
    novo->prox = l;
    return novo;
}

/*Função para achar a disciplina através do numero da matricula recebido,
  para cada disciplina encontrada envia as informações para serem armaze-
  nadas na função alocar().
*/
CN *findDisc(int mat){
    int retorno;
    struct Nota n;
    CN *l;
    l = NULL;
    FILE *file = loadArchive("db/notas_db","rb");
    retorno = fread(&n, sizeof(struct Nota), 1, file);
    while(retorno){
        if((n.matricula == mat) && (n.apagado == 0)){
            l = alocar(l, n);
        }
        retorno = fread(&n, sizeof(struct Nota), 1, file);
    }
    return l;
}

/*Função que recebendo a estrutura de lista encadeada totalmente
  preenchida e o nome do aluno, mostra na tela as informações re-
  queridas.
*/
void imprime(CN *l, char *nome){
    CN *p; //variável auxiliar para percorrer a lista
    int i=1;
    struct Disciplina d;
    clearLine(2);
    textcolor(WHITE);
    textbackground(BLACK);
	textcolor(WHITE); printf("\n\tAluno: ");
	textcolor(YELLOW); printf("%s\n\n", nome);
    for (p = l; p != NULL; p = p->prox){
		d = searchDisciplina(p->codDisc);
		textcolor(WHITE); printf("\tNome Disciplina: ");
		textcolor(YELLOW); printf("%s\n",d.nome);
        textcolor(WHITE); printf("\tCodDisciplina: ");
		textcolor(YELLOW); printf("%d\n", p->codDisc);
		textcolor(WHITE); printf("\tSemestre: ");
		textcolor(YELLOW); printf("%d\n", p->semestre);
		textcolor(WHITE); printf("\tFrequencia: ");
		textcolor(YELLOW); printf("%d\n", p->frequencia);
        textcolor(WHITE); printf("\tNota 1: ");
		textcolor(YELLOW); printf("%.1f\n", p->n1);
        textcolor(WHITE); printf("\tNota 2: ");
		textcolor(YELLOW); printf("%.1f\n", p->n2);
        textcolor(WHITE); printf("\tNota 3: ");
		textcolor(YELLOW); printf("%.1f\n", p->n3);
        textcolor(WHITE); printf("\tNota 4: ");
		textcolor(YELLOW); printf("%.1f\n\n", p->n4);
        getch();
        i++;
    }
    getch();
}

/*Função que pergunta ao usuário o nome do aluno para consultar
  suas notas. Utilizando assim as funções:
  findMat(), findDisc(), Imprime() e clearLine().
*/
void consultaNota(){
    CN *l;
    char nome[50];
    int mat;
    textcolor(WHITE);
	clearLine(2);
    textcolor(WHITE); printf("\n\n\n\t\tNome do aluno: ");
	textcolor(YELLOW); fflush(stdin); gets(nome);
	textcolor(WHITE);
    mat = findMat(nome);
    l = findDisc(mat);
    imprime(l, nome);
	clearLine(2);
}

/*
	Função para consultar as notas de uma certa disciplina
*/
void consultaDisciplina(){
	char nome[50];
	int semestre;
	struct Disciplina d;
	clearLine(2);
	textcolor(WHITE); printf("\n\n\n\t\tNome da Disciplina: ");
	textcolor(YELLOW); gets(nome);
	textcolor(WHITE); printf("\n\t\tSemestre: ");
	textcolor(YELLOW); scanf("%d",&semestre);
	getchar();
	clearLine(2);
	d = searchDisc(nome);
	textcolor(WHITE); printf("\n\t\tLISTA %s\n", d.nome);
	listarAlunos(d.codigoDisciplina,semestre);
	getchar();
	clearLine(2);
}

/*
	Função para buscar uma disciplina pelo seu nome
*/
struct Disciplina searchDisc(char nome[50]){
	int size, i, origem = 0;
	size = countId("db/disciplina_db",2) - 1;
	struct Disciplina a[size];
	FILE *file = loadArchive("db/disciplina_db","r");
	for(i=0; i<size; i++){
		fseek(file,origem,SEEK_SET);
		origem = origem + sizeof(struct Disciplina);
		fread(&a[i],sizeof(struct Disciplina),1,file);
		if(!strcasecmp(a[i].nome,nome)){
			closeArchive(file);
			return a[i];
		}
	}
	closeArchive(file);
}

/*
	Função para listar os alunos daquela determinada disciplina
*/
void listarAlunos(int codigoDisciplina,int semestre){
	int size, i, origem = 0;
	size = countId("db/notas_db",2) - 1;
	struct Nota n[size];
	struct Aluno a;
	FILE *file = loadArchive("db/notas_db","r");
	for(i=0; i<size; i++){
		fseek(file,origem,SEEK_SET);
		origem = origem + sizeof(struct Nota);
		fread(&n[i],sizeof(struct Nota),1,file);
		if((n[i].codigoDisciplina == codigoDisciplina) && (n[i].semestre == semestre)){
			a = searchAluno(n[i].matricula);
			textcolor(WHITE); printf("\n\t\tNome: ");
			textcolor(YELLOW); printf("%s",a.nome);
			textcolor(WHITE); printf("\n\t\tNota 1: ");
			textcolor(YELLOW); printf("%f - ",n[i].nota1);
			textcolor(WHITE); printf("Nota 2: ");
			textcolor(YELLOW); printf("%f",n[i].nota2);
			textcolor(WHITE); printf("\n\t\tNota 3: ");
			textcolor(YELLOW); printf("%f - ",n[i].nota3);
			textcolor(WHITE); printf("Nota 4: ");
			textcolor(YELLOW); printf("%f",n[i].nota4);
			printf("\n\t\t----------------------------------\n");
		}
	}
	closeArchive(file);
}

/*
	Função consultar todo os alunos que foram reprovados
*/
void consultaReprovados(){
	int semestre, size, i, origem=0;
	float media;
	struct Aluno a;
	clearLine(2);
	textcolor(WHITE); printf("\n\t\tSemestre: ");
	textcolor(YELLOW); scanf("%d",&semestre);
	textcolor(WHITE);
	size = countId("db/notas_db",2) - 1;
	struct Nota n[size];
	struct Disciplina d;
	FILE *file = loadArchive("db/notas_db","r");
	clearLine(2);
	printf("\n\tALUNOS REPROVADOS NO %d SEMESTRE\n", semestre);
	for(i=0; i<size; i++){
		fseek(file,origem,SEEK_SET);
		origem = origem + sizeof(struct Disciplina);
		fread(&n[i],sizeof(struct Disciplina),1,file);
		media = calcularMedia(n[i]);
		if((semestre == n[i].semestre) && (media < 7)){
			a = searchAluno(n[i].matricula);
			d = searchDisciplina(n[i].codigoDisciplina);
			textcolor(WHITE); printf("\n\t%s",d.nome);
			textcolor(LIGHTRED); printf("\n\t%0.1f", media);
			textcolor(YELLOW); printf("\t%s\n", a.nome);
		}
	}
	closeArchive(file);
	textcolor(BLACK);
	system("PAUSE");
	clearLine(2);
}

//Função para calcular a media do aluno no formato da UERN.
float calcularMedia(struct Nota n){
	float media = (((n.nota1*4)+(n.nota2*5)+(n.nota3*6))/15);
	return media;
}

/*Função que imprime em formato PDF a relação de todos os
  alunos matriculados. OBS.: Trocar "PDFCreator" pelo nome
  da impressora real para poder imprimir em papel.
*/
void imprimirAlunos(){
    FILE *file = loadArchive("db/aluno_db","rb");
	FILE *printer = fopen("\\\\127.0.0.1\\PDFCreator","wb");
	struct Aluno a;
	int retorno;
	fprintf(printer, "\n\n\n\t\t\t\t\tRELACAO GERAL DOS ALUNOS");
	fprintf(printer, "\n\n\n\t\tMatricula | Ano de Inclusao |            Nome do Aluno | Curso \n\n");
	retorno = fread(&a, sizeof(struct Aluno), 1, file);
	while(retorno){
        fprintf(printer, "\t\t       %2d |            %d | %25s| %s\n", a.matricula, a.anoIngresso, a.nome, a.curso);
		retorno = fread(&a, sizeof(struct Aluno), 1, file);
	}
	closeArchive(file); fclose(printer);
	gotoxy(20,10); textcolor(YELLOW); printf("I M P R I M I N D O...");
}

//Função que imprime em formato PDF o histórico de um aluno.
void imprimirHistorico(){
    struct Aluno a;
    struct Nota n;
    struct Disciplina d;
	int retorno, mat, cont=0, cont2=1;
	char nome[50];
    FILE *file = loadArchive("db/aluno_db","rb");
    FILE *printer = fopen("\\\\127.0.0.1\\PDFCreator","wb");
    textcolor(WHITE); printf("\n\n\n\t\tNome do aluno: ");
    textcolor(YELLOW); fflush(stdin); gets(nome);
    fprintf(printer, "\n\n\n\t\t\t\t\tH I S T O R I C O   D O   A L U N O\n\n");
    retorno = fread(&a, sizeof(struct Aluno), 1, file);
    while(retorno){
        if(!strcasecmp(a.nome,nome)){
            fprintf(printer, "\n\n\tNome do Aluno: %s", a.nome);
            fprintf(printer, "\n\n\tMatricula: %d", a.matricula); mat=a.matricula;
            fprintf(printer, "\n\n\tAno de Ingresso: %d", a.anoIngresso);
            fprintf(printer, "\n\n\tCurso: %s", a.curso);
        }
        retorno = fread(&a, sizeof(struct Aluno), 1, file);
    }
    fprintf(printer, "\n\n\n\t----------------------------------------------------------------------------------------------------\n\n\n");
    closeArchive(file); //Fecha o arquivo "db/aluno_db".
    file = loadArchive("db/notas_db","rb");
    retorno = fread(&n, sizeof(struct Nota), 1, file);
    while(retorno){
        if(n.matricula == mat){
            cont++;
            d = searchDisciplina(n.codigoDisciplina);
            fprintf(printer, "\n\n\tSemestre: %d", n.semestre);
            fprintf(printer, "\n\n\tCod.: %d  -  Disciplina: %s  -  Carga Horaria: %d", n.codigoDisciplina, d.nome, d.carga);
            fprintf(printer, "\n\n\tFrequencia: %d", n.frequencia);
            fprintf(printer, "\n\n\tNotas:");
            fprintf(printer, "\n\n\tNota 1: %.1f\tNota 2: %.1f\tNota 3: %.1f\tNota 4: %.1f", n.nota1, n.nota2, n.nota3, n.nota4);
            fprintf(printer, "\n\n");
            if(cont%5 == 0){ //Imprimir 5 registros por página
                cont2++;
                fprintf(printer, "\f\t\t\t\t\t\t\t\t\t\t\t\tPagina %d\n\n", cont2);
            }
        }
        retorno = fread(&n, sizeof(struct Nota), 1, file);
    }
    closeArchive(file); fclose(printer); //Fecha o arquivo "db/notas_db" e finaliza o ponteiro da impressora.
	gotoxy(20,10); textcolor(YELLOW); printf("I M P R I M I N D O...");
}
