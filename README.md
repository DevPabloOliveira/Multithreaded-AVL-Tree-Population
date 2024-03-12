# Multithreaded AVL Tree Population

Este é um programa em C que demonstra o uso de uma árvore AVL com inserção e remoção de elementos. O algoritmo é otimizado para lidar com grandes quantidades de inserções e remoções por meio de programação paralela usando threads. Ele implementa um conjunto de testes para analisar a escalabilidade e a adaptabilidade do código em relação à carga de trabalho.

# Funcionalidades

- Inserção Paralela: Utiliza threads para inserir elementos na árvore AVL, aproveitando a programação paralela para otimizar o desempenho.
- Remoção Paralela: Realiza a remoção de elementos na árvore AVL usando threads, garantindo exclusão mútua com mutex.
- Balanceamento Automático: Mantém a árvore AVL balanceada após inserções e remoções, garantindo que as operações tenham complexidade de tempo logarítmica.
- Identificação de Sucessor e Predecessor: Encontra e imprime o sucessor e o predecessor de um elemento na árvore AVL.

# Testes

O programa inclui uma série de testes realizados para diferentes tamanhos de árvore e número de threads. Os testes são executados para analisar o tempo de execução em cenários variados.

# Uso

Para compilar e executar o programa, siga as instruções abaixo:

    gcc -o Multithreaded_AVL_Tree_Population Multithreaded_AVL_Tree_Population.c -lpthread ./Multithreaded_AVL_Tree_Population

# Requisitos

- Compilador GCC
- Biblioteca pthread.h

# Autor

- Pablo Oliveira

# Histórico de Revisões

- Versão 1.0

  - Implementação inicial do código.

- Versão 1.1
  - Identificação de restrições de capacidade e limitações de recursos.
- Versão 1.2

  - Implementação de programação paralela com o uso de threads para otimização.

- Versão 1.3
  - Melhoria na eficiência do algoritmo com o uso de mutex para exclusão mútua.

Este programa é baseado em partes de código dos livros "Data Structures and Algorithm Analysis in C++" e "Programming with POSIX Threads". Algumas partes foram adaptadas e outras criadas do zero para atender às necessidades específicas do problema abordado.

# Observações

O código foi testado e otimizado para desempenho em diferentes cenários. No entanto, é importante considerar que o desempenho real pode variar dependendo do ambiente de execução e do hardware utilizado.

Para mais detalhes sobre o funcionamento do programa e sua implementação, consulte os comentários no código-fonte.
