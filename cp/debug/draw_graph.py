import matplotlib
matplotlib.use('TkAgg')
import networkx as nx
import matplotlib.pyplot as plt

with open('cp/debug/test_input.txt') as f:
    n, m = map(int, f.readline().split())
    lines = [line.strip().split() for line in f if line.strip()]

G = nx.DiGraph()
G.add_nodes_from(range(n))

has_weight = len(lines[0]) == 3

for tokens in lines:
    if has_weight:
        u, v, w = tokens
        G.add_edge(int(u), int(v), weight=float(w))
    else:
        u, v = tokens
        G.add_edge(int(u), int(v))
        
pos = nx.spring_layout(G)

nx.draw(G, pos, with_labels=True, node_size=700, node_color='lightblue', arrowsize=20)

if has_weight:
    edge_labels = nx.get_edge_attributes(G, 'weight')
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
    plt.title('Directed Weighted Graph')
else:
    plt.title('Directed Unweighted Graph')

plt.axis('off')
plt.show()
