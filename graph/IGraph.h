#ifndef IGRAPH_H
#define IGRAPH_H

#include <vector>
#include <cstddef>

/**
 * @struct EdgeIndex
 * @brief Representa uma aresta entre dois nós do grafo.
 */
struct EdgeIndex {
    int from;
    int to;
};

/**
 * @class IGraph
 * @brief Interface genérica para Grafos.
 * @tparam Node O tipo de dado que será armazenado em cada nó (vértice) do grafo.
 *
 * Esta classe define um conjunto de operações básicas que toda implementação de grafo
 * deve fornecer. 
 * 
 */
template<typename Node>
class IGraph {
public:

    /**
     * @brief Destrutor virtual padrão.
     */
    virtual ~IGraph() = default;

    /**
     * @brief Retorna o número de vértices no grafo.
     * @return Um valor size_t representando a ordem do grafo.
     */
    virtual size_t get_order() const = 0;

    /**
     * @brief Retorna o número de arestas no grafo.
     * @return Um valor size_t representando o tamanho do grafo.
     */
    virtual size_t get_size() const = 0;

    /**
     * @brief Adiciona um novo nó ao grafo.
     * @param node O nó a ser adicionado.
     */
    virtual void add_node(const Node& node) = 0;

    /**
     * @brief Remove um nó do grafo.
     *
     * Ao remover um nó, todas as arestas conectadas a ele também devem ser removidas.
     * @param node O nó a ser removido.
     */
    virtual void remove_node(const Node& node) = 0;

    /**
     * @brief Adiciona uma aresta entre dois nós.
     *
     * Se o grafo for direcionado, a aresta vai de 'from' para 'to'. Se for não
     * direcionado, a conexão é mútua.
     * @param from O nó de origem da aresta.
     * @param to O nó de destino da aresta.
     */
    virtual void add_edge(const Node& from, const Node& to) = 0;

    /**
     * @brief Remove uma aresta existente entre dois nós.
     *
     * @param from O nó de origem da aresta.
     * @param to O nó de destino da aresta.
     */
    virtual void remove_edge(const Node& from, const Node& to) = 0;

    /**
     * @brief Retorna uma lista com todos os vizinhos de um determinado nó.
     *
     * @param node O nó cuja lista de vizinhos será retornada.
     * @return Um std::vector<Node> com os nós vizinhos.
     */
    virtual std::vector<Node> get_neighbors(const Node& node) const = 0;

    /**
     * @brief Retorna uma lista com todos os nós presentes no grafo.
     *
     * @return Um std::vector<Node> contendo todos os nós do grafo.
     */
    virtual std::vector<Node> get_nodes() const = 0;

    /**
     * @brief Verifica se um determinado nó existe no grafo.
     *
     * @param node O nó a ser verificado.
     * @return true se o nó existe no grafo, false caso contrário.
     */
    virtual bool has_node(const Node& node) const = 0;

    /**
     * @brief Imprime uma representação visual do grafo no console.
     */
    virtual void print() const = 0;

    /**
     * @brief Retorna o índice interno associado a um nó.
     *
     * As implementações utilizam índices inteiros para representar os nós internamente. 
     * @param node O nó cujo índice se deseja obter.
     * @return O índice inteiro do nó. Caso o nó não exista, o valor retornado será -1.
     */
    virtual int get_index(const Node& node) const = 0;

    /**
     * @brief Retorna o nó associado a um índice interno.
     *
     * Esta é a operação inversa de get_index.
     * @param index O índice inteiro do nó a ser retornado.
     * @return O Node correspondente ao índice. Caso o índice seja inválido, uma exceção std::out_of_range será lançada.
     */
    virtual Node get_node(int index) const = 0;

    /**
     * @brief Retorna os índices dos vizinhos de um nó, dado o seu índice.
     *
     * @param index O índice do nó cujos vizinhos serão retornados.
     * @return Um std::vector<int> com os índices dos nós vizinhos. Caso o índice seja inválido (negativo ou maior que o número de nós), uma exceção std::out_of_range será lançada.
     */
    virtual std::vector<int> get_neighbors_indices(int index) const = 0;

     /**
     * @brief Calcula o grau de entrada de um nó.
     *
     * O grau de entrada é o número de arestas que chegam a este nó.
     * Para grafos não-direcionados, o grau de entrada é igual ao grau de saída.
     * @param node O nó para o qual o grau de entrada será calculado.
     * @return Um valor size_t representando o número de arestas incidentes.
     */
    virtual size_t get_in_degree(const Node& node) const = 0;

     /**
     * @brief Calcula o grau de saída de um nó.
     *
     * O grau de saída é o número de arestas que partem deste nó.
     * Para grafos não-direcionados, o grau de saída é igual ao grau de entrada.
     * @param node O nó para o qual o grau de saída será calculado.
     * @return Um valor size_t representando o número de arestas que saem do nó.
     */
    virtual size_t get_out_degree(const Node& node) const = 0;

    /**
     * @brief Verifica se dois nós são adjacentes.
     *
     * Para grafos direcionados: um vértice v é adjacente a um vértice u se houver uma aresta (u, v) que aponta de u para v.
     * Para grafos não-direcionados: dois vértices são adjacentes se existe uma aresta entre eles.
     * @param u O nó de origem.
     * @param v O nó de destino.
     * @return true se u e v são adjacentes, false caso contrário.
     */
    virtual bool is_adjacent(const Node& u, const Node& v) const = 0;

    /**
     * @brief Retorna todas as arestas do grafo.
     * @return Um std::vector<EdgeIndex> com todas as arestas do grafo.
     */
    virtual std::vector<EdgeIndex> get_all_edges() const = 0;

};

#endif // IGRAPH_H
