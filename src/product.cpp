#include "product.h"

// Constructor with no argument
Product::Product() {
  this->id_ = -1;
}

// Constructor with product id and product type
Product::Product(int id, ProductType type) {
  this->id_ = id;
  this->type_ = type;
}

// Destructor
Product::~Product() {
  
}

// Return the id of the product
int Product::GetID() const {
  return this->id_;
}

// Get the type of the product
ProductType Product::GetType() const {
  return this->type_;
}

// Change the type of product to manufacturer's product
void Product::ChangeType() {
  this->type_ = MANUFACTURER_PRODUCT;
}

// Equal sign overloading 
bool Product::operator==(const Product &product) const {
  return this->id_ == product.id_ && this->type_ == product.type_;
}

// Input stream 
istream &operator>>(istream &input, Product &product) {
  for (int i = 0; i < num_attribute; ++i) {
    double quality;
    input >> quality;
    product.quality_.push_back(quality);
  }
  return input;
}

// Output stream
ostream &operator<<(ostream &output, const Product &product) {
  output << "(";
  output << product.quality_[0];
  for (int i = 1; i < num_attribute; ++i) {
    output << " " << product.quality_[i];
  }
  output << ")";
  return output;
}

