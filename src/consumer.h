#ifndef CONSUMER_H
#define CONSUMER_H

#include "product.h"

const double kEpsilon = 0.00001;

// Type of distance metrics: 
//   discrete metric, norm metric, price metric, richman metric
enum DistanceType {
  DISCRETE_METRIC = 0,
  NORM_METRIC = 1,
  PRICE_METRIC = 2,
  RICHMAN_METRIC = 3,
};

// Consumer class
class Consumer
{
  // IO
  friend istream &operator>>(istream &input, Consumer &consumer);
  friend ostream &operator<<(ostream &output, const Consumer &consumer);
  
 public:
  // Constructors
  Consumer();
  Consumer(int id);
  // Destructor
  ~Consumer();

  // Get the id of the consumer
  int GetID() const;
  // Whether the consumer is satisfied by the product
  bool IsSatisfiedBy(const Product &product) const;
  // Calculate the distance between the consumer and the product
  double DistanceFrom(const Product &product, 
                      const DistanceType distance_type) const;
  
 private:
  // Consumer id: 0 ~ |C|
  int id_; 
  // requirement_[i]: the requirement on attribute i
  vector<double> requirement_; 
};

#endif
