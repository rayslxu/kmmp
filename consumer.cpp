#include "consumer.h"

// Constructor
Consumer::Consumer() {
  this->id_ = -1;
}

// Constructor with consumer id
Consumer::Consumer(int id) {
  this->id_ = id;
}

// Destructor 
Consumer::~Consumer() {

}

// Return the id of the consumer
int Consumer::GetID() const {
  return this->id_;
}

// Judge whether the consumer is satisfied by the product
//   Return true if quality of the product is better (>) than the requirement
//   of the consumer on all attributes
bool Consumer::IsSatisfiedBy(const Product &product) const {
  for (int i = 0; i < num_attribute; ++i) {
    if (this->requirement_[i] >= product.quality_[i] - kEpsilon) {
      return false;
    }
  }
  return true;
}

// Calculate the distance between the consumer and the product
//    Return the distance according to the distance metrics and the weight type
double Consumer::DistanceFrom(const Product &product, 
                              const DistanceType distance_type) const {
  double res = 0;
  switch (distance_type) {
    // Discrete metric
    case DISCRETE_METRIC: { 
      res = 1;
      break;
    }
    // Norm metric
    case NORM_METRIC: {
      for (int i = 0; i < num_attribute; ++i)
        res += product.quality_[i] - this->requirement_[i];
      break;
    }
    // Price metric
    //   We set the first attribute as the "price" attribute
    case PRICE_METRIC: {
        res = product.quality_[0] - this->requirement_[0];
      break;
    }
    // Richman metric
    case RICHMAN_METRIC: {
      for (int i = 1; i < num_attribute; ++i)
        res += product.quality_[i] - this->requirement_[i]; 
      break;
    }
    // Error: not a pre-defined type of distance metrics */
    default: {
      std::cerr << "ERROR: WRONG DISTANCE METRIC!" << std::endl;
      exit(0);
      break;
    }
  }
  return res;  
}

// input stream 
istream &operator >> (istream &input, Consumer &consumer) {
  for (int i = 0; i < num_attribute; ++i) {
    double requirement;
    input >> requirement;
    consumer.requirement_.push_back(requirement);
  }
  return input;
}

// output stream
ostream &operator << (ostream &output, const Consumer &consumer) {
  output << "(";
  for (int i = 0; i < num_attribute; ++i) {
    output << consumer.requirement_[i] << " ";
  }
  output << "\b)";
  return output;
}
