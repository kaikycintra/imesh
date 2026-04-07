import matplotlib.pyplot as plt
import numpy as np

def parse_simulation_file(filename):
    """
    Lê o arquivo e retorna a taxa de cumprimento de deadline e o total de preempções.
    """
    try:
        with open(filename, 'r') as f:
            lines = f.readlines()
        
        # Filtra linhas vazias
        lines = [line.strip() for line in lines if line.strip()]
        
        # A última linha contém as preempções (apenas para RR, SJF é 0)
        # Se a última linha não for um número isolado, assume-se 0 preempções (SJF)
        try:
            preemptions = int(lines[-1])
            process_lines = lines[:-1]
        except ValueError:
            preemptions = 0
            process_lines = lines
            
        # Calcula cumprimento de deadline: a primeira coluna é 'cumpriu' (0 ou 1)
        deadlines_met = [int(line.split()[0]) for line in process_lines]
        met_rate = (sum(deadlines_met) / len(deadlines_met)) * 100 if deadlines_met else 0
        
        return met_rate, preemptions
    except FileNotFoundError:
        print(f"Erro: Arquivo {filename} não encontrado.")
        return 0, 0

# Definição dos conjuntos de dados baseados nos arquivos fornecidos
# Estrutura: (Nome do Gráfico, Arquivo SJF, Arquivo RR)
scenarios = [
    # Máquina A (8 Workers)
    ("Deadline - Máquina A (8 Workers) - Inesperado", "sjf-8-inesperado.txt", "rr-8-inesperado.txt"),
    ("Preempções - Máquina A (8 Workers) - Inesperado", "sjf-8-inesperado.txt", "rr-8-inesperado.txt"),
    ("Deadline - Máquina A (8 Workers) - Esperado", "sjf-8-esperado.txt", "rr-8-esperado.txt"),
    ("Preempções - Máquina A (8 Workers) - Esperado", "sjf-8-esperado.txt", "rr-8-esperado.txt"),
    
    # Máquina B (2 Workers)
    ("Deadline - Máquina B (2 Workers) - Inesperado", "sjf-2-inesperado.txt", "rr-2-inesperado.txt"),
    ("Preempções - Máquina B (2 Workers) - Inesperado", "sjf-2-inesperado.txt", "rr-2-inesperado.txt"),
    ("Deadline - Máquina B (2 Workers) - Esperado", "sjf-2-esperado.txt", "rr-2-esperado.txt"),
    ("Preempções - Máquina B (2 Workers) - Esperado", "sjf-2-esperado.txt", "rr-2-esperado.txt"),
]

def generate_plots():
    fig, axes = plt.subplots(nrows=4, ncols=2, figsize=(14, 18))
    axes = axes.flatten()

    for i, (title, sjf_file, rr_file) in enumerate(scenarios):
        sjf_met, sjf_pre = parse_simulation_file(sjf_file)
        rr_met, rr_pre = parse_simulation_file(rr_file)

        is_deadline_plot = "Deadline" in title
        
        labels = ['SJF', 'RR']
        if is_deadline_plot:
            values = [sjf_met, rr_met]
            ylabel = "Cumprimento de Deadline (%)"
            color = ['skyblue', 'salmon']
            ylim = 110
        else:
            values = [sjf_pre, rr_pre]
            ylabel = "Total de Preempções"
            color = ['steelblue', 'indianred']
            ylim = max(values) * 1.2 if max(values) > 0 else 10

        # Plotagem
        bars = axes[i].bar(labels, values, color=color, edgecolor='black', alpha=0.8)
        axes[i].set_title(title, fontsize=12, fontweight='bold')
        axes[i].set_ylabel(ylabel)
        axes[i].set_ylim(0, ylim)
        
        # Adiciona os valores no topo das barras
        for bar in bars:
            height = bar.get_height()
            axes[i].text(bar.get_x() + bar.get_width()/2., height + (ylim*0.02),
                        f'{height:.1f}' if is_deadline_plot else f'{int(height)}',
                        ha='center', va='bottom', fontsize=10)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    generate_plots()