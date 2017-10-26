#include "mxpredict.h"
#include <QObject>
#include <QImage>
#include <QString>
#include <Qt>
#include <fstream>
#include <iostream>
#include "bufferfile.h"

std::string json_file = "cnn/resnet-imagenet-101-0-symbol.json";
std::string param_file = "cnn/resnet-imagenet-101-0-0123.params";
std::string synset_file = "cnn/synset.txt";
BufferFile MXPredict::json_data = BufferFile(json_file);
BufferFile MXPredict::param_data = BufferFile(param_file);


MXPredict::MXPredict(QObject *parent) : QObject(parent)
{
        std::cerr << "KONSTRUKTOR" << std::endl;

}
void MXPredict::PrintOutputResult(const std::vector<float>& data, const std::vector<std::string>& synset) {
    float best_accuracy = 0.0;
    int best_idx = 0;

    for ( int i = 0; i < static_cast<int>(data.size()); i++ ) {
        if ( data[i] > best_accuracy ) {
            best_accuracy = data[i];
            best_idx = i;
        }
    }

    printf("Best Result: [%s] id = %d, accuracy = %.8f\n",
           synset[best_idx].c_str(), best_idx, best_accuracy);
}
void MXPredict::GetImageFile(QString image_file, mx_float* image_data, const int channels, const int width) {
    QImage im(image_file);
    im = im.scaled(width, width, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int size = width * width * channels;

    mx_float* ptr_image_r = image_data;
    mx_float* ptr_image_g = image_data + size / 3;
    mx_float* ptr_image_b = image_data + size / 3 * 2;

    for (int i = 0; i < im.height(); i++) {
        for (int j = 0; j < im.width(); j++) {
            QRgb bits = im.pixel(i, j);
            *ptr_image_r++ = static_cast<mx_float>(qRed(bits)) ;
            *ptr_image_g++ = static_cast<mx_float>(qGreen(bits));
            *ptr_image_b++ = static_cast<mx_float>(qBlue(bits)) ;
        }
    }
}
std::vector<std::string> MXPredict::LoadSynset(std::string synset_file) {
    std::ifstream fi(synset_file.c_str());

    if ( !fi.is_open() ) {
        std::cerr << "Error opening synset file " << synset_file << std::endl;
    }

    std::vector<std::string> output;

    std::string synset, lemma;
    while ( fi >> synset ) {
        getline(fi, lemma);
        output.push_back(lemma);
    }

    fi.close();

    return output;
}
int MXPredict::predict(QString path) {
    std::string json_file = "cnn/resnet-imagenet-101-0-symbol.json";
    std::string param_file = "cnn/resnet-imagenet-101-0-0123.params";
    std::string synset_file = "cnn/synset.txt";

    BufferFile json_data(json_file);
    BufferFile param_data(param_file);

    mx_uint num_input_nodes = 1;  // 1 for feedforward
    const char* input_key[1] = {"data"};
    const char** input_keys = input_key;

    // Image size and channels
    int width = 224;
    int channels = 3;

    const mx_uint input_shape_indptr[2] = { 0, 4 };
    const mx_uint input_shape_data[4] = { 1,
                                          static_cast<mx_uint>(channels),
                                          static_cast<mx_uint>(width),
                                          static_cast<mx_uint>(width)};
    PredictorHandle pred_hnd = 0;

    if (json_data.GetLength() == 0 ||
                    param_data.GetLength() == 0) {
        return -1;
    }

    // Create Predictor
    MXPredCreate((const char*)MXPredict::json_data.GetBuffer(),
                 (const char*)MXPredict::param_data.GetBuffer(),
                 static_cast<size_t>(param_data.GetLength()),
                 1,
                 0,
                 num_input_nodes,
                 input_keys,
                 input_shape_indptr,
                 input_shape_data,
                 &pred_hnd);
    int image_size = width * width * channels;
    std::vector<mx_float> image_data = std::vector<mx_float>(image_size);
    GetImageFile(path, image_data.data(), channels, width);
    NDListHandle nd_hnd = 0;
    MXPredSetInput(pred_hnd, "data", image_data.data(), image_size);

    // Do Predict Forward
    MXPredForward(pred_hnd);

    mx_uint output_index = 0;

    mx_uint *shape = 0;
    mx_uint shape_len;

    // Get Output Result
    MXPredGetOutputShape(pred_hnd, output_index, &shape, &shape_len);

    size_t size = 1;
    for (mx_uint i = 0; i < shape_len; ++i) size *= shape[i];

    std::vector<float> data(size);

    MXPredGetOutput(pred_hnd, output_index, &(data[0]), size);

    // Release NDList
    if (nd_hnd)
        MXNDListFree(nd_hnd);

    // Release Predictor
    MXPredFree(pred_hnd);

    // Synset path for your model, you have to modify it
    std::vector<std::string> synset = LoadSynset(synset_file);

    // Print Output Data
    PrintOutputResult(data, synset);
    return 1;
}



