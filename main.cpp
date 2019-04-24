//
//  main.cpp
//  cnn


#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//
//--------------------------------------------------------------------------------------//
//print vector
void print_vec2(const vector<vector<int>>& c){
    for (int i = 0, iEnd = c.size(); i < iEnd; i++){
        for (int j = 0, jEnd = c[i].size(); j < jEnd; j++){
                std::cout<<c[i][j]<<",";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

void print_vec3(const vector<vector<vector<int>>>& c){
    for (int i = 0, iEnd = c.size(); i < iEnd; i++){
        for (int j = 0, jEnd = c[i].size(); j < jEnd; j++){
            for(int k = 0,kEnd = c[i][j].size(); k < kEnd; k++){
                std::cout<<c[i][j][k]<<",";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}
//--------------------------------------------------------------------------------------//
//卷积层
//zero_padding 默认为0
//stride 默认为1
int calculate_output_size(int input_size, int filter_size, int zero_padding, int stride){
    return (input_size - filter_size +
            2 * zero_padding) / stride + 1;
}

//TODO:
void padding(vector<vector<int>>& input, int zp){
    if (zp == 0){
        return ;
    }
    //TODO: 二维和三维矩阵加0
    return ;
}


vector<vector<int>> conv2(const vector<vector<int>>& input,
                          const vector<vector<int>>& weight,
                          int zero_padding, int stride){
    if (input.empty() || weight.empty()) {
        return vector<vector<int>> {};
    }
    int iHeight = input.size(), iWidth = input[0].size();
    int wHeight = weight.size(), wWidth = weight[0].size();
    
    int oHeight = calculate_output_size(iHeight, wHeight,zero_padding, stride);
    int oWidth = calculate_output_size(iWidth, wWidth, zero_padding,stride);
    vector<vector<int>> res(oHeight, vector<int>(oWidth));
    for (int i = 0; i < oHeight; i++){
        for (int j = 0; j < oWidth; j++){
            int sum = 0;
            for(int m = 0; m < wHeight; m++){
                for(int n = 0; n <wWidth; n++){
                    if((i+m) >= 0 && (i+m) < iHeight && (j+n) >= 0 && (j+n) < iWidth){
                        sum += weight[m][n] * input[i+m][j+n];
                    }
                }
            }
            res[i][j] = sum;
        }
    }
    
    return res;
}

//--------------------------------------------------------------------------------------//
//ReLU
vector<vector<int>> relu(const vector<vector<int>>& input){
    int input_height = input.size(), input_width = input[0].size();
    
    vector<vector<int>> res(input_height, vector<int>(input_width));
    for (int i = 0; i < input_height; i++){
        for(int j = 0; j < input_width; j++){
            res[i][j] = std::max(0, input[i][j]);
        }
    }
    
    return res;
}

//--------------------------------------------------------------------------------------//
//Pooling
//池化区域的大小:2 （TODO）
//池化的标准: max(TODO:average)
vector<vector<int>> max_pool_2x2(const vector<vector<int>>& input){
    if (input.empty()) {
        return vector<vector<int>> {};
    }
    const int stride = 2;
    int input_height = input.size(), input_width = input.size();
    int out_height = input_height / stride;
    int out_width = input_width / stride;
    
    vector<vector<int>> res(out_height, vector<int>(out_width));
    
    for (int i = 0; i < out_height; i++){
        for (int j = 0; j < out_width; j++){
            int tmp = -1;
            for ( int m = i * stride; m < (i * stride + stride); m++){
                for ( int n = j * stride; n < (j * stride + stride); n++){
                    tmp = std::max(tmp,input[m][n]);
                }
            }
            res[i][j] = tmp;
        }
    }
    return res;
}

//--------------------------------------------------------------------------------------//
//前向传播算法
//先考虑最简单的，样本都是二维的黑白图片。这样输入层X就是一个矩阵，矩阵的值等于图片的各个像素位置的值。这时和卷积层相连的卷积核W就也是矩阵。
//TODO:如果样本都是有RGB的彩色图片，这样输入X就是3个矩阵，即分别对应R，G和B的矩阵，或者说是一个张量。这时和卷积层相连的卷积核W就也是张量，对应的最后一维的维度为3.即每个卷积核都是3个子矩阵组成。

//定义的CNN模型参数
//1）卷积核的大小为k（不止一个），则输入层的输出（或者说第二层卷积层的对应的输入）有k个
//2) 卷积核中每个子矩阵的的大小，一般我们都用子矩阵为方阵的卷积核，比如FxF的子矩阵。
//3) TODO (本例中默认为0): 填充padding,在卷积的时候，为了可以更好的识别边缘，一般都会在输入矩阵在周围加上若干圈的0再进行卷积，加多少圈则P为多少
//4) TODO (本例中默认为1): 步幅stride,在卷积过程中每次移动的像素距离大小。

vector<vector<vector<int>>> forward(vector<vector<int>> input,
                                    vector<vector<vector<int>>> weights,
                                    int zero_padding = 0, int stride = 1
                                    ){
    vector<vector<vector<int>>> res;
    for (int i = 0; i < weights.size(); i++){
        auto conv_output = conv2(input, weights[i], zero_padding, stride);
        std::cout<<"after conv2......"<<std::endl;
        print_vec2(conv_output);
        
        
        auto relu_output = relu(conv_output);
        std::cout<<"after relu......."<<std::endl;
        print_vec2(relu_output);
        
        auto pooling_output = max_pool_2x2(relu_output);
        res.emplace_back(pooling_output);
    }
    return res;
}

//--------------------------------------------------------------------------------------//

int main(int argc, const char * argv[]) {
   vector<vector<int>> input = {
        {0,1,1,0,2},
        {2,2,2,2,1},
        {1,0,0,2,0},
        {0,1,1,0,0},
        {1,2,0,0,2}
   };
    
    vector<vector<vector<int>>> weights = {
        {
            {1,1},
            {-1,-1}
        },
        {
            {0,1},
            {-1,0}
        }
        };
    
    
    auto c = forward(input, weights, 0, 1);
    std::cout<<"last result......"<<std::endl;
    print_vec3(c);
    return 0;
}
