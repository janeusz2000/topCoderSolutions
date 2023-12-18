#include "gmock/gmock.h"
#include "gtest/gtest.h"

template <typename K, typename V> void printMap(const std::map<K, V> &map) {
  std::cout << "[";
  for (const auto &elem : map) {
    std::cout << "(" << elem.first << ", " << elem.second << "), ";
  }
  std::cout << std::endl;
}

template <typename K, typename V>
class IntervalMap { // TODO: rename this to camel case at the end
public:
  // constructor associates whole range of K with val
  IntervalMap(const V &val) : m_valBegin(val) {}

  // Assign value val to interval [keyBegin, keyEnd).
  // Overwrite previous values in this interval.
  // Conforming to the C++ Standard Library conventions, the interval
  // includes keyBegin, but excludes keyEnd.
  // If !( keyBegin < keyEnd ), this designates an empty interval,
  // and assign must do nothing.
  void assign(const K &keyBegin, const K &keyEnd, const V &val) {
    // There is a case where I dont know how to solve yet:
    // TEST(SomeTest, InsertedRangeClearsMap) {
    //  IntervalMap<int, char> interval('A');
    //  interval.assign(3, 5, 'B');
    //  interval.assign(5, 8, 'C');
    //  interval.assign(8, 10, 'D');

    //  interval.assign(3, 10, 'A');
    //  std::map<int, char> expectedMap = {};
    //  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
    //}
    //
    // This should clears up whole range but because I cannot to == operation in keys of the map
    // I don't have an idea how to solve this case (I don't have much time left)

    if (!(keyBegin < keyEnd)) { // empty given interval 
      return;
    }

    if (m_map.empty()) {
      if (val != m_valBegin) {
        m_map.insert(m_map.end(), std::make_pair(keyBegin, val));
        m_map.insert(m_map.end(), std::make_pair(keyEnd, m_valBegin));
      }
      return;
    }

    std::map<K, V> temporary_map;
    bool valueInserted = false;
    auto prev = m_map.begin();
    for (auto it = m_map.begin(); it != m_map.end(); it++) {
      if (!valueInserted && keyBegin < it->first) {

        // We need to know how much new range overlaps with existsing ones:
        auto temp = m_map.begin();
        while (it->first < keyEnd && it != std::prev(m_map.end())) {
          temp = it; // This is the range that is probable to be overlapped
          it++;
        }

        if (it == m_map.begin()) {
          // We start from the beggining where given 
          // value is different from one +/- Inf
          if (val != m_valBegin && keyEnd < it->first) {
            temporary_map.insert(temporary_map.end(), std::make_pair(keyBegin, val));
            temporary_map.insert(temporary_map.end(), std::make_pair(keyEnd, m_valBegin));
          }

          // We proceed with writing already existing ranges
          temporary_map.insert(*it);
        } else {

          // We proceed with values other then in the beggining.
          // We need to check if previous existing values are different from
          // previous existing range. We dont want to insert range to already 
          // exisiting range with the same value
          if (val != prev->second) {
            temporary_map.insert(temporary_map.end(), std::make_pair(keyBegin, val));
            if (keyEnd < it->first) {
            // We are in sa subrange of temp
              temporary_map.insert(temporary_map.end(), std::make_pair(keyEnd, temp->second));
            }
          }

          // We proceed with writing already existing ranges
          temporary_map.insert(temporary_map.end(), *it);
        }
        valueInserted = true;
      } else if (!valueInserted && it == std::prev(m_map.end())) {
        
        // We proceed with the second case of insertion where we insert at the end
        // of the sorted map
        if (val == m_valBegin) {
          // last element have always default value and keyBegin is greater or equal then
          // the one from the it, so we need to insert range from it
          temporary_map.insert(temporary_map.end(), *it);
          continue;
        } else {
          // last element range is lower then the one we want to insert
          if (it->first < keyBegin) {
            temporary_map.insert(temporary_map.end(), *it);
          }
          temporary_map.insert(temporary_map.end(), std::make_pair(keyBegin, val));
          temporary_map.insert(temporary_map.end(), std::make_pair(keyEnd, m_valBegin));
          valueInserted = true;
        }
      } else {
        temporary_map.insert(temporary_map.end(), *it);

      }
      prev = it;
    }
    m_map = std::move(temporary_map);

    // clean up
    bool endCleared = false;
    for (auto rit = m_map.rbegin(); rit != std::prev(m_map.rend()); rit++) {
      auto rNextIt = std::next(rit);
      if ((!endCleared && rNextIt->second == m_valBegin) || rit->second == rNextIt->second) {
        m_map.erase(rit->first);
      } else {
        endCleared = true;
      } 
    }
  }
  // TODO: make this private at the end
  /* friend void IntervalMapTest(); */
  V m_valBegin;
  std::map<K, V> m_map;
};

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os,
                         const IntervalMap<K, V> &intervalMap) {
  std::stringstream ss;
  ss << "[ ";
  for (const auto &[key, value] : intervalMap.m_map) {
    ss << '(' << key << ", " << value << "), ";
  }
  ss << "]";

  os << ss.str();
  return os;
}
TEST(NormalCase, normalCase) {
  IntervalMap<int, char> interval('A');
  interval.assign(1, 1, 'B');
  interval.assign(2, 2, 'A');
  interval.assign(3, 3, 'B');
  interval.assign(4, 4, 'A');

  interval.assign(3, 3, 'C');

  std::map<int, char> expectedMap = {{1, 'B'}, {2, 'A'}, {3, 'C'}, {4, 'A'}};
}

TEST(SomeTest, empty) {
  IntervalMap<int, char> interval('A');
  std::map<int, char> expectedMap = {};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, emptyInitialization) {
  IntervalMap<int, char> interval('A');
  interval.assign(8, 9, 'B');
  std::map<int, char> expectedMap = {{8, 'B'}, {9, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, invalidGivenRange) {
  IntervalMap<int, char> interval('A');
  interval.assign(9, 8, 'B');
  std::map<int, char> expectedMap = {};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, WriteNewIntervalValue) {
  IntervalMap<int, char> interval('A');
  interval.assign(8, 9, 'A');
  std::map<int, char> expectedMap = {};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRageAtTheBegging) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 4, 'B');
  interval.assign(1, 2, 'C');
  std::map<int, char> expectedMap = {{1, 'C'}, {2, 'A'}, {3, 'B'}, {4, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, RangeIsSplitedMultipleTimes) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 10, 'B');
  interval.assign(15, 30, 'C');
  interval.assign(35, 40, 'D');
  std::map<int, char> expectedMap = {{3, 'B'}, {10, 'A'}, {15, 'C'}, {30, 'A'}, {35, 'D'}, {40, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));

  interval.assign(5, 7, 'G');
  std::map<int, char> expectedMap2 = {{3, 'B'}, {5, 'G'}, {7, 'B'}, {10, 'A'}, {15, 'C'}, {30, 'A'}, {35, 'D'}, {40, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap2));
}

TEST(SomeTest, InsertedRangeAtTheBegginingTheSameValue) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(1, 2, 'A');
  std::map<int, char> expectedMap = {{3, 'B'}, {5, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeInTheMiddleTheSameValue) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(4, 5, 'B');
  std::map<int, char> expectedMap = {{3, 'B'}, {5, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeInTheMiddleDifferentValue) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(4, 5, 'C');
  std::map<int, char> expectedMap = {{3, 'B'}, {4, 'C'}, {5, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeInTheMiddleTheSameAsEnd) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(4, 5, 'A');
  std::map<int, char> expectedMap = {{3, 'B'}, {4, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeValueAtTheEnd) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(6, 8, 'C');
  std::map<int, char> expectedMap = {{3, 'B'}, {5, 'A'}, {6, 'C'}, {8, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeValueAtTheEndTheSameAsEnd) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(5, 8, 'A');
  std::map<int, char> expectedMap = {{3, 'B'}, {5, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeValeToTheEndTheSameAsPreviousOne) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(5, 8, 'B');
  std::map<int, char> expectedMap = {{3, 'B'}, {8, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

TEST(SomeTest, InsertedRangeUpdatedMayRanges) {
  IntervalMap<int, char> interval('A');
  interval.assign(3, 5, 'B');
  interval.assign(5, 8, 'C');
  interval.assign(8, 10, 'D');

  interval.assign(4, 9, 'E');
  std::map<int, char> expectedMap = {{3, 'B'}, {4, 'E'}, {9, 'D'}, {10, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}

/* TEST(SomeTest, InsertedRangeClearsMap) { */
/*   IntervalMap<int, char> interval('A'); */
/*   interval.assign(3, 5, 'B'); */
/*   interval.assign(5, 8, 'C'); */
/*   interval.assign(8, 10, 'D'); */

/*   interval.assign(3, 10, 'A'); */
/*   std::map<int, char> expectedMap = {}; */
/*   ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap)); */
/* } */

TEST(SomeTest, SomeTest) {
  IntervalMap<int, char> interval('A');
  interval.assign(1, 7, 'B');
  interval.assign(2, 4, 'A');
  std::map<int, char> expectedMap = {{1, 'B'}, {2, 'A'}, {4, 'B'}, {7, 'A'}};
  ASSERT_THAT(interval.m_map, ::testing::ElementsAreArray(expectedMap));
}
