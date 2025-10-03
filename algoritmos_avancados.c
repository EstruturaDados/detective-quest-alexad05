#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_TABELA_HASH 10

// --- ESTRUTURAS DE DADOS ---

/**
 * @brief Estrutura para os nós da Árvore Binária que representa o mapa da mansão.
 * Cada nó é um cômodo (Sala).
 */
typedef struct Sala {
    char nome[50];
    char* pista; // Ponteiro para a pista, NULL se não houver
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

/**
 * @brief Estrutura para os nós da Árvore Binária de Busca (BST) que armazena as pistas coletadas.
 */
typedef struct PistaNode {
    char pista[50];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

/**
 * @brief Estrutura para os nós da lista encadeada usada na Tabela Hash (tratamento de colisão).
 * Associa uma pista a um suspeito.
 */
typedef struct HashNode {
    char pista[50];    // Chave
    char suspeito[50]; // Valor
    struct HashNode* proximo;
} HashNode;

// --- ÁRVORE BINÁRIA (MAPA DA MANSÃO) ---

/**
 * @brief Cria dinamicamente um novo cômodo (Sala) para o mapa.
 *
 * @param nome O nome do cômodo.
 * @param pista A pista encontrada neste cômodo (pode ser NULL).
 * @return Ponteiro para a nova Sala criada.
 */
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    if (pista != NULL) {
        novaSala->pista = (char*)malloc(strlen(pista) + 1);
        strcpy(novaSala->pista, pista);
    } else {
        novaSala->pista = NULL;
    }
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// --- ÁRVORE BINÁRIA DE BUSCA (PISTAS) ---

/**
 * @brief Insere uma nova pista na Árvore de Busca de Pistas (BST).
 * A inserção mantém a ordem alfabética.
 *
 * @param raiz Ponteiro para a raiz da BST de pistas.
 * @param pista A string da pista a ser inserida.
 * @return A nova raiz da BST.
 */
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        PistaNode* novoNode = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novoNode->pista, pista);
        novoNode->esquerda = NULL;
        novoNode->direita = NULL;
        return novoNode;
    }

    // strcmp compara strings: < 0 se o primeiro for menor, > 0 se maior, 0 se igual
    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (strcmp(pista, raiz->pista) > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    // Se a pista for igual, não faz nada para evitar duplicatas.
    return raiz;
}

/**
 * @brief Exibe todas as pistas coletadas em ordem alfabética.
 * Utiliza um percurso em-ordem (in-order) na BST.
 *
 * @param raiz A raiz da BST de pistas.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

// --- TABELA HASH (PISTA -> SUSPEITO) ---

// A Tabela Hash será um vetor de ponteiros para HashNode.
HashNode* tabelaHash[TAMANHO_TABELA_HASH];

/**
 * @brief Função hash simples para transformar a string da pista em um índice da tabela.
 * Soma os valores ASCII dos caracteres.
 *
 * @param pista A string-chave.
 * @return O índice calculado para a tabela.
 */
int funcao_hash(const char* pista) {
    int soma = 0;
    for (int i = 0; pista[i] != '\0'; i++) {
        soma += pista[i];
    }
    return soma % TAMANHO_TABELA_HASH;
}

/**
 * @brief Insere uma associação pista/suspeito na tabela hash.
 * Utiliza encadeamento para tratar colisões.
 *
 * @param pista A chave.
 * @param suspeito O valor.
 */
void inserirNaHash(const char* pista, const char* suspeito) {
    int indice = funcao_hash(pista);
    HashNode* novoNode = (HashNode*)malloc(sizeof(HashNode));
    strcpy(novoNode->pista, pista);
    strcpy(novoNode->suspeito, suspeito);
    // Adiciona no início da lista encadeada (mais simples)
    novoNode->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoNode;
}

/**
 * @brief Consulta o suspeito correspondente a uma determinada pista na tabela hash.
 *
 * @param pista A chave de busca.
 * @return O nome do suspeito se encontrado, caso contrário, NULL.
 */
char* encontrarSuspeito(const char* pista) {
    int indice = funcao_hash(pista);
    HashNode* atual = tabelaHash[indice];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// --- LÓGICA DO JOGO ---

/**
 * @brief Navega pela árvore do mapa, controla a exploração e ativa o sistema de coleta de pistas.
 *
 * @param salaInicial A sala de onde a exploração começa (a raiz do mapa).
 * @param raizPistas_ptr Ponteiro para a raiz da BST de pistas, para que possa ser modificada.
 */
void explorarSalas(Sala* salaInicial, PistaNode** raizPistas_ptr) {
    Sala* salaAtual = salaInicial;
    char escolha;

    while (1) {
        printf("\n----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // Coleta de pista
        if (salaAtual->pista != NULL) {
            printf("Voce encontrou uma pista: %s\n", salaAtual->pista);
            *raizPistas_ptr = inserirPista(*raizPistas_ptr, salaAtual->pista);
            free(salaAtual->pista); // Libera a memória da pista
            salaAtual->pista = NULL; // Marca como coletada para não coletar de novo
        } else {
            printf("Nenhuma pista nova por aqui...\n");
        }

        printf("\nPara onde voce quer ir?\n");
        printf("(e)squerda, (d)ireita ou (s)air para o julgamento? ");
        scanf(" %c", &escolha);

        if (escolha == 'e') {
            if (salaAtual->esquerda != NULL) {
                salaAtual = salaAtual->esquerda;
            } else {
                printf("Nao ha caminho para a esquerda.\n");
            }
        } else if (escolha == 'd') {
            if (salaAtual->direita != NULL) {
                salaAtual = salaAtual->direita;
            } else {
                printf("Nao ha caminho para a direita.\n");
            }
        } else if (escolha == 's') {
            printf("\nExploracao encerrada. Hora do julgamento!\n");
            break;
        } else {
            printf("Opcao invalida.\n");
        }
    }
}

// Função auxiliar recursiva para contar pistas de um suspeito
void contarPistasRecursivo(PistaNode* raizPistas, const char* suspeitoAcusado, int* contador) {
    if (raizPistas == NULL) {
        return;
    }
    char* suspeitoDaPista = encontrarSuspeito(raizPistas->pista);
    if (suspeitoDaPista != NULL && strcmp(suspeitoDaPista, suspeitoAcusado) == 0) {
        (*contador)++;
    }
    contarPistasRecursivo(raizPistas->esquerda, suspeitoAcusado, contador);
    contarPistasRecursivo(raizPistas->direita, suspeitoAcusado, contador);
}


/**
 * @brief Conduz a fase de julgamento final do jogo.
 * Pede ao jogador uma acusação e verifica se há evidências suficientes.
 *
 * @param raizPistas A raiz da BST com todas as pistas coletadas.
 */
void verificarSuspeitoFinal(PistaNode* raizPistas) {
    if (raizPistas == NULL) {
        printf("\nVoce nao coletou nenhuma pista. Impossivel fazer uma acusacao.\n");
        printf("--- FIM DE JOGO ---\n");
        return;
    }

    printf("\n--- PISTAS COLETADAS ---\n");
    exibirPistas(raizPistas);

    char acusado[50];
    printf("\nQuem voce acusa? (Ex: Sr. Mostarda, Dona Branca, Coronel Bigode) ");
    // Limpa o buffer de entrada antes de ler a string
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    // Lê a linha inteira, incluindo espaços
    fgets(acusado, sizeof(acusado), stdin);
    // Remove o '\n' que o fgets adiciona
    acusado[strcspn(acusado, "\n")] = 0;


    int contagem = 0;
    contarPistasRecursivo(raizPistas, acusado, &contagem);

    printf("\n--- VEREDITO ---\n");
    if (contagem >= 2) {
        printf("A investigacao aponta para %s com %d pista(s) contundente(s).\n", acusado, contagem);
        printf("Acusacao confirmada! Voce desvendou o misterio!\n");
    } else {
        printf("Voce acusou %s, mas encontrou apenas %d pista(s) contra ele/ela.\n", acusado, contagem);
        printf("Evidencias insuficientes! O verdadeiro culpado escapou...\n");
    }
    printf("--- FIM DE JOGO ---\n");
}

// Funções para liberar memória
void liberarMapa(Sala* raiz) {
    if (raiz == NULL) return;
    liberarMapa(raiz->esquerda);
    liberarMapa(raiz->direita);
    if (raiz->pista != NULL) free(raiz->pista);
    free(raiz);
}

void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

void liberarTabelaHash() {
    for (int i = 0; i < TAMANHO_TABELA_HASH; i++) {
        HashNode* atual = tabelaHash[i];
        while (atual != NULL) {
            HashNode* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
}


/**
 * @brief Função principal. Monta o mapa inicial, popula a tabela hash de suspeitos
 * e dá início à exploração do jogo.
 */
int main() {
    // 1. Inicializa as estruturas
    PistaNode* raizPistas = NULL;
    for(int i = 0; i < TAMANHO_TABELA_HASH; i++) {
        tabelaHash[i] = NULL;
    }

    // 2. Monta o mapa da mansão (Árvore Binária)
    Sala* raizMapa = criarSala("Hall de Entrada", NULL);
    raizMapa->esquerda = criarSala("Sala de Estar", "Castiçal");
    raizMapa->direita = criarSala("Biblioteca", "Corda");
    raizMapa->esquerda->esquerda = criarSala("Quarto", "Veneno");
    raizMapa->esquerda->direita = criarSala("Jardim de Inverno", "Adaga");
    raizMapa->direita->esquerda = criarSala("Cozinha", "Chave Inglesa");

    // 3. Monta a base de conhecimento (Tabela Hash)
    inserirNaHash("Castiçal", "Sr. Mostarda");
    inserirNaHash("Corda", "Dona Branca");
    inserirNaHash("Veneno", "Sr. Mostarda");
    inserirNaHash("Adaga", "Coronel Bigode");
    inserirNaHash("Chave Inglesa", "Dona Branca");


    // 4. Inicia o jogo
    printf("Bem-vindo ao Detective Quest!\n");
    printf("Explore a mansao, colete as pistas e descubra o culpado.\n");
    explorarSalas(raizMapa, &raizPistas);

    // 5. Fase final
    verificarSuspeitoFinal(raizPistas);

    // 6. Libera toda a memória alocada
    liberarMapa(raizMapa);
    liberarPistas(raizPistas);
    liberarTabelaHash();

    return 0;
}