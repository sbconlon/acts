#include<torch/torch.h>

template <typename spacepoint_container_t>
torch::Tensor hits_to_tensor(spacepoint_container_t& sps) {
  // This function translates a container of external spacepoints into a
  // torch tensor of shape (number of hits, 7) where the columns are 
  // (hit id, x, y, z, ch0, ch1, value) 
  torch::Tensor res = torch::empty({sps.size(), 7});
  auto res_a = res.accessor<float, 2>();
  for(size_t i=0; i<res_a.size(0); ++i){
    res_a[i][0] = sps[i].measurementIndex(); // hit_id
    res_a[i][1] = sps[i].x();                // x
    res_a[i][2] = sps[i].y();                // y
    res_a[i][3] = sps[i].z();                // z
    res_a[i][4] = 0;                         // ch0   TODO: find real value
    res_a[i][5] = 0;                         // ch1   TODO: find real value
    res_a[i][6] = 0;                         // value TODO: find real value
  }
  return res;
}
