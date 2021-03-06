/*Programa que dado um arquivo de entrada com um conjunto de valores,
calcula a média, o desvio padrão e um histograma desses valores.
*/

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <array>
#include <tuple>

//template das funcoes
std::vector<double> read_file(char const *filename);
std::array<double, 2> estat_data(std::vector<double> const &data);
std::tuple<std::vector<int>, std::vector<double>> box_histogram(std::vector<double> const &data, int B);

int main(int, char const *args[]) {
  std::vector<double> vector_data;

  //Recebe os parametros
  int B = std::stoi(args[2]);
  vector_data = read_file(args[1]); //chama a funcao de ler as linhas
  
  //chama as funcoes do histograma e calculo media e desvio padrao
  auto [mean, stdev] = estat_data(vector_data);
  auto [count_box, informacao_box] = box_histogram(vector_data, B);

  //Print dos resultados
  std::cout << vector_data.size() << std::endl; //numero de elementos
  std::cout << mean << std::endl; //media
  std::cout << stdev << std::endl; //desvio padrao

  //Print do resultado do histograma separado por " "
  for(int i = 0; i < B; ++i){
      std::cout << informacao_box[i] << " " << informacao_box[i + 1] << " " << count_box[i] << std::endl;
  }

  return 0;
}


std::vector<double> read_file(char const *filename) {
  std::vector<double> data;
  std::ifstream file(filename);
  double val;

  //Ler Linhas e as guarda no vetor
  while (file >> val) {
    data.push_back(val);
  }

  return data;
}

std::array<double,2> estat_data(std::vector<double> const &data) {
  double mean{0}, stdev{0};

  //Media
  for (auto x: data) {
    mean += x;
  }
  mean /= data.size();

  //Desvio padrão
  for (auto x: data) {
    stdev += pow(x - mean, 2);
  }
  stdev /= (data.size() - 1);
  stdev = pow(stdev, 0.5);

  return {mean, stdev};
}

std::tuple<std::vector<int>, std::vector<double>> box_histogram(std::vector<double> const &data, int B){
  std::vector<int> count(B);
  std::vector<double> info(B + 1);
  double box_size;
  double max{data[0]}, min{data[0]};
  int k;

  //Acha o max e min
  for (auto x: data){
    if(max < x) max = x;
    if(min > x) min = x;
  }

  box_size = (max - min)/B;
  
  //Computa a que caixa ele pertence
  for (auto x: data) {
    if (x != max) {
      k = floor((x - min)/box_size);
      ++count[k];
    }
    else ++count[B - 1];
  }

  //gera o vetor informacao das caixas
  for (int i = 0; i <= B; ++i) {
    info[i] = min + box_size*i;
  }

  return {count, info};
}