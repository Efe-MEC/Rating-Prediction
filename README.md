Rating Prediction Project

Overview

This project is designed for rating prediction using two distinct methods:

Class-Based Implementation

Function-Map-Based Implementation

Both implementations compute similarities using the Mean-Centered Cosine Similarity method. While the RMSE (Root Mean Square Error) scores are identical for both approaches, their performance in terms of running time differs significantly:

The function-based implementation is much faster than the class-based implementation.

More detailed information is given in the Rating Predicition Project Subject.pdf.

Features

Similarity Calculation: Both methods use the Mean-Centered Cosine Similarity technique to compute similarity.

Flexible Data Input: The project supports reading data from both the terminal (stdin) and files.

Example Data: Includes example datasets for training and testing purposes.

Usage

Input Data

You can provide input data via:

Terminal (stdin): Enter data directly through the terminal.

File: Provide a file containing the data.

Example Datasets

The project includes the following datasets:

Example Data: Sample data for demonstration purposes.

Training Data: Used to train the model.

Testing Data: Used to evaluate the model.

Performance

RMSE Scores: The Root Mean Square Error is consistent across both implementations.

Execution Time: The function-based implementation significantly outperforms the class-based implementation in terms of speed.

Getting Started

Clone the repository.

Follow the instructions to set up the environment.

Run the desired implementation (class-based or function-based) with the provided datasets or your own data.

References

For additional information about the Mean-Centered Cosine Similarity method and RMSE evaluation, consult the included documentation or external references.

