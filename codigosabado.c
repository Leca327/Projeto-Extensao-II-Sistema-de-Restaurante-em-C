#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <string.h>

// Define o nome do arquivo onde as vendas serão salvas
#define ARQUIVO_VENDAS "vendas.dat"
#define NUM_MESES 12 // Define o número de meses para o array de vendas mensais

// Definição da estrutura para representar a data
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

// Definição da estrutura para o item de venda
typedef struct Venda {
    float preco;
    float peso_refeicao; // Peso da refeição (se aplicável)
    int qtd_bebida;      // Quantidade de bebidas
    Data dataVenda;
    // O ponteiro 'proximo' não é salvo no arquivo, apenas os dados para recriar a lista
    struct Venda *proximo;
} Venda;

// *** NOVA ADIÇÃO: Definição do enum para tipos de bebida ***
typedef enum {
    SAIR_BEBIDAS = 0,
    AGUA_MINERAL = 1,
    REFRIGERANTE_LATA = 2,
    SUCO_NATURAL = 3,
    CERVEJA_LATA = 4
} TipoBebida;

// *** NOVA ESTRUTURA para armazenar mês e valor para ordenação ***
typedef struct {
    int mes;
    float totalVendido;
} VendaMes;

// Função auxiliar para verificar se um ano é bissexto
bool ehBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

// Função para validar uma data (dia, mês, ano)
bool validarData(int dia, int mes, int ano) {
    // Valida o ano
    if (ano < 1900 || ano > 2100) { // Exemplo de faixa de ano, pode ser ajustado
        printf("Ano inválido. O ano deve estar entre 1900 e 2100.\n");
        return false;
    }

    // Valida o mês
    if (mes < 1 || mes > 12) {
        printf("Mês inválido. O mês deve estar entre 1 e 12.\n");
        return false;
    }

    // Valida o dia com base no mês e ano (para anos bissextos)
    int diasNoMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // Índice 0 não usado

    if (mes == 2) { // Fevereiro
        if (ehBissexto(ano)) {
            diasNoMes[2] = 29;
        }
    }

    if (dia < 1 || dia > diasNoMes[mes]) {
        printf("Dia inválido para o mês e ano fornecidos.\n");
        return false;
    }

    return true; // Data é válida
}


// Função para criar um novo nó de venda
Venda* criarVenda(float preco_base, float peso_refeicao, int qtd_bebida, int dia, int mes, int ano) {
    Venda* novaVenda = (Venda*) malloc(sizeof(Venda));
    if (novaVenda == NULL) {
        perror("Erro ao alocar memória para nova venda");
        exit(EXIT_FAILURE);
    }
    novaVenda->preco = preco_base;
    novaVenda->peso_refeicao = peso_refeicao;
    novaVenda->qtd_bebida = qtd_bebida;
    novaVenda->dataVenda.dia = dia;
    novaVenda->dataVenda.mes = mes;
    novaVenda->dataVenda.ano = ano;
    novaVenda->proximo = NULL;
    return novaVenda;
}

// Função para inserir uma nova venda no final da lista encadeada
// *** MODIFICADO: Agora recebe o array de vendas mensais para atualização ***
void inserirVenda(Venda** cabeca, float preco_base, float peso_refeicao, int qtd_bebida, int dia, int mes, int ano, float vendasMensais[]) {
    Venda* novaVenda = criarVenda(preco_base, peso_refeicao, qtd_bebida, dia, mes, ano);
    if (novaVenda == NULL) return; // Erro já tratado em criarVenda

    if (*cabeca == NULL) {
        *cabeca = novaVenda;
    } else {
        Venda* atual = *cabeca;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = novaVenda;
    }

    // *** ATUALIZA O ARRAY DE VENDAS MENSAIS ***
    // Garante que o mês é válido (1-12) e ajusta para índice de array (0-11)
    if (mes >= 1 && mes <= NUM_MESES) {
        vendasMensais[mes - 1] += preco_base;
    }
}

// Função para cadastrar o valor da refeição e obter o peso
float cadastrarValorRefeicao(float* peso) {
    char tipo_refeicao[10];
    float preco = 0.0;

    while (1) {
        printf("A refeição é quentinha? (sim/nao): ");
        scanf("%s", tipo_refeicao);
        // Limpar buffer para evitar problemas com entradas futuras
        while (getchar() != '\n');

        if (strcmp(tipo_refeicao, "sim") == 0) {
            preco = 20.00;
            printf("Valor atual da venda: R$ %.2f\n", preco);
            char precisa_embalagem[4];
            while (1) {
                printf("Precisa de embalagem? (sim/nao): ");
                scanf("%s", precisa_embalagem);
                while (getchar() != '\n');
                if (strcmp(precisa_embalagem, "sim") == 0) {
                    preco += 0.50;
                    printf("Valor atual da venda: R$ %.2f\n", preco);
                    *peso = 0.0; // Refeição quentinha não tem peso associado para cálculo
                    return preco;
                } else if (strcmp(precisa_embalagem, "nao") == 0) {
                    *peso = 0.0;
                    return preco;
                } else {
                    printf("Resposta inválida. Digite 'sim' ou 'nao'.\n");
                }
            }
        } else if (strcmp(tipo_refeicao, "nao") == 0) {
            float peso_digitado;
            printf("Digite o peso da refeição em gramas: ");
            if (scanf("%f", &peso_digitado) != 1 || peso_digitado < 0) {
                printf("Entrada de peso inválida. O peso deve ser um número positivo.\n");
                while (getchar() != '\n');
                continue; // Volta para o início do loop para nova entrada
            }
            while (getchar() != '\n');
            *peso = peso_digitado;
            preco = (peso_digitado / 100.0) * 4.99; // Preço por 100g
            printf("Valor atual da venda: R$ %.2f (baseado no peso)\n", preco);
            return preco;
        } else {
            printf("Opção inválida. Digite 'sim' ou 'nao'.\n");
        }
    }
}

// Função para adicionar bebidas usando switch
void adicionarBebidas(float* preco_total, int* qtd_total_bebidas) {
    printf("\n--- Bebidas Disponíveis ---\n");
    printf("[%d] Água Mineral - R$ 2.00\n", AGUA_MINERAL);
    printf("[%d] Refrigerante Lata - R$ 4.50\n", REFRIGERANTE_LATA);
    printf("[%d] Suco Natural - R$ 5.00\n", SUCO_NATURAL);
    printf("[%d] Cerveja Lata - R$ 6.00\n", CERVEJA_LATA);
    printf("[%d] Sair da seleção de bebidas\n", SAIR_BEBIDAS);
    printf("-----------------------------\n");

    int codigo_bebida_int; // Lê como int para compatibilidade com scanf
    TipoBebida codigo_bebida; // Usará o enum após a leitura

    int quantidade;
    char adicionar_mais[10];

    while (1) {
        printf("Digite o código da bebida que deseja adicionar (ou %d para sair): ", SAIR_BEBIDAS);
        if (scanf("%d", &codigo_bebida_int) != 1) {
            printf("Código inválido. Digite um número.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        // Converte o int lido para o tipo enum
        codigo_bebida = (TipoBebida)codigo_bebida_int;

        if (codigo_bebida == SAIR_BEBIDAS) {
            break;
        }

        float preco_unitario = 0.0;
        char nome_bebida[50] = "";

        switch (codigo_bebida) {
            case AGUA_MINERAL:
                strcpy(nome_bebida, "Água Mineral");
                preco_unitario = 2.00;
                break;
            case REFRIGERANTE_LATA:
                strcpy(nome_bebida, "Refrigerante Lata");
                preco_unitario = 4.50;
                break;
            case SUCO_NATURAL:
                strcpy(nome_bebida, "Suco Natural");
                preco_unitario = 5.00;
                break;
            case CERVEJA_LATA:
                strcpy(nome_bebida, "Cerveja Lata");
                preco_unitario = 6.00;
                break;
            default: // Caso um número fora do enum seja digitado
                printf("Código de bebida inválido. Tente novamente.\n");
                continue;
        }
        
        // Loop para garantir que a quantidade seja um número positivo
        while (1) { 
            printf("Digite a quantidade de %s: ", nome_bebida);
            if (scanf("%d", &quantidade) != 1 || quantidade <= 0) {
                printf("Quantidade inválida. Digite um número positivo.\n");
                while (getchar() != '\n'); // Limpa o buffer de entrada
            } else {
                while (getchar() != '\n'); // Limpa o buffer de entrada
                break; // Sai do loop de quantidade se for válida
            }
        }
        
        *preco_total += quantidade * preco_unitario;
        *qtd_total_bebidas += quantidade;
        printf("Valor atual da venda: R$ %.2f\n", *preco_total);

        printf("Deseja adicionar mais alguma bebida? (sim/nao): ");
        scanf("%s", adicionar_mais);
        while (getchar() != '\n');
        if (strcmp(adicionar_mais, "nao") == 0) {
            break;
        } else if (strcmp(adicionar_mais, "sim") != 0) {
            printf("Resposta inválida. Digite 'sim' ou 'nao'.\n");
        }
    }
}


void imprimirVendas(Venda* cabeca) {
    Venda* atual = cabeca;
    printf("--- Relatório de Vendas (Geral) ---\n");
    int compra_numero = 1;
    if (atual == NULL) {
        printf("Nenhuma venda registrada até o momento.\n");
        printf("-----------------------------------\n");
        return;
    }
    float total_geral = 0.0;
    while (atual != NULL) {
        printf("Compra %d: Preço Total: R$ %.2f", compra_numero++, atual->preco);
        if (atual->peso_refeicao > 0) {
            printf(", Peso da Refeição: %.2f gramas", atual->peso_refeicao);
        }
        if (atual->qtd_bebida > 0) {
            printf(", Quantidade de Bebidas: %d", atual->qtd_bebida);
        }
        printf(" (Data: %02d/%02d/%d)\n",
               atual->dataVenda.dia, atual->dataVenda.mes, atual->dataVenda.ano);
        total_geral += atual->preco;
        atual = atual->proximo;
    }
    printf("Total Geral de Vendas: R$ %.2f\n", total_geral);
    printf("-----------------------------------\n");
}


void gerarRelatorioDiario(Venda* cabeca, int dia, int mes, int ano) {
    printf("\n--- Relatório Diário (%02d/%02d/%d) ---\n", dia, mes, ano);
    Venda* atual = cabeca;
    int compra_numero = 1;
    bool venda_encontrada = false;
    float total_diario = 0.0;

    while (atual != NULL) {

        if (atual->dataVenda.dia == dia &&
            atual->dataVenda.mes == mes &&
            atual->dataVenda.ano == ano) {
            printf("Compra %d: Preço Total: R$ %.2f", compra_numero++, atual->preco);
            if (atual->peso_refeicao > 0) {
                printf(", Peso da Refeição: %.2f gramas", atual->peso_refeicao);
            }
            if (atual->qtd_bebida > 0) {
                printf(", Quantidade de Bebidas: %d", atual->qtd_bebida);
            }
            printf("\n");
            total_diario += atual->preco; // Adiciona o preço da venda ao total do dia
            venda_encontrada = true;
        }
        atual = atual->proximo;
    }
    if (!venda_encontrada) {
        printf("Nenhuma venda encontrada para a data %02d/%02d/%d.\n", dia, mes, ano);
    } else {
        printf("Total de vendas para %02d/%02d/%d: R$ %.2f\n", dia, mes, ano, total_diario);
    }
    printf("---------------------------------------\n");
}

// *** MODIFICADO: Gerar Relatório Mensal para incluir detalhes diários e total correto do mês/ano ***
void gerarRelatorioMensal(Venda* cabeca, int mes, int ano) { // Removido 'float vendasMensais[]' da assinatura
    const char* nomesMeses[] = {
        "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
        "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"
    };

    if (mes < 1 || mes > NUM_MESES) {
        printf("Mês inválido para o relatório mensal.\n");
        return;
    }

    printf("\n--- Relatório Mensal Detalhado (%s de %d) ---\n", nomesMeses[mes - 1], ano);

    bool venda_encontrada_no_mes = false;
    float total_mensal_do_ano_especifico = 0.0; // NOVO: Variável para somar o total do mês e ano específicos
    int dias_no_mes_validado = 0; // Para controlar o loop de dias

    // Obter o número de dias no mês para iterar
    int diasNoMesArray[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && ehBissexto(ano)) {
        dias_no_mes_validado = 29;
    } else {
        dias_no_mes_validado = diasNoMesArray[mes];
    }

    // Iterar por cada dia do mês para exibir as vendas daquele dia
    for (int dia_atual = 1; dia_atual <= dias_no_mes_validado; dia_atual++) {
        Venda* venda_do_dia_atual = cabeca;
        bool dia_com_venda = false; // Flag para verificar se há vendas no dia atual
        float total_dia_atual = 0.0;
        int compra_numero_dia = 1;

        // Título para o dia
        bool titulo_dia_impresso = false;

        while (venda_do_dia_atual != NULL) {
            if (venda_do_dia_atual->dataVenda.dia == dia_atual &&
                venda_do_dia_atual->dataVenda.mes == mes &&
                venda_do_dia_atual->dataVenda.ano == ano) { // IMPORTANTE: FILTRA PELO ANO AQUI TAMBÉM

                if (!titulo_dia_impresso) {
                    printf("\n--- Vendas do Dia %02d/%02d/%d ---\n", dia_atual, mes, ano);
                    titulo_dia_impresso = true;
                }

                printf("  Compra %d: Preço Total: R$ %.2f", compra_numero_dia++, venda_do_dia_atual->preco);
                if (venda_do_dia_atual->peso_refeicao > 0) {
                    printf(", Peso da Refeição: %.2f gramas", venda_do_dia_atual->peso_refeicao);
                }
                if (venda_do_dia_atual->qtd_bebida > 0) {
                    printf(", Quantidade de Bebidas: %d", venda_do_dia_atual->qtd_bebida);
                }
                printf("\n");
                total_dia_atual += venda_do_dia_atual->preco;
                venda_encontrada_no_mes = true; // Marca que alguma venda foi encontrada no mês
                dia_com_venda = true; // Marca que o dia atual tem vendas
            }
            venda_do_dia_atual = venda_do_dia_atual->proximo;
        }
        if (dia_com_venda) { // Imprime o total do dia somente se houver vendas nesse dia
            printf("  Total do Dia %02d/%02d/%d: R$ %.2f\n", dia_atual, mes, ano, total_dia_atual);
            printf("  -----------------------------------\n");
            total_mensal_do_ano_especifico += total_dia_atual; // SOMA PARA O TOTAL MENSAL DO ANO ESPECÍFICO
        }
    }

    if (!venda_encontrada_no_mes) {
        printf("Nenhuma venda encontrada para %s de %d.\n", nomesMeses[mes - 1], ano);
    } else {
        // Usa o valor calculado dentro desta função, específico para o mês/ano
        printf("\nTotal de Vendas para %s de %d: R$ %.2f\n", nomesMeses[mes - 1], ano, total_mensal_do_ano_especifico);
    }
    printf("-------------------------------------------\n");
}


// *** Algoritmo de Ordenação (Bubble Sort para demonstração) ***
void ordenarVendasMensais(VendaMes vendasAnual[], int tamanho) {
    int i, j;
    VendaMes temp;
    // Ordenação decrescente (do maior para o menor)
    for (i = 0; i < tamanho - 1; i++) {
        for (j = 0; j < tamanho - 1 - i; j++) {
            if (vendasAnual[j].totalVendido < vendasAnual[j + 1].totalVendido) {
                temp = vendasAnual[j];
                vendasAnual[j] = vendasAnual[j + 1];
                vendasAnual[j + 1] = temp;
            }
        }
    }
}

// *** NOVO Gerar Relatório Anual (com detalhes diários e ordenação mensal) ***
void gerarRelatorioAnual(Venda* cabeca, int ano) {
    printf("\n--- Relatório Anual Detalhado para o ano %d ---\n", ano);

    const char* nomesMeses[] = {
        "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
        "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"
    };

    // Array para armazenar os totais de vendas de cada mês do ano
    VendaMes vendasAnual[NUM_MESES];
    float totalGeralAnual = 0.0;
    bool vendas_encontradas_no_ano = false;

    // Inicializa os totais para este ano
    for (int i = 0; i < NUM_MESES; i++) {
        vendasAnual[i].mes = i + 1; // Mês 1-12
        vendasAnual[i].totalVendido = 0.0; // Zera para o ano atual
    }

    // Define os dias em cada mês para um ano comum e ajusta para bissexto
    int diasNoMesArray[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (ehBissexto(ano)) {
        diasNoMesArray[2] = 29; // Ajusta fevereiro para ano bissexto
    }

    // Iterar por cada mês do ano
    for (int mes_atual = 1; mes_atual <= NUM_MESES; mes_atual++) {
        float total_mensal_atual = 0.0;
        bool vendas_encontradas_no_mes = false;

        // Iterar por cada dia do mês atual
        for (int dia_atual = 1; dia_atual <= diasNoMesArray[mes_atual]; dia_atual++) {
            Venda* venda_do_dia_atual = cabeca;
            bool vendas_encontradas_no_dia = false;
            float total_diario_atual = 0.0;
            int compra_numero_dia = 1;

            // Percorre a lista de vendas para encontrar as vendas do dia, mês e ano atuais
            while (venda_do_dia_atual != NULL) {
                if (venda_do_dia_atual->dataVenda.dia == dia_atual &&
                    venda_do_dia_atual->dataVenda.mes == mes_atual &&
                    venda_do_dia_atual->dataVenda.ano == ano) {

                    // Se esta é a primeira venda do dia, imprime o cabeçalho do dia
                    if (!vendas_encontradas_no_dia) {
                        printf("\n--- Vendas do Dia %02d/%02d/%d ---\n", dia_atual, mes_atual, ano);
                        vendas_encontradas_no_dia = true;
                        vendas_encontradas_no_mes = true;
                        vendas_encontradas_no_ano = true;
                    }

                    printf("  Compra %d: Preço Total: R$ %.2f", compra_numero_dia++, venda_do_dia_atual->preco);
                    if (venda_do_dia_atual->peso_refeicao > 0) {
                        printf(", Peso da Refeição: %.2f gramas", venda_do_dia_atual->peso_refeicao);
                    }
                    if (venda_do_dia_atual->qtd_bebida > 0) {
                        printf(", Quantidade de Bebidas: %d", venda_do_dia_atual->qtd_bebida);
                    }
                    printf("\n");
                    total_diario_atual += venda_do_dia_atual->preco;
                }
                venda_do_dia_atual = venda_do_dia_atual->proximo;
            }

            // Se houve vendas no dia, exibe o total diário
            if (vendas_encontradas_no_dia) {
                printf("  Total do Dia %02d/%02d/%d: R$ %.2f\n", dia_atual, mes_atual, ano, total_diario_atual);
                printf("  -----------------------------------\n");
                total_mensal_atual += total_diario_atual; // Acumula para o total mensal
            }
        }

        // Se houve vendas no mês, exibe o total mensal e armazena para ordenação
        if (vendas_encontradas_no_mes) {
            printf("\nTotal de Vendas para %s de %d: R$ %.2f\n", nomesMeses[mes_atual - 1], ano, total_mensal_atual);
            printf("-------------------------------------------\n");
            vendasAnual[mes_atual - 1].totalVendido = total_mensal_atual; // Armazena para ordenação posterior
        }
    }

    // Se nenhuma venda foi encontrada para o ano, informa e sai
    if (!vendas_encontradas_no_ano) {
        printf("Nenhuma venda encontrada para o ano %d.\n", ano);
        printf("---------------------------------------\n");
        return;
    }

    // Calcula o total geral anual (somando os totais mensais já acumulados)
    for (int i = 0; i < NUM_MESES; i++) {
        totalGeralAnual += vendasAnual[i].totalVendido;
    }

    // Ordenar os meses pelo total de vendas (decrescente) para a exibição final
    ordenarVendasMensais(vendasAnual, NUM_MESES);

    printf("\n--- Resumo Anual por Mês (Ordem Decrescente de Vendas) ---\n");
    for (int i = 0; i < NUM_MESES; i++) {
        // Exibe apenas os meses que tiveram vendas, ou todos se desejar.
        // A lógica atual exibe todos, mas com R$0.00 para meses sem venda.
        if (vendasAnual[i].totalVendido > 0) { // Opcional: exibe apenas meses com vendas > 0
            printf("- %s: R$ %.2f\n", nomesMeses[vendasAnual[i].mes - 1], vendasAnual[i].totalVendido);
        }
    }
    printf("\nTotal Geral de Vendas no ano %d: R$ %.2f\n", ano, totalGeralAnual);
    printf("-----------------------------------------------------------\n");
}


void salvarVendas(Venda* cabeca) {
    FILE* arquivo = fopen(ARQUIVO_VENDAS, "wb"); // "wb" para escrita binária
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para salvar vendas");
        return;
    }

    Venda* atual = cabeca;
    while (atual != NULL) {

        struct {
            float preco;
            float peso_refeicao;
            int qtd_bebida;
            Data dataVenda;
        } temp;
        temp.preco = atual->preco;
        temp.peso_refeicao = atual->peso_refeicao;
        temp.qtd_bebida = atual->qtd_bebida;
        temp.dataVenda = atual->dataVenda;

        fwrite(&temp, sizeof(temp), 1, arquivo);
        atual = atual->proximo;
    }

    fclose(arquivo);
    printf("Dados de vendas salvos com sucesso em '%s'.\n", ARQUIVO_VENDAS);
}


void carregarVendas(Venda** cabeca) {
    FILE* arquivo = fopen(ARQUIVO_VENDAS, "rb");
    if (arquivo == NULL) {
        printf("Arquivo de vendas '%s' não encontrado. Iniciando com uma nova lista de vendas.\n", ARQUIVO_VENDAS);
        *cabeca = NULL;
        return;
    } else {
        printf("Arquivo '%s' localizado. Carregando dados de vendas...\n", ARQUIVO_VENDAS);
    }


    struct {
        float preco;
        float peso_refeicao;
        int qtd_bebida;
        Data dataVenda;
    } temp;

    Venda* novaVenda;
    *cabeca = NULL;

    while (fread(&temp, sizeof(temp), 1, arquivo) == 1) {

        novaVenda = criarVenda(temp.preco, temp.peso_refeicao,
                               temp.qtd_bebida, temp.dataVenda.dia,
                               temp.dataVenda.mes, temp.dataVenda.ano);
        if (*cabeca == NULL) {
            *cabeca = novaVenda;
        } else {
            Venda* atual = *cabeca;
            while (atual->proximo != NULL) {
                atual = atual->proximo;
            }
            atual->proximo = novaVenda;
        }
    }

    fclose(arquivo);
    printf("Informações de vendas recuperadas com sucesso do arquivo '%s'.\n", ARQUIVO_VENDAS);
}


void liberarLista(Venda* cabeca) {
    Venda* atual = cabeca;
    Venda* proximo;
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese"); // Define o locale para português
    Venda* listaDeVendas = NULL;
    int opcao_principal;
    int dia_venda, mes_venda, ano_venda;
    bool novo_dia = true;

    // O array vendasMensais não será mais usado para o total geral do mês no relatório mensal,
    // mas ainda é útil para o relatório anual, embora ele também calcule da lista.
    // Mantenha-o para a função inserirVenda que ainda o espera.
    float vendasMensais[NUM_MESES];

    // Inicializa o array de vendas mensais com zero
    for (int i = 0; i < NUM_MESES; i++) {
        vendasMensais[i] = 0.0;
    }

    // Carrega os dados de vendas do arquivo no início do programa
    carregarVendas(&listaDeVendas);

    do {
        if (novo_dia) {
            printf("\n--- Início do Dia de Vendas ---\n");
            bool data_valida_e_formatada = false;
            char linha_entrada[20]; // Buffer para ler a linha completa da data

            while (!data_valida_e_formatada) {
                printf("Digite a data para as vendas de hoje (dd mm aaaa): ");
                // Lê a linha inteira, incluindo espaços
                if (fgets(linha_entrada, sizeof(linha_entrada), stdin) == NULL) {
                    printf("Erro ao ler a entrada. Encerrando o programa.\n");
                    if (listaDeVendas != NULL) {
                        liberarLista(listaDeVendas);
                    }
                    return EXIT_FAILURE;
                }

                // Tenta parsear a linha lida. %n armazena quantos caracteres foram lidos até aquele ponto.
                // Isso nos permite verificar se a linha foi completamente consumida, garantindo o formato.
                int num_lidos;
                int result = sscanf(linha_entrada, "%d %d %d%n", &dia_venda, &mes_venda, &ano_venda, &num_lidos);

                // Verifica se leu 3 itens E se não há caracteres extras na linha (além de possíveis quebras de linha)
                if (result == 3 && (linha_entrada[num_lidos] == '\n' || linha_entrada[num_lidos] == '\0')) {
                    // Formato "dd mm aaaa" foi respeitado, agora valida a data em si
                    if (validarData(dia_venda, mes_venda, ano_venda)) {
                        data_valida_e_formatada = true;
                    } else {
                        // validarData já imprime a mensagem de erro específica.
                        // O loop continua para pedir a data novamente.
                    }
                } else {
                    printf("Formato de data inválido. Por favor, use 'dd mm aaaa' (ex: 01 01 2023).\n");
                    // O fgets já consome a linha, então não precisamos de getchar() aqui.
                }
            }
            novo_dia = false;
            printf("Data atual de operação: %02d/%02d/%d\n", dia_venda, mes_venda, ano_venda);
        }

        printf("\n--- Menu Principal ---\n");
        printf("1. Registrar Nova Venda\n");
        printf("2. Ver Relatórios de Vendas\n");
        printf("3. Finalizar o Dia de Vendas Atual\n");
        printf("0. Sair do Programa\n");
        printf("Escolha uma opção: ");
        if (scanf("%d", &opcao_principal) != 1) {
            printf("Opção inválida. Por favor, digite um número correspondente ao menu.\n");
            while (getchar() != '\n');
            opcao_principal= -1;
            continue;
        }
        while (getchar() != '\n');

        switch (opcao_principal) {
            case 1: { // Registrar Nova Venda
                float peso_refeicao = 0.0;
                float preco_total_item = cadastrarValorRefeicao(&peso_refeicao);
                int qtd_total_bebidas_item = 0;

                char adicionar_bebida_pergunta[10];

                while (1) {
                    printf("Deseja adicionar bebidas a este item? (sim/nao): ");
                    scanf("%s", adicionar_bebida_pergunta);
                    while (getchar() != '\n');
                    if (strcmp(adicionar_bebida_pergunta, "sim") == 0) {
                        adicionarBebidas(&preco_total_item, &qtd_total_bebidas_item);
                        break;
                    } else if (strcmp(adicionar_bebida_pergunta, "nao") == 0) {
                        break;
                    } else {
                        printf("Resposta inválida. Digite 'sim' ou 'nao'.\n");
                    }
                }

                if (preco_total_item > 0) {
                    // *** MODIFICADO: Passando vendasMensais para inserirVenda ***
                    inserirVenda(&listaDeVendas, preco_total_item, peso_refeicao, qtd_total_bebidas_item, dia_venda, mes_venda, ano_venda, vendasMensais);
                    printf("Venda registrada com sucesso para %02d/%02d/%d.\n", dia_venda, mes_venda, ano_venda);
                    printf("Valor total deste item de venda: R$ %.2f\n", preco_total_item);
                } else {
                    printf("Venda com valor R$ %.2f não registrada, pois o valor é zero.\n", preco_total_item);
                }
                break;
            }
            case 2: { // Ver Relatórios de Vendas
                int opcao_relatorio;
                printf("\n--- Menu de Relatórios ---\n");
                printf("1. Relatório Diário\n");
                printf("2. Relatório Mensal\n");
                printf("3. Relatório Anual\n");
                printf("4. Relatório Geral (Todas as Vendas)\n");
                printf("0. Voltar ao Menu Principal\n");
                printf("Escolha uma opção: ");
                if (scanf("%d", &opcao_relatorio) != 1) {
                    printf("Opção inválida. Por favor, digite um número.\n");
                    while (getchar() != '\n');
                    continue;
                }
                while (getchar() != '\n');

                switch (opcao_relatorio) {
                    case 1: {
                        int dia_rel, mes_rel, ano_rel;
                        bool data_rel_valida_e_formatada = false;
                        char linha_entrada_rel[20]; // Buffer para ler a linha completa da data do relatório

                        while (!data_rel_valida_e_formatada) {
                            printf("Digite a data para o relatório diário (dd mm aaaa): ");
                            if (fgets(linha_entrada_rel, sizeof(linha_entrada_rel), stdin) == NULL) {
                                printf("Erro ao ler a entrada. Voltando ao menu principal.\n");
                                break; // Sai do loop de data e volta ao menu de relatórios
                            }

                            int num_lidos_rel;
                            int result_rel = sscanf(linha_entrada_rel, "%d %d %d%n", &dia_rel, &mes_rel, &ano_rel, &num_lidos_rel);

                            if (result_rel == 3 && (linha_entrada_rel[num_lidos_rel] == '\n' || linha_entrada_rel[num_lidos_rel] == '\0')) {
                                if (validarData(dia_rel, mes_rel, ano_rel)) {
                                    data_rel_valida_e_formatada = true;
                                } else {
                                    // validarData já imprime a mensagem de erro
                                }
                            } else {
                                printf("Formato de data inválido. Por favor, use 'dd mm aaaa' (ex: 01 01 2023).\n");
                            }
                        }
                        if (data_rel_valida_e_formatada) { // Só gera o relatório se a data for válida e bem formatada
                           gerarRelatorioDiario(listaDeVendas, dia_rel, mes_rel, ano_rel);
                        }
                        break;
                    }
                    case 2: { // *** NOVO: Relatório Mensal (agora com detalhes) ***
                        int mes_rel, ano_rel;
                        bool data_rel_valida_e_formatada = false;
                        char linha_entrada_rel[20]; // Buffer para ler a linha completa da data do relatório

                        while (!data_rel_valida_e_formatada) {
                            printf("Digite o mês e ano para o relatório mensal (mm aaaa): ");
                            if (fgets(linha_entrada_rel, sizeof(linha_entrada_rel), stdin) == NULL) {
                                printf("Erro ao ler a entrada. Voltando ao menu principal.\n");
                                break; // Sai do loop de data e volta ao menu de relatórios
                            }

                            int num_lidos_rel;
                            int result_rel = sscanf(linha_entrada_rel, "%d %d%n", &mes_rel, &ano_rel, &num_lidos_rel);

                            // Note que aqui validamos apenas mês e ano, e o ano para validação da data em si
                            if (result_rel == 2 && (linha_entrada_rel[num_lidos_rel] == '\n' || linha_entrada_rel[num_lidos_rel] == '\0')) {
                                if (validarData(1, mes_rel, ano_rel)) { // Usamos dia 1 para validar mês e ano
                                    data_rel_valida_e_formatada = true;
                                } else {
                                    // validarData já imprime a mensagem de erro
                                }
                            } else {
                                printf("Formato de mês e ano inválido. Por favor, use 'mm aaaa' (ex: 01 2023).\n");
                            }
                        }
                        if (data_rel_valida_e_formatada) {
                            gerarRelatorioMensal(listaDeVendas, mes_rel, ano_rel); // Removido 'vendasMensais' daqui
                        }
                        break;
                    }
                    case 3: { // Relatório Anual (agora passa o ano)
                        int ano_rel;
                        printf("Digite o ano para o relatório anual: ");
                        if (scanf("%d", &ano_rel) != 1 || ano_rel < 1900 || ano_rel > 2100) {
                            printf("Ano inválido. Digite um ano entre 1900 e 2100.\n");
                            while(getchar() != '\n'); // Limpa o buffer de entrada
                            break;
                        }
                        while(getchar() != '\n'); // Limpa o buffer de entrada
                        gerarRelatorioAnual(listaDeVendas, ano_rel); // Passa listaDeVendas e o ano
                        break;
                    }
                    case 4:
                        imprimirVendas(listaDeVendas);
                        break;
                    case 0:
                        printf("Voltando ao menu principal.\n");
                        break;
                    default:
                        printf("Opção inválida. Por favor, escolha uma opção listada.\n");
                        break;
                }
                break;
            }
            case 3:
                printf("\n--- Finalizando o Dia de Vendas (%02d/%02d/%02d) ---\n", dia_venda, mes_venda, ano_venda);
                salvarVendas(listaDeVendas);
                gerarRelatorioDiario(listaDeVendas, dia_venda, mes_venda, ano_venda);
                novo_dia = true;
                printf("\nDia de vendas finalizado com sucesso. Os dados foram salvos.\n");
                break;
            case 0:
                printf("Saindo do programa...\n");
                salvarVendas(listaDeVendas); // Salva todas as vendas antes de sair
                break;
            default:
                printf("Opção inválida. Por favor, selecione uma opção válida do menu.\n");
        }
    } while (opcao_principal != 0);

    if (listaDeVendas != NULL) {
        liberarLista(listaDeVendas);
    }

    return 0;
}
