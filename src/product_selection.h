#ifndef PRODUCT_SELECTION_H
#define PRODUCT_SELECTION_H

#include <ctime>

#include <list>
#include <fstream>

#include "consumer.h"

using std::list;

// Maximum number of consumers (array size)
const int kNumMaxConsumer = 10000;

#define PITR list<Product>::iterator
#define CITR list<Consumer>::iterator
#define PITR_CONST list<Product>::const_iterator
#define CITR_CONST list<Consumer>::const_iterator

#define FOR_PITR(itr,list) for(PITR itr=list.begin(); itr!=list.end(); ++itr)
#define FOR_CITR(itr,list) for(CITR itr=list.begin(); itr!=list.end(); ++itr)
#define FOR_PITR_CONST(itr,list) for(PITR_CONST itr=list.begin(); itr!=list.end(); ++itr)
#define FOR_CITR_CONST(itr,list) for(CITR_CONST itr=list.begin(); itr!=list.end(); ++itr)

// Product selection class
class ProductSelection {
  // Output
 friend ostream& operator<<(ostream &output, const ProductSelection &ps);
  
 public:
  // Constructors
  ProductSelection();
  ProductSelection(const char* file_existing_products,
                   const char* file_manufacturer_products,
                   const char* file_candidate_products,
                   const char* file_consumers,
                   const double prob_discrete_metric,
                   const double prob_norm_metric,
                   const double prob_price_metric,
                   const double prob_richman_metric);
                   
  // Destructor
  ~ProductSelection();

  // Top-k greedy algorithm
  double TopKGreedyAlgorithm(int k, list<Product> &result);
  
  // Top-k exhaustive algorithm
  double TopKExhaustiveAlgorithm(int k, list<Product> &result);
  
  // Calculate the real-world market share of given product(s)
  double RealMarketShare(int product_id);
  double RealMarketShare(vector<int> product_id);
  // Calculate the expected market share of given product(s) under a given model
  double ExpectedMarketShare(int product_id, DistanceType dis_type);
  double ExpectedMarketShare(vector<int> product_id, DistanceType dis_type);

 private:
  // Number of products and consumers
  int num_existing_products_;
  int num_manufacturer_products_;
  int num_candidate_products_;
  int num_consumers_;
  
  // List of products and consumers 
  list<Product> list_existing_products_;
  list<Product> list_manufacturer_products_;
  list<Product> list_candidate_products_;
  list<Consumer> list_consumers_;
  
  // Probability of each distance metrics
  vector<double> prob_distance_metrics_;
  
  // Farthest products table: 
  // Vector of the distance between consumers and their farthest distance products  
  // Vector of number of products share the farthest distance  
  // Vector of number of manufacturer's products share the farthest distance
  vector< vector<double> > vector_farthest_distance_;
  vector< vector<double> > vector_num_farthest_products_;
  vector< vector<double> > vector_num_manufacturer_products_;
  
  // Build tables for each distance metrics
  void BuildTables(); 
  // Build the farthest products table
  void TableBuilder(const DistanceType metric_id);
  // Update the table after adding a new product to the market
  void UpdateTable(Product product);
  
  // Update the list of products and consumers
  void UpdateList(Product product);
  
  // Calculate the sales of selected products and manufacturer's existing products
  double Sales(const list<Product> selected_products) const;
  // Calculate the increase of sales of caused by the selected products
  double DeltaSales(const Product product) const;
  
  // Top-1 exact algorithm 
  Product Top1ExactAlgorithm(double &max_sales) const;
  
  // Recursion function of top-k exhaust algorithm
  void TopKExhaustiveRecursion(int k, 
                               int num_selected_products,
                               double &max_sales, 
                               list<Product> &selected_products,
                               list<Product> &result);
                               
  // Find product according to id
  Product FindProduct(int product_id);
  // Generate a distance type randomly according to the probability
  DistanceType RandomDistanceType();
  
};

#endif
