#include <iostream>
#include <sstream>
#include <map>
#include <cmath>
#include <iomanip>
#include <queue>

using namespace std;

class SimilarityCalculator
{
    public:
    double meanCenteredCosineSimilarity(const map<int, double>& ratings_a, const map<int, double>& ratings_b, double avg_rating_a, double avg_rating_b) const
    {
        double dotProduct = 0.0;
        double value_a = 0.0;
        double value_b = 0.0;

        for (const auto& pair_a : ratings_a)
        {
            if (ratings_b.count(pair_a.first)) 
                dotProduct += (pair_a.second - avg_rating_a) * (ratings_b.at(pair_a.first) - avg_rating_b);
            value_a += pow(pair_a.second - avg_rating_b, 2);
        }

        for (const auto& pair_b : ratings_b)
        {
            value_b += pow(pair_b.second - avg_rating_b, 2);
        }

        if (value_a == 0.0 || value_b == 0.0)
            return 0.0;

        return dotProduct / (sqrt(value_a) * sqrt(value_b));
    }
};

class UBCFRecommender
{
    public:
        void fit(const map<int, map<int, double>>& ratings);
        double predictRating(int userId, int movieId, int k = 5) const;

    private:
        map<int, map<int, double>> userRatings;
        map<int, double> userAverageRatings;
        SimilarityCalculator similarityCalculator;

        double calculateSimilarity(int userAId, int userBId) const;
        double exponentialDecay(double similarity) const { return exp(1.0 * similarity); }
        void calculateUserAverageRatings();
};

void UBCFRecommender::fit(const map<int, map<int, double>>& ratings)
{
    userRatings = ratings;
    calculateUserAverageRatings();
}

void UBCFRecommender::calculateUserAverageRatings()
{
    double sum;

    userAverageRatings.clear();
    for (const auto& userPair : userRatings)
    {
        if (!userPair.second.empty())
        {
            sum = 0.0;
            for (const auto& ratingPair : userPair.second)
            {
                sum += ratingPair.second;
            }
            userAverageRatings[userPair.first] = sum / userPair.second.size();
        } 
        else
            userAverageRatings[userPair.first] = 0.0;
    }
}

double UBCFRecommender::calculateSimilarity(int user_a, int user_b) const 
{
    return similarityCalculator.meanCenteredCosineSimilarity(userRatings.at(user_a), userRatings.at(user_b), userAverageRatings.at(user_a), userAverageRatings.at(user_b));
}

double UBCFRecommender::predictRating(int userId, int movieId, int k) const 
{
    priority_queue<pair<double, int>> similarUsersQueue;
    int otherUserId;
    double similarity;
    double weightedSum;
    double similaritySum;
    double decayedSimilarity;
    double predictedRating;

    if (userRatings.find(userId) == userRatings.end())
    {
        if (userAverageRatings.empty())
            return 0.0;
        else
            return 3.0;
    }

    if (userRatings.at(userId).count(movieId))
        return userRatings.at(userId).at(movieId);

    for (const auto& otherUserPair : userRatings) {
        otherUserId = otherUserPair.first;
        if (otherUserId != userId && userRatings.at(otherUserId).count(movieId)) {
            similarity = calculateSimilarity(userId, otherUserId);
            if (similarUsersQueue.size() < static_cast<priority_queue<pair<double, int>>::size_type>(k) || similarity > similarUsersQueue.top().first) {
                similarUsersQueue.push({similarity, otherUserId});
                if (similarUsersQueue.size() > static_cast<priority_queue<pair<double, int>>::size_type>(k)) {
                    similarUsersQueue.pop();
                }
            }
        }
    }

    weightedSum = 0.0;
    similaritySum = 0.0;

    while (!similarUsersQueue.empty()) {
        similarity = similarUsersQueue.top().first;
        decayedSimilarity = exponentialDecay(similarity);

        weightedSum += decayedSimilarity * (userRatings.at(similarUsersQueue.top().second).at(movieId) - userAverageRatings.at(similarUsersQueue.top().second));
        similaritySum += abs(decayedSimilarity);
        similarUsersQueue.pop();
    }

    if (similaritySum == 0.0) {
        return userAverageRatings.count(userId) ? userAverageRatings.at(userId) : (userAverageRatings.empty() ? 0.0 : 3.0);
    }

    predictedRating = userAverageRatings.at(userId) + (weightedSum / similaritySum);

    if (predictedRating <= 3.0) {
        predictedRating = max(1.0, min(predictedRating, 3.0));
    }

    return predictedRating;
}

int main() {
    map<int, map<int, double>> training_rating_map;
    vector<pair<int, int>> testing_rating_map;
    string line;
    UBCFRecommender recommender;
    bool readingTestData = false;
    int userId;
    int movieId;
    double rating;
    double predictedRating;

    while (getline(cin, line))
    {
        if (line == "train dataset")
            continue;
        else if (line == "test dataset")
        {
            readingTestData = true;
            if (!training_rating_map.empty()) {
                recommender.fit(training_rating_map);
            }
            continue;
        }
        else
        {
            stringstream ss(line);
            if (!readingTestData && ss >> userId >> movieId >> rating)
                training_rating_map[userId][movieId] = rating;

            else if (readingTestData && ss >> userId >> movieId)
                testing_rating_map.push_back({userId, movieId});
        }
    }

    cout << fixed << setprecision(1);
    for (const auto& userMoviePair : testing_rating_map)
    {
        userId = userMoviePair.first;
        movieId = userMoviePair.second;
        predictedRating = recommender.predictRating(userId, movieId, 100);
        cout << predictedRating << endl;
    }
    return 0;
}