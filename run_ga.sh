#!/bin/bash

EXEC="./build/genetic"
RAW_RESULTS="result/genetic_results.txt"
SUMMARY_DIR="result/genetic_summary"
RUNS=20

mkdir -p "$SUMMARY_DIR"

# Limpar resumos anteriores
rm -f "$SUMMARY_DIR"/*

declare -A COSTS
declare -A TIMES

# Executar 20 vezes
for ((i=1; i<=RUNS; i++)); do
    echo "Executando rodada $i ..."
    $EXEC

    # Processar genetic_results.txt recém-gerado
    while IFS= read -r line; do
        if [[ "$line" =~ ^Results\ for\ file:\ (.*) ]]; then
            CURRENT_FILE="${BASH_REMATCH[1]}"
        fi

        if [[ "$line" =~ Cost:\ ([0-9.]+) ]]; then
            COST="${BASH_REMATCH[1]}"
            COSTS["$CURRENT_FILE"]+="$COST "
        fi

        if [[ "$line" =~ Time:\ ([0-9.]+) ]]; then
            TIME="${BASH_REMATCH[1]}"
            TIMES["$CURRENT_FILE"]+="$TIME "
        fi
    done < "$RAW_RESULTS"
done

# Gerar arquivos de resumo
for FILE in "${!COSTS[@]}"; do
    BASE=$(basename "$FILE" .csv)
    OUTFILE="$SUMMARY_DIR/${BASE}_summary.txt"

    echo "Gerando resumo para: $FILE -> $OUTFILE"

    echo "Resumo para arquivo: $FILE" > "$OUTFILE"
    echo "" >> "$OUTFILE"

    # Custos e tempos acumulados
    COST_LIST=(${COSTS[$FILE]})
    TIME_LIST=(${TIMES[$FILE]})

    echo "Execuções:" >> "$OUTFILE"

    best_cost=99999999
    total_time=0

    for idx in "${!COST_LIST[@]}"; do
        C=${COST_LIST[$idx]}
        T=${TIME_LIST[$idx]}

        echo "Execução $((idx+1)) -> Cost: $C | Time: $T ms" >> "$OUTFILE"

        (( $(echo "$C < $best_cost" | bc -l) )) && best_cost="$C"
        total_time=$(echo "$total_time + $T" | bc)
    done

    avg_time=$(echo "$total_time / ${#TIME_LIST[@]}" | bc -l)

    echo "" >> "$OUTFILE"
    echo "--------------------------------------" >> "$OUTFILE"
    echo "Melhor custo: $best_cost" >> "$OUTFILE"
    echo "Média dos tempos: $avg_time ms" >> "$OUTFILE"
    echo "--------------------------------------" >> "$OUTFILE"

    # Adiviona todos os resultados a um csv
    echo "$FILE,$best_cost,$avg_time" >> "$SUMMARY_DIR/overview.csv"
done

echo "Processo finalizado! Resumos em: $SUMMARY_DIR/"
