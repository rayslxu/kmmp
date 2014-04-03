#include "product_selection.h"

#include <string>
#include <iostream>

#define START time_start=clock()
#define END time_end=clock()
#define DURATION (time_end-time_start)/CLOCKS_PER_SEC

using std::string;
using std::cout;
using std::endl;
using std::ofstream;

enum AlgorithmType {
  EXHAUSTIVE_ALGORITHM = 0,
  GREEDY_ALGORITHM = 1,
};

void kMMP(string file_existing_products,
          string file_manufacturer_products,
          string file_candidate_products,
          string file_consumers,
          string file_log,
          vector<double> prob_distance_metrics,
          int k,
          AlgorithmType algorithm_type) {
  ProductSelection ps(file_existing_products.c_str(),
                      file_manufacturer_products.c_str(),
                      file_candidate_products.c_str(),
                      file_consumers.c_str(),
                      prob_distance_metrics[0],
                      prob_distance_metrics[1],
                      prob_distance_metrics[2],
                      prob_distance_metrics[3]);
  list<Product> res_products;
  double time_start = 0.0;
  double time_end = 0.0;
  double sales = 0.0;
  
  switch (algorithm_type) {
    case EXHAUSTIVE_ALGORITHM: {
      START;
      sales = ps.TopKExhaustiveAlgorithm(k, res_products);
      END;
      break;
    }
    case GREEDY_ALGORITHM: {
      START;
      sales = ps.TopKGreedyAlgorithm(k, res_products);
      END;
      break;
    }
  }
  
  
  ofstream output;
  output.open(file_log.c_str());
  output << sales << "\t" << DURATION << endl;
  FOR_PITR_CONST (p_itr, res_products) {
    output << p_itr->GetID() << *p_itr << endl;
  }
  output.close();
}

int main(int argc, char** argv) {
  if (argc!= 7) {
    cout << "WRONG PARAMETERS!" << endl;
    cout << "<DIRECTORY> <k> <PROBABILITY: DISTANCE METRIC 1~4>" << endl;
    return 0;
  }
  
  string directory = argv[1];

  string file_existing_products = directory + "/existing_products";
  string file_manufacturer_products = directory + "/manufacturer_products";
  string file_candidate_products = directory + "/candidate_products";
  string file_consumers = directory + "/consumers";
  
  //string suffix = "_";
  //string suffix_ms = suffix + argv[3] + "_" + argv[4] + "_" + argv[5] + "_" + argv[6];
  //string suffix_kmmp = suffix + argv[2] + "_" + suffix_kmmp;
  
  //string file_log_exhaustive_algorithm = directory + "/log_exh" + suffix_kmmp;
  //string file_log_greedy_algorithm = directory + "/log_greedy" + suffix_kmmp;

  string file_log_exhaustive_algorithm = directory + "/log_exh";
  string file_log_greedy_algorithm = directory + "/log_greedy";

  int k = atoi(argv[2]);
  
  vector<double> prob_distance_metrics;
  prob_distance_metrics.push_back(atof(argv[3]));
  prob_distance_metrics.push_back(atof(argv[4]));
  prob_distance_metrics.push_back(atof(argv[5]));
  prob_distance_metrics.push_back(atof(argv[6]));
  
  kMMP(file_existing_products,
       file_manufacturer_products,
       file_candidate_products,
       file_consumers,
       file_log_greedy_algorithm,
       prob_distance_metrics,
       k,
       (AlgorithmType) 1); 
  kMMP(file_existing_products,
       file_manufacturer_products,
       file_candidate_products,
       file_consumers,
       file_log_exhaustive_algorithm,
       prob_distance_metrics,
       k,
       (AlgorithmType) 0);

  return 1;
}
