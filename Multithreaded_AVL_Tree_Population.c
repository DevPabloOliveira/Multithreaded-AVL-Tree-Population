/*
    Módulo: Multithreaded_AVL_Tree_Population.c
    Descrição: Programa para demonstrar o uso de uma árvore AVL

    Autor: [Pablo Oliveira]

    Histórico de revisões:
     - Versão 1.0
     - Descrição: Implementação inicial do código
     - Autor: Pablo Oliveira

    - Versão 1.1
     - Descrição: Foram identificadas restrições de capacidade e limitações de recursos devido à sobrecarga na árvore AVL causada por uma grande
                  quantidade de inserções.
     - Autor: Pablo Oliveira

    - Versão 1.2
     - Descrição: A implementação de programação paralela com o uso de threads foi utilizado como solução para lidar com a sobrecarga nas
                  inserções na árvore AVL. Ao distribuir as inserções entre threads, foi possível processá-las em paralelo, aproveitando
                  melhor os recursos disponíveis e acelerando o tempo de execução.
     - Autor: Pablo Oliveira

    - Versão 1.3
     - Descrição: Melhoria na eficiência do algoritmo empregação do uso de mutex para exclusão mutúa
     - Autor: Pablo Oliveira


    Descrição dos testes:
        Os testes tem como objetivo analisar a escalabilidade e a adaptabilidade do código a mudanças na carga de trabalho

        Teste 1:
            Elementos 1.000
            Remoção 500
            Quantidade de threads: 3
            Resultado: 0,375000 segundos

       Teste 2:
            Elementos 100.000
            Remoção 50.000
            Quantidade de threads: 3
            Resultado: 28,277000 segundos

        Teste 3:
            Elementos 1.000.000
            Remoção 500.000
            Quantidade de threads: 3
            Resultado: 46,300000 segundos

        Teste 4:
            Elementos 10.000.000
            Remoção 5.000.000
            Quantidade de threads: 3
            Resultado: 73,829000  segundos / 1,23 minutos

        Teste 5:
            Elementos 100.000.000
            Remoção 50.000.000
            Quantidade de threads: 3
            Resultado: 233,505000 segundos / 3,89 minutos

        Teste 6:
            Elementos 1.000.000.000
            Remoção 500.000.000
            Quantidade de threads: 3
            Resultado: 2096,395000 segundos / 34,93 minutos

        Teste 7:
            Elementos 1.500.000.000
            Remoção 750.000.000
            Quantidade de threads: 3
            Resultado: 3167,911000 segundos / 52,79 minutos


    Observações acerca dos testes realizados:
        - Este programa utiliza partes de código dos livros:
        -   Weiss, Mark Allen. (2014). Data Structures and Algorithm Analysis in C++. (páginas 154-157).
        -   Butenhof, D. (1997). "Programming with POSIX Threads. Addison-Wesley Professional".
        - Este programa foi escrito com partes de código dos livros referenciados, e tiverama adaptações por minha parte, outros trechos foram criados do zero
        - Para realizar os testes e medir o uso de recursos foi implementado funções para o mesmo:
            - clock(); para medir o tempo de execução do programa
            - foram implementados também outras funções para medir o uso de recursos de memória e cpu, mas ficaram limitas no sistema windows e vieram a ser omitidas

*/

/
        *Bibliotecas utilizadas neste programa.
            * /
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

    /
        *Definição de variáveis para teste na main
            *Essa definição permite fazer os teste dentro da main
                * /
    int NUM_THREADS = 3;
/ < Número de treads * /
        int NUM_ELEMENTOS_ARVORE = 5;
/ < Número de elementos a serem inseridos na árvore * /
        int NUM_ELEMENTOS_ARVORE_PARA_REMOVER = 1;
/ < Número de elementos a serem removidos na árvore * /

        /
        *Definição da estrutura de dados AvlNode.
             *Essa definição permite referenciar a própria estrutura antes de sua implementação completa.
                 * /
        typedef struct AvlNode AvlNode;

struct AvlNode
{
    int elemento;
    AvlNode *esquerda;
    AvlNode *direita;
    int altura;
};

/
    *Estrutura de dados para os parâmetros da thread.
         *Armazena os dados necessários para cada thread.
             * /
    typedef struct ThreadData
{
    AvlNode arvore;
    / < Ponteiro para a raiz da árvore AVL * /
            int inicio;
    / < Índice de início para processamento * /
            int fim;
    / < Índice de fim para processamento * /
            pthread_mutex_t *mutex;
    / < Ponteiro para o mutex utilizado para sincronização * /
} ThreadData;

/
 * Retorna a altura de um nó da árvore AVL.
 *
 * @param t O nó para o qual a altura será calculada.
 * @return A altura do nó. Se o nó for nulo, retorna -1.
 */
int altura(AvlNode *t)
{
    return t == NULL ? -1 : t->altura;
}

/
    *Cria um novo nó da árvore AVL com o elemento fornecido,
    os nós filhos e a altura especificados.
            *
                *@param elem O elemento a ser armazenado no nó.
                    *@param esq O ponteiro para o nó filho esquerdo.
                        *@param dir O ponteiro para o nó filho direito.
                            *@param alt A altura do nó.
                                *@ return Um ponteiro para o novo nó criado.*
        @note A função aloca memória para o novo nó.Em caso de falha na alocação,
    a função imprime uma mensagem de erro e encerra o programa.
            * /
        AvlNode *novoAvlNode(const int elem, AvlNode *esq, AvlNode *dir, int alt)
{
    AvlNode *n = (AvlNode *)malloc(sizeof(AvlNode));
    if (n == NULL)
    {
        printf("Erro ao alocar memória\n");
        exit(1);
    }
    n->elemento = elem;
    n->esquerda = esq;
    n->direita = dir;
    n->altura = alt;
    return n;
}

/
    *Retorna o valor máximo entre dois números inteiros.
         *
             *@param a O primeiro número inteiro.
                 *@param b O segundo número inteiro.
                     *@ return O valor máximo entre os dois números fornecidos.*
    /
    int max(const int a, const int b)
{
    return a > b ? a : b;
}

/
    *Realiza a rotação com o filho esquerdo em uma árvore AVL.
         *
             *@param k2 O endereço do nó a ser rotacionado.
                 * /
    void rotacionarComFilhoEsquerdo(AvlNode k2)
{
    // Implementação da função rotacionarComFilhoEsquerdo partes do código do livro "Data Structures and Algorithm Analysis in C++." por Autor do Livro (página 156)

    AvlNode *k1 = (*k2)->esquerda;                                            // Armazena o filho esquerdo em k1
    (*k2)->esquerda = k1->direita;                                            // Atribui a subárvore direita de k1 como a nova subárvore esquerda de k2
    k1->direita = *k2;                                                        // Torna k2 a subárvore direita de k1
    (*k2)->altura = max(altura((*k2)->esquerda), altura((*k2)->direita)) + 1; // Atualiza a altura de k2
    k1->altura = max(altura(k1->esquerda), (*k2)->altura) + 1;                // Atualiza a altura de k1
    *k2 = k1;                                                                 // Atribui k1 como a nova raiz da subárvore
}

/
    *Realiza a rotação com o filho direito em uma árvore AVL.
         *
             *@param k2 O endereço do nó a ser rotacionado.
                 * /
    void rotacionarComFilhoDireito(AvlNode k2)
{
    // Implementação da função rotacionarComFilhoDireito partes do código do livro "Data Structures and Algorithm Analysis in C++." por Autor do Livro (página 156)

    AvlNode *k1 = (*k2)->direita;                                             // Armazena o filho direito em k1
    (*k2)->direita = k1->esquerda;                                            // Atribui a subárvore esquerda de k1 como a nova subárvore direita de k2
    k1->esquerda = *k2;                                                       // Torna k2 a subárvore esquerda de k1
    (*k2)->altura = max(altura((*k2)->esquerda), altura((*k2)->direita)) + 1; // Atualiza a altura de k2
    k1->altura = max(altura(k1->direita), (*k2)->altura) + 1;                 // Atualiza a altura de k1
    *k2 = k1;                                                                 // Atribui k1 como a nova raiz da subárvore
}

/
    *Realiza a dupla rotação com o filho esquerdo em uma árvore AVL.
         *
             *@param k3 O endereço do nó a ser rotacionado.
                 * /
    void duplaRotacaoComFilhoEsquerdo(AvlNode k3)
{
    rotacionarComFilhoDireito(&((*k3)->esquerda)); // Realiza uma rotação com filho direito na subárvore esquerda de k3
    rotacionarComFilhoEsquerdo(k3);                // Realiza uma rotação com filho esquerdo em k3
}

/
    *Realiza a dupla rotação com o filho direito em uma árvore AVL.
         *
             *@param k3 O endereço do nó a ser rotacionado.
                 * /
    void duplaRotacaoComFilhoDireito(AvlNode k3)
{
    rotacionarComFilhoEsquerdo(&((*k3)->direita)); // Realiza uma rotação com o filho esquerdo do filho direito de k3
    rotacionarComFilhoDireito(k3);                 // Realiza uma rotação com k3
}

/
    *Balanceia uma árvore AVL verificando e realizando as rotações necessárias.
         *
             *@param t O endereço da raiz da árvore a ser balanceada.
                 * /
    void balancear(AvlNode t)
{
    // Implementação da função balancear partes do código do livro "Data Structures and Algorithm Analysis in C++." por Autor do Livro (página 155)

    if (*t == NULL)
    {
        return;
    }

    // Verifica se a diferença de altura entre o filho esquerdo e o filho direito é maior que 1
    if (altura((*t)->esquerda) - altura((*t)->direita) > 1)
    {
        // Verifica se o filho esquerdo do nó possui uma altura maior ou igual ao filho direito
        if (altura((*t)->esquerda->esquerda) >= altura((*t)->esquerda->direita))
        {
            rotacionarComFilhoEsquerdo(t); // Realiza uma rotação simples com o filho esquerdo
        }
        else
        {
            duplaRotacaoComFilhoEsquerdo(t); // Realiza uma rotação dupla com o filho esquerdo
        }
    }
    // Verifica se a diferença de altura entre o filho direito e o filho esquerdo é maior que 1
    else if (altura((*t)->direita) - altura((*t)->esquerda) > 1)
    {
        // Verifica se o filho direito do nó possui uma altura maior ou igual ao filho esquerdo
        if (altura((*t)->direita->direita) >= altura((*t)->direita->esquerda))
        {
            rotacionarComFilhoDireito(t); // Realiza uma rotação simples com o filho direito
        }
        else
        {
            duplaRotacaoComFilhoDireito(t); // Realiza uma rotação dupla com o filho direito
        }
    }
}

/
    *Insere um elemento na árvore AVL.
         *
             *@param x O elemento a ser inserido.
                 *@param t O endereço da raiz da árvore onde o elemento será inserido.
                     * /
    void inserir(const int x, AvlNode t)
{
    // Implementação da função inserir partes do código do livro "Data Structures and Algorithm Analysis in C++." por Autor do Livro (página 155)

    // Verifica se a árvore está vazia
    if (*t == NULL)
    {
        *t = novoAvlNode(x, NULL, NULL, 0); // Cria um novo nó com o valor x
        return;
    }

    // Verifica se o valor a ser inserido é menor que o elemento atual do nó
    if (x < (*t)->elemento)
    {
        inserir(x, &((*t)->esquerda)); // Chama recursivamente a função para inserir o valor na subárvore esquerda
    }
    // Verifica se o valor a ser inserido é maior que o elemento atual do nó
    else if (x > (*t)->elemento)
    {
        inserir(x, &((*t)->direita)); // Chama recursivamente a função para inserir o valor na subárvore direita
    }
    // Caso contrário, o valor já existe na árvore, não faz nada e retorna
    else
    {
        return;
    }

    // Atualiza a altura do nó após a inserção
    (*t)->altura = max(altura((*t)->esquerda), altura((*t)->direita)) + 1;
    balancear(t); // Realiza o balanceamento da árvore
}

/
    *Função executada por uma thread para inserir elementos na árvore AVL.
         *
             *@param arg Um ponteiro para os dados da thread contendo a árvore,
    o intervalo de elementos e o mutex.
            *@ return NULL *
        /
        void * inserirThread(void *arg)
{
    ThreadData *data = (ThreadData *)arg; // Obtém os dados da thread
    AvlNode arvore = data->arvore;        // Ponteiro para a raiz da árvore
    int inicio = data->inicio;            // Valor inicial do intervalo de valores a serem inseridos
    int fim = data->fim;                  // Valor final do intervalo de valores a serem inseridos
    int i;

    // Itera pelo intervalo de valores definido para a thread
    for (i = inicio; i <= fim; i++)
    {
        int valor = rand() % ((fim - inicio + 1) * 10) + inicio * 10; // Gera um valor aleatório para inserção

        pthread_mutex_lock(data->mutex);   // Lock do mutex antes da inserção
        inserir(valor, arvore);            // Insere o valor na árvore
        pthread_mutex_unlock(data->mutex); // Unlock do mutex após a inserção
    }

    pthread_exit(NULL); // Finaliza a thread
}

/
    *Encontra o nó com o valor mínimo na árvore AVL.
         *
             *@param t O nó raiz da árvore onde a busca será realizada.
                 *@ return O ponteiro para o nó com o valor mínimo,
    ou NULL se a árvore estiver vazia.
            * /
        AvlNode *EncontrarMinNode(AvlNode *t)
{
    if (t == NULL)
    {
        return NULL; // Retorna NULL se a árvore estiver vazia
    }

    if (t->esquerda == NULL)
    {
        return t; // Retorna o próprio nó se não houver filho esquerdo
    }

    return EncontrarMinNode(t->esquerda); // Recursivamente busca o filho esquerdo até encontrar o nó com o menor valor
}

/
    *Encontra o nó com o valor máximo na árvore AVL.
         *
             *@param t O nó raiz da árvore onde a busca será realizada.
                 *@ return O ponteiro para o nó com o valor máximo,
    ou NULL se a árvore estiver vazia.
            * /
        AvlNode *EncontrarMaxNode(AvlNode *t)
{
    if (t == NULL)
    {
        return NULL;
    }
    if (t->direita == NULL)
    {
        return t; // Retorna o próprio nó se não houver filho direito
    }
    else
    {
        return EncontrarMaxNode(t->direita);
    }
}

/
    *Remove um nó com o valor especificado da árvore AVL.
         *
             *@param x O valor a ser removido.
                 *@param t O ponteiro para o nó raiz da árvore.
                     *@param removerElemento O ponteiro para a variável onde o elemento removido será armazenado.
                         * /
    void removerNode(const int x, AvlNode t, int *removerElemento)
{
    // Implementação da função removerNode partes do código do livro "Data Structures and Algorithm Analysis in C++." por Autor do Livro (página 157)

    if (*t == NULL)
    {
        return; // Retorna se a árvore estiver vazia
    }

    if (x < (*t)->elemento)
    {
        removerNode(x, &((*t)->esquerda), removerElemento); // Chama a função recursivamente para a subárvore esquerda
    }
    else if (x > (*t)->elemento)
    {
        removerNode(x, &((*t)->direita), removerElemento); // Chama a função recursivamente para a subárvore direita
    }
    else if ((*t)->esquerda != NULL && (*t)->direita != NULL)
    {
        // Nó a ser removido tem dois filhos
        AvlNode *nodeParaRemover = *t;
        AvlNode *minNode = EncontrarMinNode(nodeParaRemover->direita);  // Encontra o nó com o menor valor na subárvore direita
        *removerElemento = minNode->elemento;                           // Armazena o elemento a ser removido
        (*t)->elemento = minNode->elemento;                             // Substitui o elemento do nó atual pelo elemento do nó encontrado
        removerNode((*t)->elemento, &((*t)->direita), removerElemento); // Remove o nó encontrado recursivamente
    }
    else
    {
        // Nó a ser removido tem zero ou um filho
        AvlNode *nodeParaRemover = *t;
        *t = ((*t)->esquerda != NULL) ? (*t)->esquerda : (*t)->direita; // Substitui o nó atual pelo seu filho (se existir)
        *removerElemento = nodeParaRemover->elemento;                   // Armazena o elemento a ser removido
        free(nodeParaRemover);                                          // Libera a memória do nó removido
    }

    // Atualiza a altura do nó pai e realiza o balanceamento, se necessário
    if (*t != NULL)
    {
        (*t)->altura = max(altura((*t)->esquerda), altura((*t)->direita)) + 1;
        balancear(t);
    }
}

/
    *Imprime os elementos da árvore AVL em ordem crescente.
         *
             *@param t O ponteiro para o nó raiz da árvore.
                 * /
    void printArvoreEmOrdem(AvlNode *t)
{
    if (t != NULL)
    {
        printArvoreEmOrdem(t->esquerda); // Recursivamente imprime os elementos da subárvore esquerda.
        printf("%d ", t->elemento);      // Imprime o elemento do nó atual.
        printArvoreEmOrdem(t->direita);  // Recursivamente imprime os elementos da subárvore direita.
    }
}

/
        *Imprime os elementos da árvore AVL em pré -
    ordem.
            *
                *@param t O ponteiro para o nó raiz da árvore.
                    * /
        void printArvoreEmPreOrdem(AvlNode *t)
{
    if (t != NULL)
    {
        printf("%d ", t->elemento);         // Imprime o elemento do nó atual.
        printArvoreEmPreOrdem(t->esquerda); // Recursivamente imprime os elementos da subárvore esquerda.
        printArvoreEmPreOrdem(t->direita);  // Recursivamente imprime os elementos da subárvore direita.
    }
}

/
        *Imprime os elementos da árvore AVL em pós -
    ordem.
            *
                *@param t O ponteiro para o nó raiz da árvore.
                    * /
        void printArvoreEmPosOrdem(AvlNode *t)
{
    if (t != NULL)
    {
        printArvoreEmPosOrdem(t->esquerda); // Recursivamente imprime os elementos da subárvore esquerda.
        printArvoreEmPosOrdem(t->direita);  // Recursivamente imprime os elementos da subárvore direita.
        printf("%d ", t->elemento);         // Imprime o elemento do nó atual.
    }
}

/
    *Imprime o sucessor e o predecessor de um elemento na árvore AVL.
         *
             *@param x O elemento para o qual se deseja encontrar o sucessor e o predecessor.
                 *@param t O ponteiro para o nó raiz da árvore.
                     * /
    void printSucessorEPredecessor(const int x, AvlNode *t)
{
    AvlNode *successor = NULL;   // Ponteiro para o sucessor
    AvlNode *predecessor = NULL; // Ponteiro para o predecessor
    AvlNode *current = t;        // Ponteiro para o nó atual

    // Percorre a árvore para encontrar o elemento e seus sucessor e predecessor
    while (current != NULL)
    {
        if (x < current->elemento)
        {
            successor = current;
            current = current->esquerda; // Desce para a subárvore esquerda
        }
        else if (x > current->elemento)
        {
            predecessor = current;
            current = current->direita; // Desce para a subárvore direita
        }
        else
        {
            // Elemento encontrado, encontrando o sucessor e o predecessor se existirem
            if (current->direita != NULL)
            {
                successor = EncontrarMinNode(current->direita); // Encontra o nó mínimo da subárvore direita
            }
            if (current->esquerda != NULL)
            {
                predecessor = EncontrarMaxNode(current->esquerda); // Encontra o nó máximo da subárvore esquerda
            }
            break;
        }
    }

    // Imprime o sucessor, se existir
    if (successor != NULL)
    {
        printf("Sucessor de %d: %d\n", x, successor->elemento);
    }
    else
    {
        printf("Não há sucessor para %d\n", x);
    }

    // Imprime o predecessor, se existir
    if (predecessor != NULL)
    {
        printf("Predecessor de %d: %d\n", x, predecessor->elemento);
    }
    else
    {
        printf("Não há predecessor para %d\n", x);
    }
}

/
    *Imprime o elemento mínimo e máximo de uma árvore AVL.
         *
             *@param t O ponteiro para o nó raiz da árvore.
                 * /
    void printMinMax(AvlNode *t)
{
    AvlNode *minNode = EncontrarMinNode(t); // Encontra o nó mínimo da árvore
    AvlNode *maxNode = EncontrarMaxNode(t); // Encontra o nó máximo da árvore

    // Imprime o elemento mínimo, se existir
    if (minNode != NULL)
    {
        printf("Elemento mínimo: %d\n", minNode->elemento);
    }
    else
    {
        printf("A árvore está vazia. Não há elemento mínimo.\n");
    }

    // Imprime o elemento máximo, se existir
    if (maxNode != NULL)
    {
        printf("Elemento máximo: %d\n", maxNode->elemento);
    }
    else
    {
        printf("A árvore está vazia. Não há elemento máximo.\n");
    }
}

/
    *Função executada por uma thread para remover elementos de uma árvore AVL.
         *
             *@param arg O argumento passado para a thread,
    que contém os dados da thread.
            *@ return Nenhum valor de retorno.*
        /
        void * removerThread(void *arg)
{
    ThreadData *data = (ThreadData *)arg; // Obtém os dados da thread
    AvlNode arvore = data->arvore;        // Ponteiro para a árvore
    int inicio = data->inicio;            // Índice de início
    int fim = data->fim;                  // Índice de fim
    pthread_mutex_t *mutex = data->mutex; // Ponteiro para o mutex

    for (int i = inicio; i <= fim; i++)
    {
        int valor = rand() % (NUM_ELEMENTOS_ARVORE_PARA_REMOVER * 10); // Gera um valor aleatório para remover
        int removerElemento = -1;

        pthread_mutex_lock(mutex); // Lock do mutex antes da remoção
        removerNode(valor, arvore, &removerElemento);
        pthread_mutex_unlock(mutex); // Unlock do mutex após a remoção

        if (removerElemento != -1)
        {
            printf("Elemento removido: %d\n", removerElemento);
        }
        // else {
        // printf("Elemento não encontrado: %d\n", valor);
        //}
    }

    pthread_exit(NULL);
}

/
    *Função principal do programa.
         *
             *@ return 0 valor indicando o status de saída do programa.*
    /
    int main()
{

    // Declaração das variáveis para medição do tempo
    clock_t iniciar_tempo, fim_tempo;
    double tempo_total;

    // Inicia o tempo
    iniciar_tempo = clock();

    // variável para for
    int i;

    // Configura a localização para imprimir números e caracteres acentuados com formatação correta
    setlocale(LC_ALL, "Portuguese");

    // Cria a raiz da árvore AVL
    AvlNode *raiz = NULL;

    // Cria um array de threads
    pthread_t threads[NUM_THREADS];

    // Cria um array de dados das threads
    ThreadData thread_data[NUM_THREADS];

    // Cria um mutex para garantir exclusão m?tua durante as inserções
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    // Calcula o número de elementos por thread
    int elementos_por_thread = NUM_ELEMENTOS_ARVORE / NUM_THREADS;

    // Inicializa o gerador de n?meros aleat?rios
    srand(time(NULL));

    // Cria as threads para inserção paralela
    for (i = 0; i < NUM_THREADS; i++)
    {
        // Define o intervalo de elementos para a thread
        int inicio = i * elementos_por_thread;
        int fim = inicio + elementos_por_thread - 1;

        // Define os dados da thread
        thread_data[i].arvore = &raiz;
        thread_data[i].inicio = inicio;
        thread_data[i].fim = fim;
        thread_data[i].mutex = &mutex;

        // Cria a thread
        pthread_create(&threads[i], NULL, inserirThread, (void *)&thread_data[i]);
    }
    // Aguarda as threads terminarem
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Imprime a árvore em ordem crescente
    printf("?rvore AVL em ordem crescente:\n");
    printArvoreEmOrdem(raiz);
    printf("\n");

    // Imprime a árvore em pré-ordem
    printf("árvore AVL em pré-ordem: \n");
    printArvoreEmPreOrdem(raiz);
    printf("\n");

    // Imprime a árvore em pós-ordem
    printf("árvore AVL em pós-ordem: \n");
    printArvoreEmPosOrdem(raiz);
    printf("\n");

    // Cria as threads para remoção paralela
    for (i = 0; i < NUM_THREADS; i++)
    {
        // Define o intervalo de elementos para a thread
        int inicio = i * elementos_por_thread;
        int fim = inicio + elementos_por_thread - 1;

        // Define os dados da thread
        thread_data[i].arvore = &raiz;
        thread_data[i].inicio = inicio;
        thread_data[i].fim = fim;
        thread_data[i].mutex = &mutex;

        // Cria a thread
        pthread_create(&threads[i], NULL, removerThread, (void *)&thread_data[i]);
    }

    // Aguarda as threads terminarem
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Imprime a árvore novamente após as remoções
    printf("Árvore AVL em ordem crescente após remoções:\n");
    printf("\n");
    printArvoreEmOrdem(raiz);
    printf("\n");

    // Gerar números aleatórios para encontrar sucessor e predecessor
    int num_aleatorio_para_sucessor = 1;
    for (i = 0; i < num_aleatorio_para_sucessor; i++)
    {
        // Gerar um número aleatório entre 1 e 100
        int valor = rand() % 100 + 1;

        printf("\n");
        // Encontra e imprimi o sucessor e o predecessor do valor aleatório na árvore AVL
        printSucessorEPredecessor(valor, raiz);
        printf("\n");
    }

    // Imprimir o elemento mínimo e máximo da árvore AVL
    printf("\n");
    printMinMax(raiz);
    printf("\n");

    // Libera o mutex
    pthread_mutex_destroy(&mutex);

    // Atribui NULL à raiz para liberar a memória da árvore AVL.
    raiz = NULL;

    // Finaliza o tempo e calcula o tempo total
    // O tempo total de execução é calculado pela diferença entre o tempo final e o tempo inicial,
    // dividido pelo valor da constante CLOCKS_PER_SEC, que representa o número de clock ticks por segundo.
    fim_tempo = clock();
    tempo_total = ((double)(fim_tempo - iniciar_tempo)) / CLOCKS_PER_SEC;

    // Imprime o tempo total de execução
    // O resultado é armazenado na variável tempo_total.
    printf("Tempo total de execução: %f segundos\n", tempo_total);
    printf("\n");

    // Retorna 0 para indicar o término do programa
    return 0;
}