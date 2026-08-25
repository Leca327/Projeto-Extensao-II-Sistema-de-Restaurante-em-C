# Sistema de Vendas para Restaurante Self-Service

Aplicação de linha de comando em C para registro e acompanhamento das vendas de um restaurante self-service, com controle por dia de operação, persistência dos dados entre execuções e geração de relatórios.
Projeto de Extensão II — Engenharia da Computação, Universidade Veiga de Almeida.

---

## O que o sistema faz

O programa opera por **dia de vendas**: ao iniciar, pede a data corrente e mantém todos os registros vinculados a ela até que o dia seja encerrado. Isso reproduz a rotina real de um caixa, em que o operador abre e fecha o expediente.

Cada venda pode combinar refeição e bebidas:

- **Quentinha** — preço fixo de R$ 20,00, com acréscimo opcional de R$ 0,50 pela embalagem
- **Refeição por peso** — R$ 4,99 a cada 100 gramas, calculado a partir do peso informado
- **Bebidas** — água mineral, refrigerante, suco natural e cerveja, em qualquer quantidade, somadas ao total da venda

## Relatórios

| Relatório | Conteúdo |
|---|---|
| Diário | Todas as compras de uma data, com peso e quantidade de bebidas por venda, e o total do dia |
| Mensal | Percorre cada dia do mês informado, detalha as vendas encontradas e consolida o total mensal |
| Anual | Detalha o ano inteiro dia a dia e mês a mês, e fecha com um ranking dos meses em ordem decrescente de faturamento |
| Geral | Lista todas as vendas registradas no sistema |

## Estruturas e técnicas empregadas

- **Lista encadeada** de vendas, com alocação dinâmica (`malloc`) e liberação da memória ao encerrar
- **Structs aninhadas** — cada venda contém uma struct `Data` com dia, mês e ano
- **Enum** para os tipos de bebida, tornando o `switch` do menu legível e evitando números mágicos
- **Persistência em arquivo binário** (`vendas.dat`), com `fwrite`/`fread` — os dados são carregados na abertura do programa e gravados ao encerrar o dia ou sair
- **Bubble sort** aplicado ao array de totais mensais, para ordenar o ranking anual
- **Validação de datas** com verificação de ano bissexto e de dias válidos por mês
- **Tratamento de entrada** — leitura com `fgets` e `sscanf` para validar o formato completo da data, e limpeza do buffer após cada `scanf` para evitar leituras residuais

## Como compilar e executar

Com o GCC:

```bash
gcc codigosabado.c -o vendas
./vendas
```

No Windows, o executável gerado será `vendas.exe`.

O arquivo `vendas.dat` é criado automaticamente na primeira execução, no mesmo diretório do programa. Se ele já existir, as vendas anteriores são carregadas ao iniciar.

## Arquivos

```
.
├── codigosabado.c   # Código-fonte completo
└── vendas.dat       # Base de vendas de exemplo (arquivo binário)
```

## Equipe

- André Gustavo de Barros Azevedo Nogueira
- Fabio Cezar Paiva Paixão
- José Antônio Salgado de Castro
- Letícia dos Reis Prado
- Ricardo Lemos Barbosa Filho
