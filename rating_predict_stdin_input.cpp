#include <unordered_map>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

unordered_map<int, unordered_map<int, float>> rating_map;
unordered_map<int, float> user_average_ratings;

void calculateUserAverageRatings()
{
    int userId;
    int count;
    float sum;

    for (const auto& user : rating_map)
    {
        userId = user.first;
        sum = 0.0;
        count = 0;
        for (const auto& rating : user.second)
        {
            sum += rating.second;
            count++;
        }
        user_average_ratings[userId] = sum / count;
    }
}

float meanCenteredCosineSimilarity(const unordered_map<int, float>& ratings1, const unordered_map<int, float>& ratings2, float avgRating1, float avgRating2)
{
    float dotProduct = 0.0;
    float value_1 = 0.0;
    float value_2 = 0.0;

    for (const auto& rating1 : ratings1)
    {
        if (ratings2.find(rating1.first) != ratings2.end())
        {
            dotProduct += (rating1.second - avgRating1) * (ratings2.at(rating1.first) - avgRating2);
            value_1 += pow(rating1.second - avgRating1, 2);
        }
    }

    for (const auto& rating2 : ratings2)
    {
        value_2 += pow(rating2.second - avgRating2, 2);
    }

    if (value_1 == 0.0 || value_2 == 0.0)
        return 0.0;
    return dotProduct / (sqrt(value_1) * sqrt(value_2));
}

float predictRating(int userId, int movieId, int k)
{
    vector<pair<float, int>> similarities;
    int otherUserId;
    float similarity;
    float weightedSum;
    float similaritySum;
    float decayedSimilarity;
    double predictedRating;

    for (const auto& otherUser : rating_map)
    {
        otherUserId = otherUser.first;
        if (otherUserId != userId && otherUser.second.find(movieId) != otherUser.second.end())
        {
            similarity = meanCenteredCosineSimilarity(rating_map[userId], otherUser.second, user_average_ratings[userId], user_average_ratings[otherUserId]);
            similarities.push_back({similarity, otherUserId});
        }
    }
    sort(similarities.rbegin(), similarities.rend());

    weightedSum = 0.0;
    similaritySum = 0.0;
    for (int i = 0; i < min(k, (int)similarities.size()); ++i)
    {
        similarity = similarities[i].first;
        otherUserId = similarities[i].second;
        decayedSimilarity = exp(similarity);
        weightedSum += decayedSimilarity * (rating_map[otherUserId][movieId] - user_average_ratings[otherUserId]);
        similaritySum += abs(decayedSimilarity);
    }
    if (similaritySum == 0.0)
    {
        if (user_average_ratings.count(userId))
            return user_average_ratings[userId];
        else
            if (user_average_ratings.empty())
                return 0.0;
            else
                return 3.0;
    }

    predictedRating = user_average_ratings[userId] + (weightedSum / similaritySum);

    if (predictedRating <= 3.0)
        predictedRating = max(1.0, min(predictedRating, 3.0));

    return predictedRating;
}

int main (void)
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    string line;
    bool isTest = false;
    vector<pair<int, int>> testPairs;
    int userId;
    int movieId;
    float rating;
    float predicted_rating;

    while (getline(cin, line)) 
    {
        if (line == "train dataset")
        {
            isTest = false;
            continue;
        }
        else if (line == "test dataset")
        {
            isTest = true;
            continue;
        }
        stringstream ss(line);

        if (isTest)
        {
            ss >> userId >> movieId;
            testPairs.emplace_back(userId, movieId);
        } 
        else
        {
            ss >> userId >> movieId >> rating;
            rating_map[userId][movieId] = rating;
        }
    }

    calculateUserAverageRatings();

    for (const auto& pair : testPairs)
    {
        userId = pair.first;
        movieId = pair.second;
        predicted_rating = predictRating(userId, movieId, 100);
        cout << fixed << setprecision(1) << predicted_rating << endl;
    }
    return 0;
}