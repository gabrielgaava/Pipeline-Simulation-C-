#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fstream>
using namespace std;

//Define clear e cls para funcionar tanto em windows como em unix
#ifdef _unix_
#define clear "clear"
#else
#define clear "cls"
#endif

//Registradores
int ebp, esp, eax, edx;

//Outros auxiliares
int clock1=0, processoAtual=0;

//Arquivos Disponiveis
char file1[] = "exemplo_2.txt";

//Variaveis
int line = 0;
string matriz[100][15];
string tx[3] = {"", "", ""};


//Estrutura de auxilio
//=================================
typedef struct Pipeline{
  string status;
  string instruction;
  int statusID = 0;
} pipAux;

pipAux processAux[17];
//=================================

//Estrutura auxiliar para salvar o inicio de cada Label
//=================================
typedef struct Label{
  int linha;
  string nomeLabel;
}label;

label labelVet[10];
int numDeLabels = 0;
int numDeInstrucoes = 0;
//=================================

//Carrega todas as labels
void pesquisarLabels(){
  char aux[50];
  string aux2;
  int cont = 0;

  FILE * arquivo;
  arquivo = fopen(file1, "r");

  while(fgets(aux, 100, arquivo) != NULL){
    if(aux[0] == '_' || (aux[0] == 'L' && aux[2] == ':')){
      aux2 = aux;
      aux2.pop_back();
      labelVet[numDeLabels].linha = cont;
      labelVet[numDeLabels].nomeLabel = aux2;
      cout << "> Label :" << labelVet[numDeLabels].nomeLabel << " ---> Linha:  " << labelVet[numDeLabels].linha << endl;
      numDeLabels++;
    }
    else {
      numDeInstrucoes++;
    }
    cont++;
  }

  cout << "> Numero total de instrucoes: " << numDeInstrucoes << endl;
  fclose(arquivo);
}

//Verifica se a instrução possuia condição de parada (Desvio)
bool verificaDesvio(string read){
  //Comando que geram desvios padrão
  string desvios [7] = { "jmp", "je", "jne", "jg", "jge", "jl", "jle" };

  bool flag = false;
  for(int i = 0; i < 7; i++){
    if(read == desvios[i]) flag = true;
  }

  return flag;
}

//Verifica o endereço do Registrador
int * choseRegister(string a){
  if(a == "ebp") return &ebp;
  if(a == "esp") return &esp;
  if(a == "eax") return &eax;
  if(a == "edx") return &edx;
}

//Função para alcançar a linha desejada apos o jump
void pularLinhas(FILE * arquivo, int linhaDeseja){
  int i = 0;
  char aux[100];
  fclose(arquivo);
  arquivo = fopen(file1, "r");
  while (i < linhaDeseja) {
    fgets(aux, 100, arquivo);
    if(aux[0] == '_' || aux[0] == 'L'){  }
    else { i++; }
  }
}

//Função responsavel por atualizar os dados da matriz de acordo com o "ciclo" atual
void atualizaMatriz(){
  cout << "| Relatorio : ===========================" << endl;
  for(int i = 0; i < processoAtual; i++){
    switch (processAux[i].statusID) {
      case 0:
        processAux[i].status = " FI |";
        processAux[i].statusID += 1;
        cout << "> Instrucao [i"<< i+1 << "] sendo buscada (FI)" << endl;
        break;

      case 1:
        processAux[i].status = " DI |";
        processAux[i].statusID += 1;
        cout << "> Instrucao [i"<< i+1 << "] sendo decodificada (DI)" << endl;
        break;

      case 2:
        processAux[i].status = " CO |";
        processAux[i].statusID += 1;
        cout << "> Instrucao [i"<< i+1 << "] esta calculando operandos (CO)" << endl;
        break;

      case 3:
        processAux[i].status = " FO |";
        processAux[i].statusID += 1;
        cout << "> Instrucao [i"<< i+1 << "] esta obtendo os operandos (FO)" << endl;
        break;

      case 4:
        processAux[i].status = " EI |";
        processAux[i].statusID += 1;
        cout << "> Instrucao [i"<< i+1 << "] esta executando a instrucao (EI)" << endl;
        break;

      case 5:
        processAux[i].status = " WO |";
        processAux[i].statusID = -1;
        cout << "> Instrucao [i"<< i+1 << "] esta escrevendo operando (WO) --> Finalizada" << endl;
        break;

      case -1:
        processAux[i].status = "    |";
        break;
    }//End of Switch case

  //Atualizando a Matriz
  matriz[clock1][i] = processAux[i].status;

  }//End For
}

//Printa no terminal a situação atual da matriz do PipeLine
void printMatriz(){
  cout << "|==========================+  P I P E L I N E  +=========================|" << endl;
  cout << "   | i1 | i2 | i3 | i4 | i5 | i6 | i7 | i8 | i9 | 10 | 11 | 12 | 13 | 14 |" << endl;
  for(int i = 0; i < clock1; i++){
    if(i < 9) cout << "0";
    cout << i+1 << " |";
    for(int j = 0; j < 14; j++){
      cout << matriz[i][j];
    }
    cout << endl;
  }
}

//Fncao para leitura da linha atual do txt e separa os valores
int idenificarInstrucao(char aux[], FILE * arquivo){
  int numChar, flag = 0;
  numChar = strlen(aux);
  cout << "Num de caracter: " << numChar << endl;

  tx[0] = ""; //
  tx[1] = ""; // R E S E T
  tx[2] = ""; //

  //Preenchendo as strings de auxilio
  for(int i = 1; aux[i] != '\n'; i++){

    switch (aux[i]) {
      case '\t':
        flag++;
        break;

      case ',':
        flag++;
        i++;
        break;

      default:
        tx[flag] += aux[i];
        break;
    }

  }

  //Printando as strings de auxilio
  for(int i = 0; i < 3; i++){
    cout << "Texto ["<< i << "]" <<tx[i] << endl;
  }

  //Identificando a diferenciando cada intrução assembly =============================
  if(tx[0] == "movl"){
      int * a, * b;
      a = choseRegister(tx[1]);
      b = choseRegister(tx[2]);
      cout << "Moving" << endl;
      //chama função do MOVL
  }

  //Pulando as linhas para a label desejada
  else if(tx[0] == "jmp"){
    for(int i = 0; i < numDeLabels; i++){
      if(tx[1] == labelVet[i].nomeLabel) {
        pularLinhas(arquivo, labelVet[i].linha);
        cout << "Jump para: " << labelVet[i].nomeLabel << endl;
      }
    }
  }

}

//F){nção principal do Pipeline
int loopProgram(FILE * fileChosed){
  char line[100];
  char * world;
  int x, stop = 0;
  while(stop != 1) {

    //system("clear");// <------ REMOVER PARA VER OUTPUTS DE AUXILIO

    //Caso haja alguma leitura
    if((fgets(line, 100, fileChosed) != NULL)){
      //E Caso não seja uma Label
      if(line[0] != '_' && line[0] != 'L'){


        printf("> Linha de leitura:");
        printf("%s",line);
        idenificarInstrucao(line, fileChosed);

        world = strtok(line,"\t");
        printf("> Instrucao lida: %s\n\n",world);


        processAux[processoAtual].instruction = line;
        processAux[processoAtual].statusID = 0;
        processAux[processoAtual].status = "    |";

        //Testando Jumping
        if(processAux[3].statusID == 5){
          cout << "| ==== D E S V I O ==== |" << endl << "> JMP ----> L2" << endl;
          for(int i = 0; i < 8; i++){
            processAux[i].statusID = -1;
          }

          pularLinhas(fileChosed, 8);
          processoAtual = 8;

        }

        //Atualizando os dados da matriz de visualização
        atualizaMatriz();

        puts("");
        //Pula a para a proxima linha de instrução
        processoAtual++;
        //Avança par ao proximo ciclo de instrução
        clock1++;

        printMatriz();

        puts("Continue : [0]Yes\t[1]No");
        scanf("%d",&stop);

      }//Fim if

      //Caso não seja uma label OU o arquivo terminou
      else {
        stop = 0;
        puts("Leitura de Label!");
      }
    }//Fim If (Fgets)

  }//Fim While

}

//Main?!
int main(int argc, char const *argv[]) {

  srand(time(NULL));
  pesquisarLabels();

  //Abertura do arquivo
  FILE * fileChosed = fopen(file1, "r");

  if(fileChosed != NULL)
    puts("\n> Arquivo aberto!");
  else
    puts("> !Erro de abertura de arquivo! ");

  int entrada = rand() % 5;
  cout << "\n> O numero de entrada eh: " << entrada << endl;


  int stop = 0;


  //Inicializando a matriz de exibição
  for(int i = 0; i < 14; i++){
    for(int j = 0; j < 100; j++){
      matriz[i][j] = "    |";
    }
  }

  puts("\n\nFile:\n");
  //readFile();

  loopProgram(fileChosed);

  fclose(fileChosed);


  return 0;
}
