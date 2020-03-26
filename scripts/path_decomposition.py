from sage.all import Graph, matrix, graphs, plot
from sage.graphs.graph_decompositions.vertex_separation import vertex_separation, linear_ordering_to_path_decomposition, path_decomposition
from random import randrange, uniform

def inv(perm):
    inverse = [0] * len(perm)
    for i, p in enumerate(perm):
        inverse[p] = i
    return inverse

def generate_input(n):
    # g = graphs.CycleGraph(n)
    # g = graphs.HeawoodGraph()
    # g = graphs.QueenGraph( [2, 2] )
    #g = graphs.RandomBicubicPlanar(n)
    g = graphs.RandomGNP(n, uniform(.2,.6))
    return g

def graph_relabel(g):
    d = {}
    for i, l in enumerate(g.vertices()):
        d[l]=i
    g.relabel(d)
    return g

def get_path_decomp(h):
    degs = h.degree(labels=True)
    for x in degs:
        if(degs[x] == 1):
            v = x;
    print(list(h.depth_first_search(v)))
    r = list(h.depth_first_search(v))
    path_decomposition_order = []
    for i in range(len(r)-1):
        d = [item for item in r[i] if item not in r[i+1]]
        path_decomposition_order.extend(d)
    d = [item for item in r[-1] if item not in path_decomposition_order]
    path_decomposition_order.extend(d)
    print(path_decomposition_order)
    print(len(path_decomposition_order))
    return path_decomposition_order


it = 0
while(it < 1000):
    # Generate input graph.
    b = 23
    n = randrange(b, b+10)
    g = generate_input(n)
    # Relabel vertices of the graph.
    g = graph_relabel(g)
    # Obtain optimal path decomposition.
    pw, L = path_decomposition(g, algorithm = "BAB")
    h = linear_ordering_to_path_decomposition(g, L)
    #print("pw = " + str(pw))
    if(pw > 5 or (not g.is_connected())):
        continue
    it = it+1
    print(it)
    # Get path decomposition ordering of the vertices.
    path_decomposition_order = get_path_decomp(h)
    # Relabel Graph
    g.relabel(inv(path_decomposition_order))
    #p = plot(g, vertex_labels=True, vertex_size=800, vertex_color='white')
    #p.save('test' + str(it) + '.pdf')
    g.export_to_file('graphs/'+ str(it) + '.txt', format = 'edgelist')


# Plot stuff.
#p = plot(g, vertex_labels=True, vertex_size=800, vertex_color='white')
#p.save('test1.pdf')
#p = plot(g, vertex_labels=True, vertex_size=800, vertex_color='white')
#p.save('test2.pdf')
#p = plot(h, vertex_labels=True, vertex_size=800, vertex_color='white')
#p.save('test3.pdf')

#save(obj, "name") works to save variables of the environment
