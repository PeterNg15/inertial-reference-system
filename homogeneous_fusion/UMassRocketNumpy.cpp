

#include <vector>
#include <iostream>
#include "UMassRocketNumpy.h"

/* Code from here

http://www.cplusplus.com/forum/beginner/235896/

*/

void print(matrix& m)
{
    for(const auto& v : m) {
        for(const int i : v) { std::cout << i << ' '; }
        std::cout << '\n';
    }
    std::cout << '\n';
}

matrix transpose( matrix& m)
{
    matrix outtrans(m.at(0).size(), std::vector<double>(m.size()));
    for (std::size_t i = 0; i < m.size(); ++i) {
        for (std::size_t j = 0; j < m.at(0).size(); ++j) {
            outtrans.at(j).at(i) = m.at(i).at(j);
        }
    }
    return outtrans;
}

matrix matmul(matrix& m1, matrix& m2)
{
    matrix result(m1.size(), std::vector<double>(m2.at(0).size()));

    for(std::size_t row = 0; row < result.size(); ++row) {
        for(std::size_t col = 0; col < result.at(0).size(); ++col) {
            for(std::size_t inner = 0; inner < m2.size(); ++inner) {
                result.at(row).at(col) += m1.at(row).at(inner) * m2.at(inner).at(col);
            }
        }
    }
    return result;
}

matrix operator+(matrix& m1, matrix& m2){
    matrix matrixAddition(m1.size(), std::vector<double>(m1.at(0).size()));
    for (std::size_t i = 0; i < m1.size(); ++i) {
        for (std::size_t j = 0; j < m1.at(0).size(); ++j) {
            matrixAddition[i][j] = m1[i][j] + m2[i][j];
        }
    }
    return matrixAddition;
}

matrix operator-(matrix& m1, matrix& m2){
    matrix matrixSubtraction(m1.size(), std::vector<double>(m1.at(0).size()));
    for (std::size_t i = 0; i < m1.size(); ++i) {
        for (std::size_t j = 0; j < m1.at(0).size(); ++j) {
            matrixSubtraction[i][j] = m1[i][j] - m2[i][j];
        }
    }
    return matrix(matrixSubtraction);
}

matrix operator*(matrix& m1, matrix& m2){
    matrix matrixSubtraction(m1.size(), std::vector<double>(m1.at(0).size()));
    for (std::size_t i = 0; i < m1.size(); ++i) {
        for (std::size_t j = 0; j < m1.at(0).size(); ++j) {
            matrixSubtraction[i][j] = m1[i][j] * m2[i][j];
        }
    }
    return matrix(matrixSubtraction);
}

matrix operator/(matrix& m1, matrix& m2){
    matrix matrixSubtraction(m1.size(), std::vector<double>(m1.at(0).size()));
    for (std::size_t i = 0; i < m1.size(); ++i) {
        for (std::size_t j = 0; j < m1.at(0).size(); ++j) {
            matrixSubtraction[i][j] = m1[i][j] / m2[i][j];
        }
    }
    return matrix(matrixSubtraction);
}

matrix numToSquareMatrix(double num, matrix& m){
    matrix square(m.size(), std::vector<double>(m.at(0).size(), num));
    return square;
}

// int main()
// {
//     //matrix src(ROWS, std::vector<int>(COLS, 2));
//     matrix src1 {{1.0, 1.0}};
//     matrix src2 {{-1.0, 1.0}, {2.0, 3.0}};
//     matrix src = numToSquareMatrix(1, src1);
//     src1 = src + src1;
//     print(src1);
//     // print(newMatrix);
//     // matrix a = matmul(newMatrix, transpose(newMatrix));
//     // print(a);
    

//     // matrix src_trans = transpose(src);
//     // print(src_trans);

//     // matrix matMul = matmul(src, src_trans);
//     // print(matMul);


//     return EXIT_SUCCESS;
// }