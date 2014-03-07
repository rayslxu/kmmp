#include "product_selection.h"

using std::fstream;
using std::cout;
using std::endl;

// Number of attribute for each product/customer
int num_attribute = 0;

//Constructor with no argument
ProductSelection::ProductSelection() {
  this->num_existing_products_ = 0;
  this->num_manufacturer_products_ = 0;
  this->num_candidate_products_ = 0;
  this->num_consumers_ = 0;
  this->prob_distance_metrics_.push_back(1.0);
  this->prob_distance_metrics_.push_back(0.0);
  this->prob_distance_metrics_.push_back(0.0);
  this->prob_distance_metrics_.push_back(0.0);
}

// Constructor: 
//  read files of products and consumers
//  read parameters: type of distance metrics and attributes' weight
ProductSelection::ProductSelection(const char* file_existing_products,
                                   const char* file_manufacturer_products,
                                   const char* file_candidate_products,
                                   const char* file_consumers,
                                   const double prob_discrete_metric,
                                   const double prob_norm_metric,
                                   const double prob_price_metric,
                                   const double prob_richman_metric) {
  fstream fin;
  
  // Read the existing products from file
  fin.open(file_existing_products, std::fstream::in);
  if (!fin) {
    cout << "ERROR: CANNOT OPEN FILE OF EXISTING PRODUCTS!" << endl;
    exit(-1);
  }
  fin >> this->num_existing_products_ >> num_attribute;
  cout << "Existing products:\t" << this->num_existing_products_ << endl
       << "Attributes:\t\t" << num_attribute << endl;
  for (int i = 0; i < this->num_existing_products_; ++i) {
    Product existing_product(i, EXISTING_PRODUCT);
    fin >> existing_product;
    this->list_existing_products_.push_back(existing_product);
  }
  fin.close();
  fin.clear();

  // Read the manufacturers' existing products from file
  fin.open(file_manufacturer_products, std::fstream::in);
  if (!fin) {
    cout << "ERROR: CANNOT OPEN FILE OF MANUFACTURER'S EXISTING PRODUCTS!" << endl;
    exit(-1);
  }
  fin >> this->num_manufacturer_products_;
  cout << "Manufacturer products:\t" << this->num_manufacturer_products_ << endl;
  for (int i = 0; i < this->num_manufacturer_products_; ++i) {
    int id_manufacturer_product;
    fin >> id_manufacturer_product;
    FOR_PITR (p_itr, this->list_existing_products_) {
      if (p_itr->GetID() == id_manufacturer_product) {
        p_itr->ChangeType();
        this->list_manufacturer_products_.push_back(*p_itr);
      }
    }
  }
  fin.close();
  fin.clear();
  
  // Read the candidate new products from file
  fin.open(file_candidate_products, std::fstream::in);
  if (!fin) {
    cout << "ERROR: CANNOT OPEN FILE OF CANDIDATE NEW PRODUCTS!" << endl;
    exit(-1);
  }
  double tmp_num_attribute;
  fin >> this->num_candidate_products_ >> tmp_num_attribute;
  if (tmp_num_attribute != num_attribute) {
    // Error: the number of attributes is inconsistent 
    cout << "ERROR: INCONSISTENT NUMBER OF ATTRIBUTES!" << endl;
    exit(-1);
  }
  cout << "Candidate products:\t" << this->num_candidate_products_ << endl;
  for (int i = 0; i < this->num_candidate_products_; ++i) {
    Product candidate_product(this->num_existing_products_+i, CANDIDATE_PRODUCT);
    fin >> candidate_product;
    this->list_candidate_products_.push_back(candidate_product);
  }
  fin.close();
  fin.clear();

  // Read consumers from file
  fin.open(file_consumers, std::fstream::in);
  if (!fin) {
    cout << "ERROR: CANNOT OPEN FILE OF CONSUMERS!" << endl;
    exit(-1);
  }
  fin >> this->num_consumers_ >> tmp_num_attribute;
  if (tmp_num_attribute != num_attribute) {
    // Error: the number of attributes is inconsistent 
    cout << "ERROR: INCONSISTENT NUMBER OF ATTRIBUTES!" << endl;
    exit(-1);
  }
  cout << "Consumers:\t\t" << this->num_consumers_ << endl;
  for (int i = 0; i < this->num_consumers_; ++i) {
    Consumer consumer(i);
    fin >> consumer;
    this->list_consumers_.push_back(consumer);
  }
  fin.close();
  fin.clear();
  
  // Set probability of each distance metrics
  double sum_prob = prob_discrete_metric + prob_norm_metric +
                    prob_price_metric + prob_richman_metric;
  if (sum_prob - 1.0 > kEpsilon || sum_prob - 1.0 < -kEpsilon) {
    // Error: the summation of the probability is not 1
    cout << "ERROR: WRONG PROBABILITY OF DISTANCE METRICS! " 
         << sum_prob << endl;
    exit(-1);
  } else {
    this->prob_distance_metrics_.push_back(prob_discrete_metric);
    this->prob_distance_metrics_.push_back(prob_norm_metric);
    this->prob_distance_metrics_.push_back(prob_price_metric);
    this->prob_distance_metrics_.push_back(prob_richman_metric);
  }
  
  // Initialize the farthest product tables
  for (int i = 0; i < 4; ++i) {
    vector<double> row1, row2, row3;
    for (int j = 0; j < this->num_consumers_; ++j) {
      row1.push_back(-1.0);
      row2.push_back(0.0);
      row3.push_back(0.0);
    }
    this->vector_farthest_distance_.push_back(row1);
    this->vector_num_farthest_products_.push_back(row2);
    this->vector_num_manufacturer_products_.push_back(row3);
  }
  
  cout << endl << "Read file success!" << endl << endl;
}

//Destructor
ProductSelection::~ProductSelection() {
  
}

// Top-k exhaustive algorithm
//   Select k products from candidate new products, which stored in result.
//   Return the expected sales of the selected k products.
double ProductSelection::TopKExhaustiveAlgorithm(int k, list<Product> &result) {
	cout << "Exhaustive search algorithm start!" << endl << endl; 
  list<Product> selected_products;
  double max_sales = this->Sales(result);
  cout << "Original sales: " << max_sales << endl << endl;
  this->TopKExhaustiveRecursion(k, 0, max_sales, selected_products, result); 
  cout << "Maximum sales: " << max_sales << endl << endl;
  cout << "Result: " << endl;
  FOR_PITR (p_itr, result) {
    cout << p_itr->GetID() << ": " << *p_itr << endl;
    this->UpdateList(*p_itr);
  }
  cout << endl << "Exhaustive search algorithm end!" << endl << endl;
  return max_sales;
}


// Recursion function of top-k exhaustive algorithm
void ProductSelection::TopKExhaustiveRecursion(int k, 
                                               int num_selected_products,
                                               double &max_sales, 
                                               list<Product> &selected_products,
                                               list<Product> &result) {
  if (num_selected_products == k) {
    // If we have already selected k products,
    // calculate the sales, if larger than the maximum sales till now, record it
    double sales = this->Sales(selected_products);
    if (sales > max_sales) {
      max_sales = sales;
      result.clear();
      FOR_PITR_CONST (p_itr, selected_products) {
        result.push_back(*p_itr);
      }
    }
  } else if (num_selected_products < k) {
    // If we have selected less than k products, go on selecting the i+1 product 
    FOR_PITR_CONST (new_p_itr, this->list_candidate_products_) {
      bool thereIsNoDuplicate = true;
      // Check whether the new product has been selected
      FOR_PITR_CONST (selected_p_itr, selected_products) {
        if (new_p_itr->GetID() == selected_p_itr->GetID()) {
          thereIsNoDuplicate = false;
          break;
        }
      }
      // If successfully select i+1 products, go to recursion i+1
      if (thereIsNoDuplicate) {
        selected_products.push_back(*new_p_itr);
        this->TopKExhaustiveRecursion(k, 
                                      num_selected_products + 1,
                                      max_sales, 
                                      selected_products,
                                      result);
        selected_products.pop_back();
      }
    }
  } else {
    //Error: Select more than k products
    cout << "ERROR: SELECT MORE THAN K PRODUCTS!" << endl;
    exit(0);  
  }
}


// Top-k greedy algorithm
double ProductSelection::TopKGreedyAlgorithm(int k, list<Product> &result) {
  cout << "Greedy algorithm start!" << endl << endl; 
  double delta_sales = 0.0;
  // Initialize the sales by the manufacturers' existing products' sales
  double sales = this->Sales(result);
  cout << "Original sales: " << sales << endl << endl;
  this->BuildTables();
  int i = 0;
  for (i = 0; i < k; ++i) {
    cout << "Look for top " << i+1 << endl;    
    // Look for top 1 products
    double max_delta_sales = 0.0;
    Product top1_product = this->Top1ExactAlgorithm(max_delta_sales);
     
    // If the id of the found product is -1, which means no more product 
    // can improve the sales, return the current result
    if(top1_product.GetID() == -1) {
      cout << "No more products can improve the sales" << endl << endl;
      break;
    } else {
      cout << "Found top " << i+1 << " : ";
      cout << top1_product.GetID() << " " << top1_product << endl;
      cout << "Increase of sales: " << max_delta_sales << endl << endl;      
      result.push_back(top1_product);
      // Update the product lists
      this->UpdateList(top1_product);
      // Update the farthest products table
      this->UpdateTable(top1_product);
      delta_sales += max_delta_sales;
    }
  }
  cout << "Found " << i << " products!" << endl;
  sales += delta_sales;
  cout << "Maximum sales: " << sales << endl;
  cout << endl << "Greedy algorithm end!" << endl << endl;
  return sales;
}

// top1 exact algorithm
Product ProductSelection::Top1ExactAlgorithm(double &max_delta_sales) const {
  Product top1_product(-1, CANDIDATE_PRODUCT);
  FOR_PITR_CONST (p_itr, this->list_candidate_products_) {
    double delta_sales = this->DeltaSales(*p_itr);
    if (delta_sales > max_delta_sales) {
      top1_product = *p_itr;
      max_delta_sales = delta_sales;
    }
  }
  return top1_product;
}


// Calculate the sales of selected products and manufacturer's existing products
double ProductSelection::Sales(const list<Product> selected_products) const {
  double sales[4] = {0.0};
  for (int i = 0; i < 4; ++i) {
    // if the probability of distance metric i is 0, 
    // then there's no need to calculate the sales under this model
    if (this->prob_distance_metrics_[i] == 0.0) continue;
    DistanceType distance_type = (DistanceType) i;
    FOR_CITR_CONST (c_itr, this->list_consumers_) {
      double farthest_distance = -1.0;
      double num_products = 0.0;
      double num_manufacturer_products = 0.0;
      FOR_PITR_CONST (p_itr, this->list_existing_products_) {
        if (c_itr->IsSatisfiedBy(*p_itr)) {
          double distance = c_itr->DistanceFrom(*p_itr, distance_type);
          if (distance > farthest_distance) {
            farthest_distance = distance;
            num_products = 1.0;
            if (p_itr->GetType() == MANUFACTURER_PRODUCT) {
              num_manufacturer_products = 1.0;
            } else {
              num_manufacturer_products = 0.0;
            }
          } else if (distance == farthest_distance) {
            num_products += 1.0;
            if (p_itr->GetType() == MANUFACTURER_PRODUCT) {
              num_manufacturer_products += 1.0;
            }
          }
        }
      }
      FOR_PITR_CONST (p_itr, selected_products) {
        if (c_itr->IsSatisfiedBy(*p_itr)) {
          double distance = c_itr->DistanceFrom(*p_itr, distance_type);
          if (distance > farthest_distance) {
            farthest_distance = distance;
            num_products = num_manufacturer_products = 1.0;
          }
          else if (distance == farthest_distance) {
            num_products += 1.0;
            num_manufacturer_products += 1.0;
          }
        }
      }
      if (num_products != 0) {
        sales[i] += num_manufacturer_products / num_products;
      } 
    }
  }
  double expected_sales = 0.0;
  for (int i = 0; i < 4; ++i) {
    expected_sales += this->prob_distance_metrics_[i] * sales[i];
  }
  return expected_sales;
}

// Calculate the increase of sales of caused by the selected products
double ProductSelection::DeltaSales(const Product product) const {
  double delta_sales[4] = {0.0};
  for (int i = 0; i < 4; ++i) {
    // if the probability of distance metric i is 0, 
    // then there's no need to calculate the sales under this model
    if (this->prob_distance_metrics_[i] == 0.0) continue;
    DistanceType distance_type = (DistanceType) i;
    FOR_CITR_CONST (c_itr, this->list_consumers_) {
      if (c_itr->IsSatisfiedBy(product)) {
        double distance = c_itr->DistanceFrom(product, distance_type);
        int id = c_itr->GetID();
        double farthest_distance = this->vector_farthest_distance_[i][id];
        double num_total = this->vector_num_farthest_products_[i][id];
        double num_manufacturer = this->vector_num_manufacturer_products_[i][id];
        if (num_total != 0.0) {
          if (distance > farthest_distance) {
            delta_sales[i] += 1.0 - num_manufacturer / num_total;
          } else if (distance == farthest_distance) {
            delta_sales[i] += (num_manufacturer + 1.0) / (num_total + 1.0);
            delta_sales[i] -= num_manufacturer / num_total;
            
          }
        } else {
          delta_sales[i] += 1.0;
        }
      } 
    }
  }
  double expected_delta_sales = 0.0;
  for (int i = 0; i < 4; ++i) {
    expected_delta_sales += this->prob_distance_metrics_[i] * delta_sales[i];
  }
  return expected_delta_sales;
}

// Build the farthest products table
void ProductSelection::TableBuilder(const DistanceType distance_type) {
  int m_id = (int) distance_type;
  FOR_CITR_CONST (c_itr, this->list_consumers_) {
    int c_id = c_itr->GetID();
    FOR_PITR_CONST (p_itr, this->list_existing_products_) {
      if (c_itr->IsSatisfiedBy(*p_itr)) {
        double distance = c_itr->DistanceFrom(*p_itr, distance_type);
        if (distance > this->vector_farthest_distance_[m_id][c_id]) {
          this->vector_farthest_distance_[m_id][c_id] = distance;
          this->vector_num_farthest_products_[m_id][c_id] = 1.0;
          if (p_itr->GetType() == MANUFACTURER_PRODUCT) {
            this->vector_num_manufacturer_products_[m_id][c_id] = 1.0;
          } else {
            this->vector_num_manufacturer_products_[m_id][c_id] = 0.0;
          }
        } else if (distance == this->vector_farthest_distance_[m_id][c_id]) {
          this->vector_num_farthest_products_[m_id][c_id] += 1.0;
          if (p_itr->GetType() == MANUFACTURER_PRODUCT) {
            this->vector_num_manufacturer_products_[m_id][c_id] += 1.0;
          }
        }
      }
    }
  }
}

// Build tables for each distance metrics
void ProductSelection::BuildTables() {  
  for (int i = 0; i < 4; ++i) {
    // Initialize the farthest distance table
    for (int j = 0; j < this->num_consumers_; ++j) {
      this->vector_farthest_distance_[i][j] = -1.0;
      this->vector_num_farthest_products_[i][j] = 0.0;
      this->vector_num_manufacturer_products_[i][j] = 0.0;
    }
    // Build farthest product table for each distance metrics
    this->TableBuilder((DistanceType) i);
  }
}

// Update the table after adding a new product to the market
void ProductSelection::UpdateTable(Product product) {
  for(int i = 0; i < 4; ++i) {
    DistanceType distance_type = (DistanceType) i;
    FOR_CITR_CONST (c_itr, this->list_consumers_) {
      if (c_itr->IsSatisfiedBy(product)) {
        int id = c_itr->GetID();
        double distance = c_itr->DistanceFrom(product, distance_type);
        if (distance > this->vector_farthest_distance_[i][id]) {
          this->vector_farthest_distance_[i][id] = distance;
          this->vector_num_farthest_products_[i][id] = 1.0;
          this->vector_num_manufacturer_products_[i][id] = 1.0;
        } else if (distance == this->vector_farthest_distance_[i][id]) {
          this->vector_num_farthest_products_[i][id] += 1.0;
          this->vector_num_manufacturer_products_[i][id] += 1.0;
        }
      }
    }
  }
}

// Update the list of products and consumers 
void ProductSelection::UpdateList(Product product) {
  // Remove the product from candidate new product set
  this->list_candidate_products_.remove(product);
  // Change the type of the product to manufacturer's existing product
  product.ChangeType();
  // Add the product to manufacturer's product set
  this->list_existing_products_.push_back(product);
  this->list_manufacturer_products_.push_back(product);
}

// Calculate the expected market share of ONE product under a given model
double ProductSelection::ExpectedMarketShare(int product_id, 
                                             DistanceType dis_type) {
  Product product = FindProduct(product_id);
  if (product.GetID() == -1) {
    cout << "ERROR: CANNOT FIND THE PRODUCT!" << endl;
    exit(-1);
  }
  double sales = 0.0;
  FOR_CITR_CONST (citr, this->list_consumers_) {
    if (citr->IsSatisfiedBy(product)) {
      double distance = citr->DistanceFrom(product, dis_type);
      double farthest_distance = -1.0;
      double sum_farthest_products = 0.0;
      FOR_PITR_CONST (pitr, this->list_existing_products_) {
        if (citr->IsSatisfiedBy(*pitr)) {
          double dis_tmp = citr->DistanceFrom(*pitr, dis_type);
          if (dis_tmp > distance) {
            farthest_distance = dis_tmp;
            break;
          } else if (dis_tmp > farthest_distance) {
            farthest_distance = dis_tmp;
            sum_farthest_products = 1.0;
          } else if (dis_tmp == farthest_distance) {
            sum_farthest_products += 1.0;
          }
        }
      }
      if (distance == farthest_distance) {
        if (product.GetType() == CANDIDATE_PRODUCT) {
          sales += 1.0 / (1 + sum_farthest_products);
        } else {
          sales += 1.0 / sum_farthest_products;
        }
      } else if (distance > farthest_distance) {
        sales += 1.0;
      }
    }
  }
  return sales / this->num_consumers_;
}

// Calculate the expected market share of a list of CANDIDATE 
// products under a given model
double ProductSelection::ExpectedMarketShare(vector<int> product_id, 
                                             DistanceType dis_type) {
  double sales = 0.0;
  FOR_CITR_CONST (citr, this->list_consumers_) {
    double farthest_distance = -1.0;
    double sum_query = 0.0;
    double sum_total = 0.0;
    FOR_PITR_CONST (pitr, this->list_existing_products_) {
      if (citr->IsSatisfiedBy(*pitr)) {
        double dis = citr->DistanceFrom(*pitr, dis_type);
        if (dis > farthest_distance) {
          farthest_distance = dis;
          sum_total = 1.0;
        } else if (dis == farthest_distance) {
          sum_total += 1.0;
        }
      }
    }
    for(int i = 0; i < product_id.size(); ++i) {
      Product product = this->FindProduct(product_id[i]);
      if (product.GetID() == -1) {
        cout << "ERROR: CANNOT FIND THE PRODUCT!" << endl;
        exit(-1);
      }
      if (citr->IsSatisfiedBy(product)) {
        double dis = citr->DistanceFrom(product, dis_type);
        if (dis > farthest_distance) {
          farthest_distance = dis;
          sum_query = sum_total = 1.0;
        } else if (dis == farthest_distance) {
          sum_query += 1.0;
          sum_total += 1.0;
        }
      }
    }
    if (sum_total != 0) {
      sales += sum_query / sum_total;
    }
  }
  return sales / this->num_consumers_;
}

// Calculate the real-world market share of ONE product
double ProductSelection::RealMarketShare(int product_id) {
  Product product = FindProduct(product_id);
  if (product.GetID() == -1) {
    cout << "ERROR: CANNOT FIND THE PRODUCT!" << endl;
    exit(-1);
  }
  double sales = 0.0;
  std::srand(std::time(0));
  FOR_CITR_CONST (citr, this->list_consumers_) {
    if (citr->IsSatisfiedBy(product)) {
      DistanceType dis_type = this->RandomDistanceType();
      double distance = citr->DistanceFrom(product, dis_type);
      double farthest_distance = -1.0;
      double sum_farthest_products = 0.0;
      FOR_PITR_CONST (pitr, this->list_existing_products_) {
        if (citr->IsSatisfiedBy(*pitr)) {
          double dis_tmp = citr->DistanceFrom(*pitr, dis_type);
          if (dis_tmp > distance) {
            farthest_distance = dis_tmp;
            break;
          } else if (dis_tmp > farthest_distance) {
            farthest_distance = dis_tmp;
            sum_farthest_products = 1.0;
          } else if (dis_tmp == farthest_distance) {
            sum_farthest_products += 1.0;
          }
        }
      }
      if (distance == farthest_distance) {
        if (product.GetType() == CANDIDATE_PRODUCT) {
          sales += 1.0 / (1 + sum_farthest_products);
        } else {
          sales += 1.0 / sum_farthest_products;
        }
      } else if (distance > farthest_distance) {
        sales += 1.0;
      }
    }
  }
  return sales / this->num_consumers_;
}

// Calculate the real market share of a list of CANDIDATE 
// products under a given model
double ProductSelection::RealMarketShare(vector<int> product_id) {
  double sales = 0.0;
  std::srand(std::time(0));
  FOR_CITR_CONST (citr, this->list_consumers_) {
    double farthest_distance = -1.0;
    double sum_query = 0.0;
    double sum_total = 0.0;
    DistanceType dis_type = this->RandomDistanceType();
    FOR_PITR_CONST (pitr, this->list_existing_products_) {
      if (citr->IsSatisfiedBy(*pitr)) {
        double dis = citr->DistanceFrom(*pitr, dis_type);
        if (dis > farthest_distance) {
          farthest_distance = dis;
          sum_total = 1.0;
        } else if (dis == farthest_distance) {
          sum_total += 1.0;
        }
      }
    }
    for(int i = 0; i < product_id.size(); ++i) {
      Product product = this->FindProduct(product_id[i]);
      if (product.GetID() == -1) {
        cout << "ERROR: CANNOT FIND THE PRODUCT!" << endl;
        exit(-1);
      }
      if (citr->IsSatisfiedBy(product)) {
        double dis = citr->DistanceFrom(product, dis_type);
        if (dis > farthest_distance) {
          farthest_distance = dis;
          sum_query = sum_total = 1.0;
        } else if (dis == farthest_distance) {
          sum_query += 1.0;
          sum_total += 1.0;
        }
      }
    }
    if (sum_total != 0) {
      sales += sum_query / sum_total;
    }
  }
  return sales / this->num_consumers_;
}

// Return the product according to id
Product ProductSelection::FindProduct(int product_id) {
  Product product(-1, CANDIDATE_PRODUCT);
  FOR_PITR_CONST (pitr, this->list_existing_products_) {
    if (pitr->GetID() == product_id) {
      product = *pitr;
      break;
    }
  }
  if (product.GetID() != product_id) {
    FOR_PITR_CONST (pitr, this->list_candidate_products_) {
      if (pitr->GetID() == product_id) {
        product = *pitr;
        break;
      }
    }
  }
  return product;
}


// Generate a distance type randomly according to the probability
DistanceType ProductSelection::RandomDistanceType() {
  DistanceType dis_type;
  double random_metric = ((double) std::rand() / RAND_MAX);
  if (random_metric >= 1- this->prob_distance_metrics_[3]) {
    dis_type = RICHMAN_METRIC;
  } else if (random_metric >= this->prob_distance_metrics_[0] + 
                              this->prob_distance_metrics_[1]) {
    dis_type = PRICE_METRIC;
  } else if (random_metric >= this->prob_distance_metrics_[0]) {
    dis_type = NORM_METRIC;
  } else {
    dis_type = DISCRETE_METRIC;
  }
  return dis_type;
}

// Output stream
ostream& operator<<(ostream &output, const ProductSelection &ps) {
  
  output << "Existing Products:" << endl;
  FOR_PITR_CONST (itr, ps.list_existing_products_) {
    output << *itr <<endl;
  }
  output << "Manufacturer's Products:" <<endl;
  FOR_PITR_CONST (itr, ps.list_manufacturer_products_) {
    output << *itr <<endl;
  } 
  output << "New Products:" << endl;
  FOR_PITR_CONST (itr, ps.list_candidate_products_) {
    output << *itr << endl;
  }
  output << "Consumers:" << endl;
  FOR_CITR_CONST (itr, ps.list_consumers_) {
    output << *itr << endl;
  }
  return output;
}



