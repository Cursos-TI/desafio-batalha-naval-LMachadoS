#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h> 

// --- Constantes Globais ---
#define TAMANHO_TABULEIRO 10
#define TAMANHO_HABILIDADE 7 // Tamanho das matrizes de habilidade (ex: 7x7)
#define CENTRO_HABILIDADE (TAMANHO_HABILIDADE / 2) // Índice do centro (3 para 7x7)
#define NAVIOS_POSICIONADOS 4

// --- Códigos de Representação ---
#define AGUA 0
#define NAVIO 3
#define AFETADO 5 // Novo código para área afetada pela habilidade

// --- Tipos de Orientacao ---
typedef enum {
    HORIZONTAL,
    VERTICAL,
    DIAGONAL_PRINCIPAL, 
    DIAGONAL_SECUNDARIA 
} Orientacao;

// --- Estrutura para os Navios  ---
typedef struct {
    int l_i, c_i;
    Orientacao orientacao;
} Navio;

// --- Protótipos das Funções ---
void inicializar_tabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
void posicionar_navio(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], int l_i, int c_i, int tamanho, Orientacao orientacao);
void construir_habilidade_cone(int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE]);
void construir_habilidade_cruz(int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE]);
void construir_habilidade_octaedro(int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE]);
void aplicar_habilidade(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], 
                        int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE], 
                        int l_origem, int c_origem);
void exibir_tabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]);
// (A função posicao_valida foi omitida para simplificar o escopo, assumindo posições válidas)

void inicializar_tabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    for (int l = 0; l < TAMANHO_TABULEIRO; l++) {
        for (int c = 0; c < TAMANHO_TABULEIRO; c++) {
            tabuleiro[l][c] = AGUA;
        }
    }
}

// Funcao para posicionar um navio (simplificada)
void posicionar_navio(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], 
                      int l_i, int c_i, int tamanho, Orientacao orientacao) {

    int dl = 0; 
    int dc = 0; 

    switch (orientacao) {
        case HORIZONTAL: dc = 1; break;
        case VERTICAL: dl = 1; break;
        case DIAGONAL_PRINCIPAL: dl = 1; dc = 1; break;
        case DIAGONAL_SECUNDARIA: dl = 1; dc = -1; break;
    }

    for (int k = 0; k < tamanho; k++) {
        int l_atual = l_i + k * dl;
        int c_atual = c_i + k * dc;
        
        // Garante que o navio esteja dentro do limite (Embora a validação fosse ideal)
        if (l_atual >= 0 && l_atual < TAMANHO_TABULEIRO && c_atual >= 0 && c_atual < TAMANHO_TABULEIRO) {
             tabuleiro[l_atual][c_atual] = NAVIO;
        }
    }
}


/**
 * Constrói a matriz de habilidade CONE (apontando para baixo) usando condicionais.
 * Exemplo (7x7): 1 linha de topo, 3 linhas no meio, 5 linhas na base.
 */
void construir_habilidade_cone(int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE]) {
    int centro = CENTRO_HABILIDADE;
    for (int l = 0; l < TAMANHO_HABILIDADE; l++) {
        for (int c = 0; c < TAMANHO_HABILIDADE; c++) {
            
            // Distância lateral do centro
            int dist_coluna = abs(c - centro); 
            
            // Lógica do Cone (quanto mais para baixo, mais largo é o efeito)
            // A largura do cone é definida pela linha (l)
            // A matriz 7x7 terá largura: 1 (l=0), 3 (l=1), 5 (l=2), 7 (l=3)
            // Para um cone mais "pontiagudo", usamos l/2.
            
            if (dist_coluna <= l) { // Se a distância lateral for menor ou igual à linha (l)
                matriz_habilidade[l][c] = 1; 
            } else {
                matriz_habilidade[l][c] = 0;
            }
        }
    }
}

/**
 * Constrói a matriz de habilidade CRUZ (centrada) usando condicionais.
 */
void construir_habilidade_cruz(int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE]) {
    int centro = CENTRO_HABILIDADE;
    for (int l = 0; l < TAMANHO_HABILIDADE; l++) {
        for (int c = 0; c < TAMANHO_HABILIDADE; c++) {
            
            // Posições afetadas: a linha ou a coluna é igual ao centro
            if (l == centro || c == centro) {
                matriz_habilidade[l][c] = 1; // Afetado
            } else {
                matriz_habilidade[l][c] = 0; // Não afetado
            }
        }
    }
}

/**
 * Constrói a matriz de habilidade OCTAEDRO (losango) usando condicionais.
 * A forma é definida pela "distância de Manhattan" do centro.
 * Distância de Manhattan = |l - centro| + |c - centro|
 */
void construir_habilidade_octaedro(int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE]) {
    int centro = CENTRO_HABILIDADE;
    // O raio da forma de losango (metade da matriz - 1)
    int raio = centro; 

    for (int l = 0; l < TAMANHO_HABILIDADE; l++) {
        for (int c = 0; c < TAMANHO_HABILIDADE; c++) {
            
            // Distância de Manhattan: Soma da distância vertical e horizontal do centro.
            int dist_manhattan = abs(l - centro) + abs(c - centro);

            // A posição é afetada se a distância de Manhattan for menor ou igual ao raio.
            if (dist_manhattan <= raio) { 
                matriz_habilidade[l][c] = 1; // Afetado
            } else {
                matriz_habilidade[l][c] = 0; // Não afetado
            }
        }
    }
}

/**
 * Sobrepõe a matriz de habilidade ao tabuleiro principal.
 * Garante que a aplicação fique dentro dos limites do tabuleiro.

 */
void aplicar_habilidade(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO], 
                        int matriz_habilidade[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE], 
                        int l_origem, int c_origem) {
    
    int offset = CENTRO_HABILIDADE;
    
    printf("\nAplicando habilidade com ponto de origem em (%c%d)...\n", 'A' + c_origem, l_origem);

    for (int l_habil = 0; l_habil < TAMANHO_HABILIDADE; l_habil++) {
        for (int c_habil = 0; c_habil < TAMANHO_HABILIDADE; c_habil++) {
            
            // Verifica se a posição na matriz de habilidade está marcada (valor 1)
            if (matriz_habilidade[l_habil][c_habil] == 1) {
                
                // Calcula as coordenadas correspondentes no tabuleiro principal
                int l_tabuleiro = l_origem + (l_habil - offset);
                int c_tabuleiro = c_origem + (c_habil - offset);
                
                // Condicional de Limite: Garante que a aplicação não saia do tabuleiro 10x10
                if (l_tabuleiro >= 0 && l_tabuleiro < TAMANHO_TABULEIRO &&
                    c_tabuleiro >= 0 && c_tabuleiro < TAMANHO_TABULEIRO) {
                    
                    // Condicional de Marcação: Apenas marca se não for um navio (3)
                    // Se a posição já for um navio, mantemos o valor 3 para visualização
                    if (tabuleiro[l_tabuleiro][c_tabuleiro] != NAVIO) {
                        tabuleiro[l_tabuleiro][c_tabuleiro] = AFETADO;
                    }
                }
            }
        }
    }
}

/**
 * Exibe o tabuleiro com representação visual.
 */
void exibir_tabuleiro(int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO]) {
    printf("\n Tabuleiro Batalha Naval - Visao de Habilidade \n");

    // Cabecalho das colunas (A a J)
    printf("   "); 
    for (int c = 0; c < TAMANHO_TABULEIRO; c++) {
        printf("%c ", 'A' + c); 
    }
    printf("\n");

    // Imprime a matriz
    for (int l = 0; l < TAMANHO_TABULEIRO; l++) {
        printf("%d |", l); 

        for (int c = 0; c < TAMANHO_TABULEIRO; c++) {
            // Usa caracteres diferentes para visualização
            if (tabuleiro[l][c] == AGUA) {
                printf("~ "); // Água (0)
            } else if (tabuleiro[l][c] == NAVIO) {
                printf("N "); // Navio (3)
            } else if (tabuleiro[l][c] == AFETADO) {
                printf("X "); // Área Afetada (5)
            } else {
                printf("? "); // Fallback
            }
        }
        printf("\n");
    }
    printf("---------------------------------------------------\n");
}

int main() {
    int tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
    
    // Matrizes temporárias para as habilidades (7x7)
    int habilidade_cone[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE] = {0};
    int habilidade_cruz[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE] = {0};
    int habilidade_octaedro[TAMANHO_HABILIDADE][TAMANHO_HABILIDADE] = {0};
    
    // Navios do nível anterior (para ter algo no tabuleiro)
    Navio navios[NAVIOS_POSICIONADOS] = {
        {2, 1, HORIZONTAL}, // B2-D2
        {5, 8, VERTICAL},   // I5-I7
        {6, 0, DIAGONAL_PRINCIPAL}, // A6-C8
        {0, 9, DIAGONAL_SECUNDARIA} // J0-H2
    };

    // --- Configuração Inicial ---
    inicializar_tabuleiro(tabuleiro);
    for (int i = 0; i < NAVIOS_POSICIONADOS; i++) {
        posicionar_navio(tabuleiro, navios[i].l_i, navios[i].c_i, 3, navios[i].orientacao);
    }
    printf("Navios posicionados (N). Tabuleiro pronto.\n");

    // --- 1. Teste de Habilidade CONE ---
    construir_habilidade_cone(habilidade_cone);
    // Ponto de origem: Linha 2, Coluna 5 (F2). Centrar o topo em F2.
    // O cone aponta para baixo.
    aplicar_habilidade(tabuleiro, habilidade_cone, 2, 5);
    exibir_tabuleiro(tabuleiro);

    // Reinicializa o tabuleiro para o próximo teste
    inicializar_tabuleiro(tabuleiro);
    for (int i = 0; i < NAVIOS_POSICIONADOS; i++) {
        posicionar_navio(tabuleiro, navios[i].l_i, navios[i].c_i, 3, navios[i].orientacao);
    }
    
    // --- 2. Teste de Habilidade CRUZ ---
    construir_habilidade_cruz(habilidade_cruz);
    // Ponto de origem: Linha 7, Coluna 2 (C7).
    aplicar_habilidade(tabuleiro, habilidade_cruz, 7, 2);
    exibir_tabuleiro(tabuleiro);

    // Reinicializa o tabuleiro para o próximo teste
    inicializar_tabuleiro(tabuleiro);
    for (int i = 0; i < NAVIOS_POSICIONADOS; i++) {
        posicionar_navio(tabuleiro, navios[i].l_i, navios[i].c_i, 3, navios[i].orientacao);
    }
    
    // --- 3. Teste de Habilidade OCTAEDRO (Losango) ---
    construir_habilidade_octaedro(habilidade_octaedro);
    // Ponto de origem: Linha 4, Coluna 4 (E4). Posição central.
    aplicar_habilidade(tabuleiro, habilidade_octaedro, 4, 4);
    exibir_tabuleiro(tabuleiro);

    return 0;
}