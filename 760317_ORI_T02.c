/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização e Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 760317
 * Aluno: Fábio Yukio Yamaya
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 31
#define TAM_GENERO 2
#define TAM_NASCIMENTO 11
#define TAM_CELULAR 16
#define TAM_VEICULO 31
#define TAM_PLACA 9
#define TAM_DATA 9
#define TAM_HORA 6
#define TAM_TRAJETO 121
#define TAM_VALOR 7
#define TAM_VAGAS 2
#define TAM_STRING_INDICE (30 + 6 + 4 + 1)

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS "Busca por %s.\nNos percorridos:\n"

/* Registro da Carona */
typedef struct
{
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char genero[TAM_GENERO];
    char nascimento[TAM_NASCIMENTO]; /* DD/MM/AAAA */
    char celular[TAM_CELULAR];
    char veiculo[TAM_VEICULO];
    char placa[TAM_PLACA];
    char trajeto[TAM_TRAJETO];
    char data[TAM_DATA];   /* DD/MM/AA, ex: 24/09/19 */
    char hora[TAM_HORA];   /* HH:MM, ex: 07:30 */
    char valor[TAM_VALOR]; /* 999.99, ex: 004.95 */
    char vagas[TAM_VAGAS];
} Carona;

/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
    char pk[TAM_PRIMARY_KEY];
    int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
    char string[TAM_STRING_INDICE + 1];
    char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
    int num_chaves;  /* numero de chaves armazenadas*/
    Chave_ip *chave; /* vetor das chaves e rrns [m-1]*/
    int *desc;       /* ponteiros para os descendentes, *desc[m]*/
    char folha;      /* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
    int num_chaves;  /* numero de chaves armazenadas*/
    Chave_is *chave; /* vetor das chaves e rrns [m-1]*/
    int *desc;       /* ponteiros para os descendentes, *desc[m]*/
    char folha;      /* flag folha da arvore*/
} node_Btree_is;

typedef struct
{
    int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
char ARQUIVO_IP[2000 * sizeof(Chave_ip)];
char ARQUIVO_IS[2000 * sizeof(Chave_is)];
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de Caronas  */
void criar_iride(Indice *iride);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, int ip);

/* Aloca dinamicamente os vetores de chaves e descendentes */
void *criar_no(char ip);

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada. */
int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho);

/* Percorre a arvore e retorna a pk da string destino/data-hora informada. Retorna -1, caso não
 * encontrada. */
char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho);

/* Realiza percurso em-ordem imprimindo todas as caronas na ordem lexicografica do destino e data/hora e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_is(int noderrn, int nivel);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn);
int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn);

/* Lista todos os registros não marcados para remoção 
void listar(Indice iprimary, Indice iride);*/

/* Realiza percurso pré-ordem imprimindo as chaves primárias dos registros e o
 * nível de cada nó (raiz = nível 1) */
void imprimir_arvore_ip(int noderrn, int nivel);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn);

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice *iprimary, Indice *iride);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice iride);

void listar(Indice iprimary, Indice iride);

/*******************************************************/

void libera_no(void *node, char ip);

/*Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j);

/* Insere um novo registro na Árvore B */
void insere_chave_ip(Indice *iprimary, Chave_ip chave);

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão. */
int insere_aux_ip(int noderrn, Chave_ip *chave);
int insere_aux_is(int noderrn, Chave_is *chave);

/* VOCÊS NÃO NECESSARIAMENTE PRECISAM USAR TODAS ESSAS FUNÇÕES, É MAIS PARA TEREM UMA BASE MESMO, 
 * PODEM CRIAR SUAS PRÓPRIAS FUNÇÕES SE PREFERIREM */

void recuperarNo(int noderrn, void *no, int ip);

int compararChaves(const void *a, const void *b);

void imprimirNoIp(node_Btree_ip node);

void imprimirNoIs(node_Btree_is node);

void encontraDestino(char *trajeto);

void insere_chave_is(Indice *iride, Chave_is chave);

void escreveArquivo(Carona car, int rrn);

int exibir_registro(int rrn);

void exibir_no_busca(void *no, int op);

void listarAux(int rrn, int op, int nivel);

void exibir_carona_is(Chave_is chave);

int main()
{
    char *p; /* # */
             /* Arquivo */
    int carregarArquivo = 0;
    nregistros = 0, nregistrosip = 0, nregistrosis = 0;
    scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
    if (carregarArquivo)
        nregistros = carregar_arquivo();

    scanf("%d %d%*c", &ordem_ip, &ordem_is);

    tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
    tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 10);

    /* Índice primário */
    Indice iprimary;
    iprimary.raiz = -1;
    ARQUIVO_IP[0] = '\0';
    criar_iprimary(&iprimary);

    /*Índice secundário de nomes dos Caronas */
    Indice iride;
    iride.raiz = -1;
    criar_iride(&iride);

    /* imprimir_arvore_ip(iprimary.raiz, 1);
    imprimir_arvore_is(iride.raiz, 1);
 */
    /* Execução do programa */
    int opcao = 0;
    while (1)
    {
        scanf("%d%*c", &opcao);
        switch (opcao)
        {
        case 1: /* Cadastrar uma nova Carona */
            cadastrar(&iprimary, &iride);
            break;
        case 2: /* Alterar a qtd de vagas de uma Carona */
            printf(INICIO_ALTERACAO);
            if (alterar(iprimary))
                printf(SUCESSO);
            else
                printf(FALHA);
            break;
        case 3: /* Buscar uma Carona */
            printf(INICIO_BUSCA);
            buscar(iprimary, iride);
            break;
        case 4: /* Listar todos as Caronas */
            printf(INICIO_LISTAGEM);
            listar(iprimary, iride);
            break;
        case 5: /* Imprimir o arquivo de dados */
            printf(INICIO_ARQUIVO);
            printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
            break;
        case 6: /* Imprime o Arquivo de Índice Primário*/
            printf(INICIO_INDICE_PRIMARIO);
            if (!*ARQUIVO_IP)
                puts(ARQUIVO_VAZIO);
            else
                for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
                {
                    fwrite(p, 1, tamanho_registro_ip, stdout);
                    puts("");
                }
            break;
        case 7: /* Imprime o Arquivo de Índice Secundário*/
            printf(INICIO_INDICE_SECUNDARIO);
            if (!*ARQUIVO_IS)
                puts(ARQUIVO_VAZIO);
            else
                for (p = ARQUIVO_IS; *p != '\0'; p += tamanho_registro_is)
                {
                    fwrite(p, 1, tamanho_registro_is, stdout);
                    puts("");
                }
            //printf("%s\n", ARQUIVO_IS);
            break;
        case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
            return 0;
        default: /* exibe mensagem de erro */
            printf(OPCAO_INVALIDA);
            break;
        }
    }
    return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
    scanf("%[^\n]%*c", ARQUIVO);
    return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{
    if (rrn < 0)
        return 0;

    Carona j = recuperar_registro(rrn);
    char *traj, trajeto[TAM_TRAJETO];

    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.genero);
    printf("%s\n", j.nascimento);
    printf("%s\n", j.celular);
    printf("%s\n", j.veiculo);
    printf("%s\n", j.placa);
    printf("%s\n", j.data);
    printf("%s\n", j.hora);
    printf("%s\n", j.valor);
    printf("%s\n", j.vagas);

    strcpy(trajeto, j.trajeto);

    traj = strtok(trajeto, "|");

    while (traj != NULL)
    {
        printf("%s", traj);
        traj = strtok(NULL, "|");
        if (traj != NULL)
        {
            printf(", ");
        }
    }

    printf("\n");

    return 1;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn)
{
    char temp[257], *p;
    strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
    temp[256] = '\0';
    Carona j;

    p = strtok(temp, "@");
    strcpy(j.nome, p);
    p = strtok(NULL, "@");
    strcpy(j.genero, p);
    p = strtok(NULL, "@");
    strcpy(j.nascimento, p);
    p = strtok(NULL, "@");
    strcpy(j.celular, p);
    p = strtok(NULL, "@");
    strcpy(j.veiculo, p);
    p = strtok(NULL, "@");
    strcpy(j.placa, p);
    p = strtok(NULL, "@");
    strcpy(j.trajeto, p);
    p = strtok(NULL, "@");
    strcpy(j.data, p);
    p = strtok(NULL, "@");
    strcpy(j.hora, p);
    p = strtok(NULL, "@");
    strcpy(j.valor, p);
    p = strtok(NULL, "@");
    strcpy(j.vagas, p);

    gerarChave(&j);

    return j;
}

void gerarChave(Carona *novo)
{
    novo->pk[0] = '\0';

    strncat(novo->pk, novo->nome, 1);
    strncat(novo->pk, novo->placa, 3);
    strncat(novo->pk, novo->data, 2);
    strncat(novo->pk, &novo->data[3], 2);
    strncat(novo->pk, novo->hora, 2);

    return;
}

void criar_iprimary(Indice *iprimary)
{
    if (nregistros == 0)
    {
        return;
    }

    Chave_ip chave;

    for (size_t i = 0; i < nregistros; i++)
    {
        Carona recuperado = recuperar_registro(i);
        if (buscar_chave_ip(iprimary->raiz, recuperado.pk, 0) == -1)
        {
            strcpy(chave.pk, recuperado.pk);
            chave.rrn = i;
            insere_chave_ip(iprimary, chave);
        }
    }
}

void criar_iride(Indice *iride)
{
    if (nregistros == 0)
    {
        return;
    }

    char destino[TAM_TRAJETO];
    Chave_is chave;

    for (size_t i = 0; i < nregistros; i++)
    {
        Carona recuperado = recuperar_registro(i);

        strcpy(chave.pk, recuperado.pk);
        chave.string[0] = '\0';
        strcpy(destino, recuperado.trajeto);
        encontraDestino(destino);
        strcat(chave.string, destino);
        strcat(chave.string, "$");
        //Data
        strncat(chave.string, recuperado.data + 6, 2);
        strncat(chave.string, recuperado.data + 3, 2);
        strncat(chave.string, recuperado.data, 2);
        //Horario
        strncat(chave.string, recuperado.hora, 2);
        strncat(chave.string, recuperado.hora + 3, 2);
        if (buscar_chave_is(iride->raiz, chave.string, 0) == NULL)
        {
            insere_chave_is(iride, chave);
        }
    }
}

void insere_chave_ip(Indice *iprimary, Chave_ip chave)
{
    Chave_ip *chaveP = (Chave_ip *)malloc(sizeof(Chave_ip));
    *chaveP = chave;

    if (buscar_chave_ip(iprimary->raiz, chave.pk, 0) != -1)
    {
        printf(ERRO_PK_REPETIDA, chave.pk);
        return;
    }
    node_Btree_ip *no = (node_Btree_ip *)criar_no(1);
    int i, rrn = iprimary->raiz;
    int filho_direito;

    if (nregistrosip == 0)
    {
        no->chave[0] = chave;
        no->folha = 'T';
        no->num_chaves = 1;
        for (size_t i = 0; i < ordem_is; i++)
        {
            no->desc[i] = -1;
        }
        write_btree((void *)no, 0, 1);
        iprimary->raiz = 0;
        nregistrosip++;
        libera_no((void *)no, 1);
        return;
    }

    filho_direito = insere_aux_ip(iprimary->raiz, chaveP);

    if (filho_direito != -1)
    {
        no->folha = 'F';
        no->num_chaves = 1;
        no->chave[0] = *chaveP;
        no->chave[0].pk[10] = '\0';
        no->desc[0] = iprimary->raiz;
        no->desc[1] = filho_direito;

        iprimary->raiz = nregistrosip;
        write_btree((void *)no, nregistrosip, 1);
        nregistrosip++;
    }

    free(chaveP);
    libera_no((void *)no, 1);

    return;
}

void insere_chave_is(Indice *iride, Chave_is chave)
{
    Chave_is *chaveP = (Chave_is *)malloc(sizeof(Chave_is));
    *chaveP = chave;

    if (buscar_chave_is(iride->raiz, chave.string, 0) != NULL)
    {
        return;
        printf(ERRO_PK_REPETIDA, chave.pk);
    }
    node_Btree_is *no = (node_Btree_is *)criar_no(0);
    int i, rrn = iride->raiz;
    int filho_direito;

    if (nregistrosis == 0)
    {
        no->chave[0] = chave;
        no->folha = 'T';
        no->num_chaves = 1;
        for (size_t i = 0; i < ordem_is; i++)
        {
            no->desc[i] = -1;
        }
        write_btree((void *)no, 0, 0);
        iride->raiz = 0;
        nregistrosis++;
        libera_no((void *)no, 0);
        return;
    }

    filho_direito = insere_aux_is(iride->raiz, chaveP);

    if (filho_direito != -1)
    {
        no->folha = 'F';
        no->num_chaves = 1;
        no->chave[0] = *chaveP;
        no->desc[0] = iride->raiz;
        no->desc[1] = filho_direito;

        iride->raiz = nregistrosis;
        write_btree((void *)no, nregistrosis, 0);
        nregistrosis++;
    }

    free(chaveP);
    libera_no((void *)no, 0);

    return;
}

int insere_aux_ip(int noderrn, Chave_ip *chave)
{
    node_Btree_ip *no = (node_Btree_ip *)criar_no(1);
    recuperarNo(noderrn, (void *)no, 1);
    int i, filho_direito;

    if (no->folha == 'T')
    {
        if (no->num_chaves < ordem_ip - 1)
        {
            i = no->num_chaves - 1;
            while (i >= 0 && strcmp(chave->pk, no->chave[i].pk) < 0)
            {
                no->chave[i + 1] = no->chave[i];
                i--;
            }
            i++;
            no->chave[i] = *chave;
            no->num_chaves++;
            write_btree((void *)no, noderrn, 1);
            chave->rrn = -1;
            libera_no((void *)no, 1);
            return -1;
        }
        else
        {
            libera_no((void *)no, 1);
            return divide_no_ip(noderrn, chave, -1);
        }
    }
    else
    {
        i = no->num_chaves - 1;
        while (i >= 0 && strcmp(chave->pk, no->chave[i].pk) < 0)
        {
            i--;
        }
        i++;

        filho_direito = insere_aux_ip(no->desc[i], chave);

        if (chave->rrn != -1)
        {
            if (no->num_chaves < ordem_ip - 1)
            {
                i = no->num_chaves - 1;
                while (i >= 0 && strcmp(chave->pk, no->chave[i].pk) < 0)
                {
                    no->chave[i + 1] = no->chave[i];
                    no->desc[i + 2] = no->desc[i + 1];
                    i--;
                }
                no->chave[i + 1] = *chave;
                no->desc[i + 2] = filho_direito;
                no->num_chaves++;
                write_btree((void *)no, noderrn, 1);
                chave->rrn = -1;

                libera_no((void *)no, 1);

                return -1;
            }
            else
            {
                libera_no((void *)no, 1);
                return divide_no_ip(noderrn, chave, filho_direito);
            }
        }
    }
    libera_no((void *)no, 1);
    return -1;
}

int insere_aux_is(int noderrn, Chave_is *chave)
{
    node_Btree_is *no = (node_Btree_is *)criar_no(0);
    recuperarNo(noderrn, (void *)no, 0);
    int i, filho_direito;

    if (no->folha == 'T')
    {
        if (no->num_chaves < ordem_is - 1)
        {
            i = no->num_chaves - 1;
            while (i >= 0 && strcmp(chave->string, no->chave[i].string) < 0)
            {
                no->chave[i + 1] = no->chave[i];
                i--;
            }
            i++;
            no->chave[i] = *chave;
            no->num_chaves++;
            write_btree((void *)no, noderrn, 0);
            strcpy(chave->pk, "##########\0");
            libera_no((void *)no, 0);
            return -1;
        }
        else
        {
            libera_no((void *)no, 0);
            return divide_no_is(noderrn, chave, -1);
        }
    }
    else
    {
        i = no->num_chaves - 1;
        while (i >= 0 && strcmp(chave->string, no->chave[i].string) < 0)
        {
            i--;
        }
        i++;

        filho_direito = insere_aux_is(no->desc[i], chave);

        if (chave->pk[0] != '#')
        {
            if (no->num_chaves < ordem_is - 1)
            {
                i = no->num_chaves - 1;
                while (i >= 0 && strcmp(chave->string, no->chave[i].string) < 0)
                {
                    no->chave[i + 1] = no->chave[i];
                    no->desc[i + 2] = no->desc[i + 1];
                    i--;
                }
                no->chave[i + 1] = *chave;
                no->desc[i + 2] = filho_direito;
                no->num_chaves++;
                write_btree((void *)no, noderrn, 0);
                strcpy(chave->pk, "##########\0");

                libera_no((void *)no, 0);

                return -1;
            }
            else
            {
                libera_no((void *)no, 0);
                return divide_no_is(noderrn, chave, filho_direito);
            }
        }
    }
    libera_no((void *)no, 0);
    return -1;
}

int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn)
{
    node_Btree_ip *no = (node_Btree_ip *)criar_no(1);
    recuperarNo(rrnesq, (void *)no, 1);
    int i, chave_alocada;

    i = no->num_chaves - 1;
    chave_alocada = 0;

    node_Btree_ip *novoNo = (node_Btree_ip *)criar_no(1);
    novoNo->folha = no->folha;
    novoNo->num_chaves = (ordem_ip - 1) / 2;

    for (int j = novoNo->num_chaves - 1; j >= 0; j--)
    {
        if (!chave_alocada && strcmp(chave->pk, no->chave[i].pk) > 0)
        {
            novoNo->chave[j] = *chave;
            novoNo->desc[j + 1] = desc_dir_rrn;
            chave_alocada = 1;
        }
        else
        {
            novoNo->chave[j] = no->chave[i];
            strcpy(no->chave[i].pk, "##########\0");
            novoNo->desc[j + 1] = no->desc[i + 1];
            no->desc[i + 1] = -1;
            i--;
        }
    }

    if (!chave_alocada)
    {
        while (i >= 0 && strcmp(chave->pk, no->chave[i].pk) < 0)
        {
            no->chave[i + 1] = no->chave[i];
            no->desc[i + 2] = no->desc[i + 1];
            i--;
        }
        no->chave[i + 1] = *chave;
        no->desc[i + 2] = desc_dir_rrn;
    }

    int pos = ordem_ip / 2;
    *chave = no->chave[pos];
    novoNo->desc[0] = no->desc[pos + 1];
    no->desc[pos + 1] = -1;
    no->num_chaves = ordem_ip / 2;

    write_btree((void *)no, rrnesq, 1);
    write_btree((void *)novoNo, nregistrosip, 1);

    libera_no(no, 1);
    libera_no(novoNo, 1);

    nregistrosip++;

    return nregistrosip - 1;
}

int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn)
{
    node_Btree_is *no = (node_Btree_is *)criar_no(0);
    recuperarNo(rrnesq, (void *)no, 0);
    int i, chave_alocada;

    i = no->num_chaves - 1;
    chave_alocada = 0;

    node_Btree_is *novoNo = (node_Btree_is *)criar_no(0);
    novoNo->folha = no->folha;
    novoNo->num_chaves = (ordem_is - 1) / 2;

    for (int j = novoNo->num_chaves - 1; j >= 0; j--)
    {
        if (!chave_alocada && strcmp(chave->string, no->chave[i].string) > 0)
        {
            novoNo->chave[j] = *chave;
            novoNo->desc[j + 1] = desc_dir_rrn;
            chave_alocada = 1;
        }
        else
        {
            novoNo->chave[j] = no->chave[i];
            novoNo->desc[j + 1] = no->desc[i + 1];
            no->desc[i + 1] = -1;
            i--;
        }
    }

    if (!chave_alocada)
    {
        while (i >= 0 && strcmp(chave->string, no->chave[i].string) < 0)
        {
            no->chave[i + 1] = no->chave[i];
            no->desc[i + 2] = no->desc[i + 1];
            i--;
        }
        no->chave[i + 1] = *chave;
        no->desc[i + 2] = desc_dir_rrn;
    }

    int pos = ordem_is / 2;
    *chave = no->chave[pos];
    novoNo->desc[0] = no->desc[pos + 1];
    no->desc[pos + 1] = -1;
    no->num_chaves = ordem_is / 2;

    write_btree((void *)no, rrnesq, 0);
    write_btree((void *)novoNo, nregistrosis, 0);

    libera_no(no, 0);
    libera_no(novoNo, 0);

    nregistrosis++;

    return nregistrosis - 1;
}

void recuperarNo(int noderrn, void *no, int ip)
{
    char temp[5];

    if (ip)
    {
        node_Btree_ip *node = (node_Btree_ip *)no;
        char *posicao = ARQUIVO_IP + (tamanho_registro_ip * noderrn);
        strncpy(temp, posicao, 3);
        temp[3] = '\0';
        int aux;
        node->num_chaves = atoi(temp);
        posicao = posicao + 3;
        for (size_t j = 0; j < ordem_ip - 1; j++)
        {
            strncpy(node->chave[j].pk, posicao, 10);
            node->chave[j].pk[10] = '\0';
            strncpy(temp, posicao + 10, 4);
            temp[4] = '\0';
            if (temp[0] == '*')
            {
                node->chave[j].rrn = -1;
            }
            else
            {
                node->chave[j].rrn = atoi(temp);
            }
            posicao = posicao + 14;
        }
        node->folha = posicao[0];
        posicao++;
        for (size_t i = 0; i < ordem_ip; i++)
        {
            strncpy(temp, posicao, 3);
            temp[3] = '\0';
            if (temp[0] == '*')
            {
                node->desc[i] = -1;
            }
            else
            {
                node->desc[i] = atoi(temp);
            }
            posicao += 3;
        }
    }
    else
    {
        node_Btree_is *node = (node_Btree_is *)no;
        char *posicao = ARQUIVO_IS + (tamanho_registro_is * noderrn);

        strncpy(temp, posicao, 3);
        temp[3] = '\0';
        node->num_chaves = atoi(temp);

        posicao = posicao + 3;

        for (size_t i = 0; i < ordem_is - 1; i++)
        {
            strncpy(node->chave[i].pk, posicao, 10);
            posicao += 10;
            node->chave[i].pk[10] = '\0';
            strncpy(node->chave[i].string, posicao, 41);
            node->chave[i].string[40] = '\0';
            posicao += 41;
        }
        node->folha = posicao[0];
        posicao++;
        for (size_t i = 0; i < ordem_is; i++)
        {
            strncpy(temp, posicao, 3);
            temp[3] = '\0';
            if (temp[0] == '*')
            {
                node->desc[i] = -1;
            }
            else
            {
                node->desc[i] = atoi(temp);
            }
            posicao += 3;
        }
    }

    return;
}

int compararChaves(const void *a, const void *b)
{
    int comp = strcmp(((Chave_ip *)a)->pk, ((Chave_ip *)b)->pk);
    if (comp == 0)
    {
        return 0;
    }
    else if (comp > 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void imprimir_arvore_ip(int noderrn, int nivel)
{
    node_Btree_ip *node = (node_Btree_ip *)criar_no(1);
    recuperarNo(noderrn, (void *)node, 1);
    printf("Nivel: %d///////////////\n", nivel);
    printf("Node rrn: %d/////////////\n", noderrn);

    imprimirNoIp(*node);

    if (node->folha == 'T')
    {
        return;
    }

    for (size_t i = 0; i <= node->num_chaves; i++)
    {
        imprimir_arvore_ip(node->desc[i], nivel + 1);
    }
    libera_no((void *)node, 1);
    return;
}

void imprimirNoIp(node_Btree_ip node)
{
    for (size_t i = 0; i < node.num_chaves; i++)
    {
        puts(node.chave[i].pk);
        printf("%d\n", node.chave[i].rrn);
    }
}

void write_btree(void *salvar, int rrn, char ip)
{
    if (ip == 1)
    {
        char temp[tamanho_registro_ip], noArray[tamanho_registro_ip + 1];
        noArray[0] = '\0';
        char *posicao = ARQUIVO_IP + (tamanho_registro_ip * rrn);
        node_Btree_ip *node = (node_Btree_ip *)salvar;
        sprintf(temp, "%03d", node->num_chaves);
        strcat(noArray, temp);
        for (size_t i = 0; i < ordem_ip - 1; i++)
        {
            if (i < node->num_chaves)
            {
                strcat(noArray, node->chave[i].pk);
                if (node->chave[i].rrn != -1)
                {
                    sprintf(temp, "%04d", node->chave[i].rrn);
                }
                else
                {
                    sprintf(temp, "****");
                }
                strcat(noArray, temp);
            }
            else
            {
                strcat(noArray, "##############");
            }
        }

        sprintf(temp, "%c", node->folha);
        strcat(noArray, temp);

        for (size_t i = 0; i < ordem_ip; i++)
        {
            if (i <= node->num_chaves && node->desc[i] != -1)
            {
                sprintf(temp, "%03d", node->desc[i]);
                strcat(noArray, temp);
            }
            else
            {
                strcat(noArray, "***");
            }
        }
        strncpy(posicao, noArray, tamanho_registro_ip);
        return;
    }
    else
    {
        char temp[tamanho_registro_is], noArray[tamanho_registro_is + 1];
        noArray[0] = '\0';
        char *posicao = ARQUIVO_IS + (tamanho_registro_is * rrn);
        node_Btree_is *node = (node_Btree_is *)salvar;
        sprintf(temp, "%03d", node->num_chaves);
        strcat(noArray, temp);
        for (size_t i = 0; i < ordem_is - 1; i++)
        {
            if (i < node->num_chaves)
            {
                strcat(noArray, node->chave[i].pk);
                while (strlen(node->chave[i].string) < 41)
                {
                    strcat(node->chave[i].string, "#");
                }
                strcat(noArray, node->chave[i].string);
            }
            else
            {
                for (size_t j = 0; j < 51; j++)
                {
                    strcat(noArray, "#");
                }
            }
        }

        sprintf(temp, "%c", node->folha);
        strcat(noArray, temp);

        for (size_t i = 0; i < ordem_is; i++)
        {
            if (i <= node->num_chaves && node->desc[i] != -1)
            {
                sprintf(temp, "%03d", node->desc[i]);
                strcat(noArray, temp);
            }
            else
            {
                strcat(noArray, "***");
            }
        }
        strncpy(posicao, noArray, tamanho_registro_is);
        return;
    }
}

void *criar_no(char ip)
{
    if (ip == 1)
    {
        node_Btree_ip *no = (node_Btree_ip *)malloc(sizeof(node_Btree_ip));
        no->chave = (Chave_ip *)malloc((ordem_ip - 1) * sizeof(Chave_ip));
        no->desc = (int *)malloc(ordem_ip * sizeof(int));
        return (void *)no;
    }
    else
    {
        node_Btree_is *no = (node_Btree_is *)malloc(sizeof(node_Btree_is));
        no->chave = (Chave_is *)malloc((ordem_is - 1) * sizeof(Chave_is));
        no->desc = (int *)malloc(ordem_is * sizeof(int));
        return (void *)no;
    }
}

void libera_no(void *node, char ip)
{
    if (ip == 1)
    {
        node_Btree_ip *nodeIp = (node_Btree_ip *)node;
        free(nodeIp->desc);
        free(nodeIp->chave);
        free(nodeIp);
    }
    else
    {
        node_Btree_is *nodeIs = (node_Btree_is *)node;
        free(nodeIs->desc);
        free(nodeIs->chave);
        free(nodeIs);
    }

    return;
}

int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho)
{
    if (noderrn == -1)
    {
        return -1;
    }

    node_Btree_ip *no = (node_Btree_ip *)criar_no(1);
    recuperarNo(noderrn, (void *)no, 1);
    int i;

    if (exibir_caminho)
    {
        exibir_no_busca((void *)no, 1);
    }

    i = 0;
    while (strcmp(pk, no->chave[i].pk) > 0 && i < no->num_chaves)
    {
        i++;
    }
    if (strcmp(pk, no->chave[i].pk) == 0)
    {
        int ret = no->chave[i].rrn;
        libera_no((void *)no, 1);
        return ret;
    }

    if (no->folha == 'T')
    {
        libera_no((void *)no, 1);
        return -1;
    }
    else
    {
        int nxtrrn = no->desc[i];
        libera_no((void *)no, 1);
        return buscar_chave_ip(nxtrrn, pk, exibir_caminho);
    }
}

void encontraDestino(char *trajeto)
{
    char *traj;

    traj = strtok(trajeto, "|");

    while (traj != NULL)
    {
        strcpy(trajeto, traj);
        traj = strtok(NULL, "|");
    }
}

char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho)
{
    if (noderrn == -1)
    {
        return NULL;
    }

    node_Btree_is *no = (node_Btree_is *)criar_no(0);
    recuperarNo(noderrn, (void *)no, 0);
    int i, rrn = noderrn;

    if (exibir_caminho)
    {
        exibir_no_busca((void *)no, 0);
    }

    i = 0;
    while (strcmp(titulo, no->chave[i].string) > 0 && i < no->num_chaves)
    {
        i++;
    }
    for (int j = 0; j < TAM_STRING_INDICE; j++)
    {
        if (no->chave[i].string[j] == '#')
        {
            no->chave[i].string[j] = '\0';
            j = TAM_STRING_INDICE;
        }
    }
    if (strcmp(titulo, no->chave[i].string) == 0)
    {
        char *pk = (char *)malloc(TAM_PRIMARY_KEY);
        strcpy(pk, no->chave[i].pk);
        libera_no((void *)no, 0);
        return pk;
    }

    if (no->folha == 'T')
    {
        libera_no((void *)no, 0);
        return NULL;
    }
    else
    {
        int nxtrrn = no->desc[i];
        libera_no((void *)no, 0);
        return buscar_chave_is(nxtrrn, titulo, exibir_caminho);
    }
}

int imprimir_arvore_is(int noderrn, int nivel)
{
    node_Btree_is *node = (node_Btree_is *)criar_no(0);
    recuperarNo(noderrn, (void *)node, 0);
    printf("Nivel: %d///////////////\n", nivel);
    printf("Node rrn: %d/////////////\n", noderrn);

    imprimirNoIs(*node);

    if (node->folha == 'T')
    {
        return 1;
    }

    for (size_t i = 0; i <= node->num_chaves; i++)
    {
        imprimir_arvore_is(node->desc[i], nivel + 1);
    }
    libera_no((void *)node, 0);
    return 1;
}

void imprimirNoIs(node_Btree_is node)
{
    for (size_t i = 0; i < node.num_chaves; i++)
    {
        puts(node.chave[i].pk);
        puts(node.chave[i].string);
    }
}

void cadastrar(Indice *iprimary, Indice *iride)
{
    Carona novaCarona;

    scanf("%[^\n]", novaCarona.nome);
    scanf("\n%[^\n]", novaCarona.genero);
    scanf("\n%[^\n]", novaCarona.nascimento);
    scanf("\n%[^\n]", novaCarona.celular);
    scanf("\n%[^\n]", novaCarona.veiculo);
    scanf("\n%[^\n]", novaCarona.placa);
    scanf("\n%[^\n]", novaCarona.trajeto);
    scanf("\n%[^\n]", novaCarona.data);
    scanf("\n%[^\n]", novaCarona.hora);
    scanf("\n%[^\n]", novaCarona.valor);
    scanf("\n%[^\n]", novaCarona.vagas);

    gerarChave(&novaCarona);

    if (buscar_chave_ip(iprimary->raiz, novaCarona.pk, 0) != -1)
    {
        printf(ERRO_PK_REPETIDA, novaCarona.pk);
        return;
    }
    escreveArquivo(novaCarona, nregistros);
    inserir_registro_indices(iprimary, iride, novaCarona);
    nregistros++;
}

int alterar(Indice iprimary)
{
    char chave[TAM_PRIMARY_KEY];
    scanf("%[^\n]", chave);

    int rrn = buscar_chave_ip(iprimary.raiz, chave, 0);

    if (rrn == -1)
    {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }

    int novaVaga;
    int campoValido = 0;
    while (!campoValido)
    {
        scanf("%d", &novaVaga);
        if (novaVaga > 0 && novaVaga < 9)
        {
            campoValido = 1;
        }
        else
        {
            printf(CAMPO_INVALIDO);
        }
        fflush(stdin);
    }

    Carona nova = recuperar_registro(rrn);
    sprintf(nova.vagas, "%d", novaVaga);
    escreveArquivo(nova, rrn);
    return 1;
}

void buscar(Indice iprimary, Indice iride)
{
    Chave_ip chaveIp;
    Chave_is chaveIs;
    int rrn, op;
    char *chave;
    char destino[31];
    char data[TAM_DATA];
    char hora[TAM_HORA];

    scanf("%d%*c", &op);

    switch (op)
    {
    case 1:
        fgets(chaveIp.pk, TAM_PRIMARY_KEY, stdin);
        chaveIp.pk[10] = '\0';

        printf(NOS_PERCORRIDOS_IP, chaveIp.pk);

        rrn = buscar_chave_ip(iprimary.raiz, chaveIp.pk, 1);

        printf("\n");

        if (rrn == -1)
        {
            printf(REGISTRO_N_ENCONTRADO);
            return;
        }

        exibir_registro(rrn);
        return;

    case 2:
        scanf("%[^\n]", destino);
        scanf("\n%[^\n]", data);
        scanf("\n%[^\n]", hora);

        chaveIs.string[0] = '\0';

        strcat(chaveIs.string, destino);
        strcat(chaveIs.string, "$");
        //Data
        strncat(chaveIs.string, data + 6, 2);
        strncat(chaveIs.string, data + 3, 2);
        strncat(chaveIs.string, data, 2);
        //Horario
        strncat(chaveIs.string, hora, 2);
        strncat(chaveIs.string, hora + 3, 2);

        printf(NOS_PERCORRIDOS_IS, chaveIs.string);

        chave = buscar_chave_is(iride.raiz, chaveIs.string, 1);

        printf("\n");

        if (chave == NULL)
        {
            printf(REGISTRO_N_ENCONTRADO);
            return;
        }

        rrn = buscar_chave_ip(iprimary.raiz, chave, 0);

        exibir_registro(rrn);
        free(chave);
        return;
    }
}

void listar(Indice iprimary, Indice iride)
{

    int op;

    scanf("%d%*c", &op);
    if (nregistros == 0)
    {
        printf(REGISTRO_N_ENCONTRADO);
        return;
    }

    if (op == 1)
    {
        listarAux(iprimary.raiz, 1, 1);
    }
    else
    {
        listarAux(iride.raiz, 0, 1);
    }
    return;
}

void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j)
{
    //Atualiza ip
    Chave_ip chaveIp;
    strcpy(chaveIp.pk, j.pk);
    chaveIp.rrn = nregistros;
    insere_chave_ip(iprimary, chaveIp);

    // Atualiza is
    Chave_is chaveIs;
    char destino[TAM_TRAJETO];
    strcpy(chaveIs.pk, j.pk);
    chaveIs.string[0] = '\0';
    strcpy(destino, j.trajeto);
    encontraDestino(destino);
    strcat(chaveIs.string, destino);
    strcat(chaveIs.string, "$\0");
    //Data
    strncat(chaveIs.string, j.data + 6, 2);
    strncat(chaveIs.string, j.data + 3, 2);
    strncat(chaveIs.string, j.data, 2);

    //Horario
    strncat(chaveIs.string, j.hora, 2);
    strncat(chaveIs.string, j.hora + 3, 2);
    insere_chave_is(iride, chaveIs);
}

void escreveArquivo(Carona car, int rrn)
{
    char registro[257];

    strcpy(registro, car.nome);
    strcat(registro, "@");
    strcat(registro, car.genero);
    strcat(registro, "@");
    strcat(registro, car.nascimento);
    strcat(registro, "@");
    strcat(registro, car.celular);
    strcat(registro, "@");
    strcat(registro, car.veiculo);
    strcat(registro, "@");
    strcat(registro, car.placa);
    strcat(registro, "@");
    strcat(registro, car.trajeto);
    strcat(registro, "@");
    strcat(registro, car.data);
    strcat(registro, "@");
    strcat(registro, car.hora);
    strcat(registro, "@");
    strcat(registro, car.valor);
    strcat(registro, "@");
    strcat(registro, car.vagas);
    strcat(registro, "@");

    while (strlen(registro) < 256)
    {
        strcat(registro, "#\0");
    }

    if (rrn == nregistros)
    {
        strncat(ARQUIVO, registro, 257);
    }
    else
    {
        char *inicio = ARQUIVO + (rrn * 256);
        strncpy(inicio, registro, 256);
    }
}

void exibir_no_busca(void *no, int ip)
{
    if (ip)
    {
        node_Btree_ip *node = (node_Btree_ip *)no;
        printf("%s", node->chave[0].pk);
        if (node->num_chaves != 1)
        {
            for (size_t i = 1; i < node->num_chaves; i++)
            {
                printf(", %s", node->chave[i].pk);
            }
        }
    }
    else
    {
        node_Btree_is *node = (node_Btree_is *)no;
        for (size_t i = 0; i < TAM_STRING_INDICE; i++)
        {
            if (node->chave[0].string[i] == '#')
            {
                node->chave[0].string[i] = '\0';
                i = TAM_STRING_INDICE;
            }
        }

        printf("%s", node->chave[0].string);
        if (node->num_chaves != 1)
        {
            for (size_t i = 1; i < node->num_chaves; i++)
            {
                for (size_t j = 0; j < TAM_STRING_INDICE; j++)
                {
                    if (node->chave[i].string[j] == '#')
                    {
                        node->chave[i].string[j] = '\0';
                        j = TAM_STRING_INDICE;
                    }
                }
                printf(", %s", node->chave[i].string);
            }
        }
    }

    printf("\n");
    return;
}

void listarAux(int rrn, int ip, int nivel)
{
    if (ip)
    {
        node_Btree_ip *no = (node_Btree_ip *)criar_no(1);
        recuperarNo(rrn, no, 1);
        printf("%d - ", nivel);
        exibir_no_busca((void *)no, 1);
        if (no->folha == 'T')
        {
            libera_no((void *)no, 1);
            return;
        }
        for (size_t i = 0; i <= no->num_chaves; i++)
        {
            listarAux(no->desc[i], 1, nivel + 1);
        }
        libera_no((void *)no, 1);
        return;
    }
    else
    {
        node_Btree_is *no = (node_Btree_is *)criar_no(0);
        recuperarNo(rrn, no, 0);
        if (no->folha == 'T')
        {
            for (size_t i = 0; i < no->num_chaves; i++)
            {
                exibir_carona_is(no->chave[i]);
            }
            libera_no((void *)no, 0);
            return;
        }
        for (size_t i = 0; i <= no->num_chaves; i++)
        {
            listarAux(no->desc[i], 0, 0);
            if (i < no->num_chaves)
            {
                exibir_carona_is(no->chave[i]);
            }
        }
        libera_no((void *)no, 0);
        return;
    }
}

void exibir_carona_is(Chave_is chave)
{
    char destino[31];
    char data[TAM_DATA];
    data[0] = '\0';
    char horas[TAM_HORA];
    horas[0] = '\0';
    char *temp;
    char *posicao;
    temp = strtok(chave.string, "$");
    posicao = chave.string + strlen(temp) + 1;
    strcpy(destino, temp);
    while (strlen(destino) < 30)
    {
        strcat(destino, "-");
    }
    strncat(data, posicao + 4, 2);
    strcat(data, "/");
    strncat(data, posicao + 2, 2);
    strcat(data, "/");
    strncat(data, posicao, 2);
    posicao += 6;

    strncat(horas, posicao, 2);
    strcat(horas, ":");
    posicao += 2;
    strncat(horas, posicao, 2);
    printf("%s %s - %s-\n", destino, data, horas);
    return;
}