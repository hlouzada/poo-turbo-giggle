#include <algorithm>
#include <iostream>
#include <vector>

// Classe que mantem um conjunto de valores sem duplicacao e em ordem crescente.
// Permite verificar a existencia ou nao de um valor e pegar uma faixa de
// elementos entre dois valores especificados.
// Template para diferentes tipos de dados do OrderedUniqueValues
template<typename Type>
class OrderedUniqueValues {
  // Invariante:
  // Se size() > 1 && 0 <= i < size()-1 então _data[i] < data[i+1]
  std::vector<Type> _data;

public:
  // Definição de um tipo de iterador para os elementos.
  typedef typename std::vector<Type>::const_iterator const_iterator;

  // Verifica se um elementos com o dado valor foi inserido.
  bool find(Type value) {
    // Como os dados estao ordenados em _data, entao basta fazer uma busca
    // binaria.
    return std::binary_search(begin(_data), end(_data), value);
  }

  // Retorna um par de iteradores para o primeiro e um depois do ultimo
  // valores que sao maiores ou iguais a min_value e menores ou iguais a
  // max_value.
  std::pair<const_iterator, const_iterator> find_range(Type min_value,
                                                       Type max_value) const {
    // Dados ordenados em _data, entao podemos usar lower_bound e upper_bound.
    // Encontra o primeiro elemento que tem valor maior ou igual a min_value.
    auto first = std::lower_bound(begin(_data), end(_data), min_value);
    // Encontra o primeiro elemento que tem valor maior do que max_value.
    auto last = std::upper_bound(begin(_data), end(_data), max_value);
    return {first, last};
  }

  // Numero de elementos correntemente armazenados.
  size_t size() const { return _data.size(); }

  // Insere um novo elemento, se nao existir ainda.
  void insert(Type value) {
    auto [first, last] = std::equal_range(begin(_data), end(_data), value);
    if (first == last) {
      _data.insert(last, value);
    }
  }
};


int main(int, char *[]) {
  // Alguns testes simples.
  
  // Definição dos valores e do tamanho
  std::vector<int> some_values_int{7, -10, 4, 8, -2, 9, -10, 8, -5, 6, -9, 5};
  std::vector<float> some_values_float{7.124125, -10.1251, 4, 8.6126, -2.152, 9.10, -10.1251, 8.6126, -5.26, 6.12, -9.5, 5.6};
  std::vector<double> some_values_double{7.152, -10.9125601276, 4.152, 8.12516, -2.5261, 9.5126, -10.9125601276, 8.12516, -5.11, 6.63666125123, -9.6365135, 5.613513};
  std::vector<size_t> some_sizes{1, 2, 3, 4, 5, 6, 6, 6, 7, 8, 9, 10};

  // Criação das classes com os diferentes tipos de dados
  OrderedUniqueValues<int> ouv_int;
  OrderedUniqueValues<float> ouv_float;
  OrderedUniqueValues<double> ouv_double;
  
  // Testes com o int
  for (size_t i = 0; i < some_values_int.size(); ++i) {
    ouv_int.insert(some_values_int[i]);
    if (ouv_int.size() != some_sizes[i]) {
      std::cerr << "Erro de insercao int: indice " << i
                << ", valor: " << some_values_int[i]
                << ", tamanho esperado: " << some_sizes[i]
                << ", tamanho obtido: " << ouv_int.size() << std::endl;
    }
  }

  for (auto x : some_values_int) {
    if (!ouv_int.find(x)) {
      std::cerr << "Nao achou valor int inserido " << x << std::endl;
    }
  }

  auto [first1, last1] = ouv_int.find_range(0, 9);
  for (auto current = first1; current != last1; ++current) {
    if (*current < 0) {
      std::cerr << "Erro na selecao dos valores int nao-negativos: " << *current
                << std::endl;
    }
  }
  auto [first2, last2] = ouv_int.find_range(-10, 0);
  for (auto current = first2; current != last2; ++current) {
    if (*current >= 0) {
      std::cerr << "Erro na selecao dos valores int negativos: " << *current
                << std::endl;
    }
  }

  // Testes com o float
  for (size_t i = 0; i < some_values_float.size(); ++i) {
    ouv_float.insert(some_values_float[i]);
    if (ouv_float.size() != some_sizes[i]) {
      std::cerr << "Erro de insercao float: indice " << i
                << ", valor: " << some_values_float[i]
                << ", tamanho esperado: " << some_sizes[i]
                << ", tamanho obtido: " << ouv_float.size() << std::endl;
    }
  }

  for (auto x : some_values_float) {
    if (!ouv_float.find(x)) {
      std::cerr << "Nao achou valor float inserido " << x << std::endl;
    }
  }

  auto [first3, last3] = ouv_float.find_range(0.0, 9.0);
  for (auto current = first3; current != last3; ++current) {
    if (*current < 0) {
      std::cerr << "Erro na selecao dos valores float nao-negativos: " << *current
                << std::endl;
    }
  }
  auto [first4, last4] = ouv_float.find_range(-10, 0);
  for (auto current = first4; current != last4; ++current) {
    if (*current >= 0) {
      std::cerr << "Erro na selecao dos valores float negativos: " << *current
                << std::endl;
    }
  }

  // Teste com o double
  for (size_t i = 0; i < some_values_double.size(); ++i) {
    ouv_double.insert(some_values_double[i]);
    if (ouv_double.size() != some_sizes[i]) {
      std::cerr << "Erro de insercao double: indice " << i
                << ", valor: " << some_values_double[i]
                << ", tamanho esperado: " << some_sizes[i]
                << ", tamanho obtido: " << ouv_double.size() << std::endl;
    }
  }

  for (auto x : some_values_double) {
    if (!ouv_double.find(x)) {
      std::cerr << "Nao achou valor double inserido " << x << std::endl;
    }
  }

  auto [first5, last5] = ouv_double.find_range(0, 9);
  for (auto current = first5; current != last5; ++current) {
    if (*current < 0) {
      std::cerr << "Erro na selecao dos valores double nao-negativos: " << *current
                << std::endl;
    }
  }
  auto [first6, last6] = ouv_double.find_range(-10, 0);
  for (auto current = first6; current != last6; ++current) {
    if (*current >= 0) {
      std::cerr << "Erro na selecao dos valores double negativos: " << *current
                << std::endl;
    }
  }
  return 0;
}
