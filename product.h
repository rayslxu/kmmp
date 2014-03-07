#ifndef PRODUCT_H
#define PRODUCT_H

#include <cstdlib>
#include <iostream>
#include <vector>

using std::istream;
using std::ostream;
using std::vector;

// Maximum number of attributes for each product/customer (array size)
const int kNumMaxAttribute = 10;

// The number of attributes for each product/customer
// defined in product_selection.cc
extern int num_attribute;

// Types of products: existing product, candidate new product 
enum ProductType {
  EXISTING_PRODUCT = 0, 
  MANUFACTURER_PRODUCT = 1,
  CANDIDATE_PRODUCT = 2, 
};

// Product class
class Product {
  friend class Consumer;
  // IO
  friend istream& operator>>(istream &input, Product &product);
  friend ostream& operator<<(ostream &output, const Product &product);
  
 public:
  // Constructors
  Product();
  Product(int id, ProductType type);
  // Destructor
  ~Product();
  
  // Get the id of the product
  int GetID() const;
  // Get the type of the product
  ProductType GetType() const;  
  // Change the type of product to manufacturer's product
  void ChangeType();
  
  bool operator==(const Product &product) const; 
  
 private:
  // Product id: 0 ~ (|Pm| + |Pn|)
  int id_;
  // Product type
  ProductType type_;    
  // quality_[i]: the quality of attribute i
  vector<double> quality_;
};


#endif
