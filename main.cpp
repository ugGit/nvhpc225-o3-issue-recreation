#include "component_connection_ssv.cpp"
#include "geometry/cell.hpp"
#include "geometry/measurement.hpp"

#include <tuple>

int main(){
  /*
   * Format the input data (only works for a single detector module,
   * otherwise not generic enough).
   */
  // fields: geometry_id,hit_id,channel0,channel1,timestamp,value
  using row_type = std::tuple<int, int, int, int, int, double>;  
  std::vector<row_type> raw_input{
    row_type{0,0,23,34,0,0.499423710761263},
    row_type{0,0,24,34,0,0.9664007758177955},
    row_type{0,1,36,38,0,0.42829640557804416},
    row_type{0,1,37,39,0,0.47732009631725014},
    row_type{0,1,35,39,0,0.2248546897281531},
    row_type{0,1,38,38,0,0.3300625315595242},
    row_type{0,1,36,39,0,0.7873422712754605},
    row_type{0,1,37,38,0,0.5652962370516108},
    row_type{0,1,38,37,0,0.5883027614619736},
    row_type{0,1,38,39,0,0.7959729342742885},
    row_type{0,2,24,37,0,0.20687748856071822},
    row_type{0,2,23,38,0,0.7114168040342564},
    row_type{0,2,22,38,0,0.7538067449003325},
    row_type{0,2,21,39,0,0.616867468861805},
    row_type{0,2,20,39,0,0.943257463801676},
    row_type{0,2,21,38,0,0.8774740303863138},
    row_type{0,2,21,37,0,0.5648877994772653},
    row_type{0,2,19,38,0,0.7902240673016644},
    row_type{0,2,22,37,0,0.6568738150568367},
    row_type{0,2,23,36,0,0.8099418212548222},
    row_type{0,2,23,39,0,0.2398117091069909},
    row_type{0,2,18,39,0,0.5191563508728868},
    row_type{0,2,24,39,0,0.10926985902286092},
    row_type{0,2,20,38,0,0.9218746048193104},
    row_type{0,2,19,37,0,0.14126386147806552},
    row_type{0,2,21,36,0,0.32323370512974625},
    row_type{0,2,24,38,0,0.805913992388251},
    row_type{0,2,23,37,0,0.4598338854807944},
    row_type{0,2,18,37,0,0.4011159025527057},
    row_type{0,2,22,36,0,0.9692579698585929},
    row_type{0,2,24,36,0,0.4827317338267486},
    row_type{0,2,20,36,0,0.5399171379589328},
    row_type{0,3,35,4,0,0.5715152408784197},
    row_type{0,3,35,3,0,0.20261705023364185},
    row_type{0,3,36,4,0,0.6824386418067243},
    row_type{0,3,34,3,0,0.2750526856655612},
    row_type{0,3,37,4,0,0.8187398189095577},
    row_type{0,4,17,8,0,0.9709416595429284},
    row_type{0,4,16,8,0,0.7518192332939613}
  };
  
  // transform the raw input into cell objects
  vecmem::vector<traccc::cell> cells;
  cells.reserve(raw_input.size());
  for(int i=0; i<raw_input.size(); i++){
    traccc::cell c {};
    c.channel0 = std::get<2>(raw_input[i]);
    c.channel1 = std::get<3>(raw_input[i]);
    c.time = std::get<4>(raw_input[i]);
    c.activation = std::get<5>(raw_input[i]);
    cells.push_back(c);
  }

  // sort the vector based on channel1, if equal by channel0
  std::sort(cells.begin(), cells.end(), [](const traccc::cell& a, const traccc::cell& b){
    return a.channel1 != b.channel1 ? a.channel1 < b.channel1 : a.channel0 < b.channel0;
  });

  // prepare the function input data
  traccc::cell_container_types::host data;
  data.push_back(traccc::cell_module{0}, cells);

  // print out the prepared input data
  for(int i=0; i < data.size(); i++){
    printf("-----------------\n");
    auto module = data.at(i);
    printf("Module %d: %d activations\n", module.header.module, module.items.size());
    for(int j=0; j < module.items.size(); j++){
      auto item = module.items.at(j);
      printf("Activation %d: channel0=%d; channel1=%d; activation=%f\n", j, item.channel0, item.channel1, item.activation);
    }
  }
  
  /*
   * Execute the algorithm
   */
  traccc::measurement_container_types::host res = traccc::stdpar::component_connection_ssv(data);

  /*
   * Print out the result
   */
  for(int i=0; i < res.size(); i++){
    printf("-----------------\n");
    auto module = res.at(i);
    printf("Module %d: %d hits\n", module.header.module, module.items.size());
    for(int j=0; j < module.items.size(); j++){
      auto item = module.items.at(j);
      printf("Hit %d: channel0=%f; channel1=%f; variance0=%f; variance1=%f\n", j, item.local[0], item.local[1], item.variance[0], item.variance[1]);
    }
  }

  return 0;
}
