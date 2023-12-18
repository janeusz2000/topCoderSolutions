#include <gmock/gmock.h>
#include <vector>
 
// #1 int stores drake's stomach capacity
// #2 drake eats cakes (their sizes are the elements of the int[cakes])
// #3 as long as drake is hungry it eats a cake - even if it exceeds its stomach capacity
//
// We must return the maximum total size of cakes you can feed to the drake

int howBigOfACakeCanDrakeEat(int capacity, std::vector<int> cakes, int excess) {
  std::sort(cakes.begin(), cakes.end());

  excess++;
  int ans = 0;
  while(excess && !cakes.empty()) {
    ans += cakes.back();
    excess--;
    cakes.pop_back();
  }

  std::vector<int> possibleSums = {cakes[0]};
  if (ans == cakes[0] && ans >= capacity && excess == 0) {
    return ans;
  }

  for (int i = 1; i < cakes.size(); i++) {
    if (cakes[i] < capacity) {
      std::vector<int> newSums = {cakes[i]};
      for (const auto number: possibleSums) {
        if (number + cakes[i] < capacity) {
          newSums.push_back(number + cakes[i]);
        }
      }
      std::copy(newSums.begin(), newSums.end(), std::back_inserter(possibleSums));
    }
  }

  std::sort(possibleSums.begin(), possibleSums.end());
  std::cout << "[";
  for (const auto& elem : possibleSums) {
    std::cout << elem << ", ";
  }
  std::cout << std::endl;
  return *possibleSums.rbegin() + ans;
}

TEST(HowMuchDragonCanEat, basicCase0) {
  ASSERT_EQ(howBigOfACakeCanDrakeEat(1234, {10, 20, 30, 40}, 0), 100);
}

TEST(HowMuchDragonCanEat, basicCase1) {
  ASSERT_EQ(howBigOfACakeCanDrakeEat(100, {100, 100, 100}, 0), 100);
}

TEST(HowMuchDragonCanEat, basicCase2) {
  ASSERT_EQ(howBigOfACakeCanDrakeEat(101, {100, 100, 100}, 0), 200);
}

TEST(HowMuchDragonCanEat, basicCase3) {
  ASSERT_EQ(howBigOfACakeCanDrakeEat(103, {100, 100, 100, 100, 100}, 2), 400);
}

TEST(HowMuchDragonCanEat, basicCase4) {
  ASSERT_EQ(howBigOfACakeCanDrakeEat(4700, {1000, 8000, 2000, 5000, 3000}, 0), 12000);
}

TEST(HowMuchDragonCanEat, basicCase5) {
  ASSERT_EQ(howBigOfACakeCanDrakeEat(100, {1000, 8000, 2000, 5000, 3000}, 0), 12000);
}
