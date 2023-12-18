#include "gtest/gtest.h"
#include <iostream>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

struct Position
{
    int x;
    int y;
};

std::unordered_map<char, std::vector<Position>> constructGraph(
    const std::vector<std::vector<char>>& board)
{
    std::unordered_map<char, std::vector<Position>> graph;
    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board[0].size(); y++)
        {
            graph[board[x][y]].push_back(Position{x, y});
        }
    }
    return graph;
}

class TrackingData
{
public:
    explicit TrackingData(const Position& position, unsigned int boardSize)
        : m_lastPosition(position), currentPostion(0)
    {
        m_visited.resize(boardSize, std::vector<bool>(boardSize));
    }

    TrackingData makeCopyWithNewPosition(const Position& position)
    {
        TrackingData newData = *this;
        newData.m_lastPosition = position;
        newData.visitPosition(position);
        return newData;
    }

    void visitPosition(const Position& position)
    {
        m_visited[position.x][position.y] = true;
    }

    bool isVisited(const Position& position) const
    {
        return m_visited[position.x][position.y];
    }

    bool travelIsValid(const Position& position) const
    {
        int difference_x = std::abs(m_lastPosition.x - position.x);
        int difference_y = std::abs(m_lastPosition.y - position.y);
        return difference_x <= 1 && difference_y <= 1;
    }

    Position m_lastPosition;
    int currentPostion;

private:
    std::vector<std::vector<bool>> m_visited;
};

std::optional<std::queue<TrackingData>> initializeBfsQueue(
    const std::unordered_map<char, std::vector<Position>>& graph,
    const char firstSymbol,
    int boardSize)
{
    std::queue<TrackingData> bfsQueue;
    if (graph.find(firstSymbol) != graph.end())
    {
        for (const Position& position : graph.at(firstSymbol))
        {
            TrackingData data(position, boardSize);
            data.visitPosition(position);
            bfsQueue.push(std::move(data));
        }
        return bfsQueue;
    }
    return std::nullopt;
}

bool check_word(const std::vector<std::vector<char>>& board,
                const std::string& word)
{
    std::unordered_map<char, std::vector<Position>> graph =
        constructGraph(board);
    auto bfsQueue = initializeBfsQueue(graph, word[0], board.size());
    if (bfsQueue)
    {
        while (!bfsQueue->empty())
        {
            TrackingData currentData = bfsQueue->front();
            bfsQueue->pop();

            if (word.length() == ++currentData.currentPostion)
            {
                return true;
            }

            char nextCharacter = word[currentData.currentPostion];
            const std::vector<Position>& availableDestinations =
                graph[nextCharacter];
            for (const Position& pos : availableDestinations)
            {
                if (!currentData.isVisited(pos) &&
                    currentData.travelIsValid(pos))
                {
                    bfsQueue->push(currentData.makeCopyWithNewPosition(pos));
                }
            }
        }
    }

    return false;
}

// ============== TEST ==============
// TODO: Replace examples and use TDD development by writing your own tests

TEST(GIVEN_TESTS, BASIC)
{
    const std::vector<std::vector<char>> test_board = {{'E', 'A', 'R', 'A'},
                                                       {'N', 'L', 'E', 'C'},
                                                       {'I', 'A', 'I', 'S'},
                                                       {'B', 'Y', 'O', 'R'}};

    ASSERT_TRUE(check_word(test_board, "C"));
    ASSERT_TRUE(check_word(test_board, "EAR"));
    ASSERT_FALSE(check_word(test_board, "EARS"));
    ASSERT_TRUE(check_word(test_board, "BAILER"));

    ASSERT_TRUE(check_word(test_board, "RSCAREIOYBAILNEA"))
        << "Must be able to check indefinite word lengths going in all "
           "directions";

    ASSERT_FALSE(check_word(test_board, "CEREAL"))
        << "Valid guesses can't overlap themselves";

    ASSERT_FALSE(check_word(test_board, "ROBES"))
        << "Valid guesses have to be adjacent";

    ASSERT_FALSE(check_word(test_board, "BAKER"))
        << "All the letters have to be in the board";

    ASSERT_FALSE(check_word(test_board, "CARS"))
        << "Valid guesses cannot wrap around the edges of the board";
}
