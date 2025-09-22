#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "spi_adxl367.h"
#include "model.h"

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"

// モデル入力サイズ
constexpr int window_size = 6;
constexpr int input_features = 3;
constexpr int input_size = window_size * input_features;
float input_buffer[window_size][input_features];
int buffer_index = 0;

// StandardScalerの平均とスケール（Colabから取得した値を使用）
float mean[3] = {0.12f, -0.03f, 0.08f};   // 仮の値
float scale[3] = {0.95f, 1.02f, 0.98f};   // 仮の値

// TensorFlow Lite Micro設定
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

constexpr int kTensorArenaSize = 1024 * 64;
uint8_t tensor_arena[kTensorArenaSize];


void tfpico_setup() {
    tflite::InitializeTarget();
    model = tflite::GetModel(model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf(
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::MicroMutableOpResolver<10> resolver;
    resolver.AddFullyConnected(tflite::Register_FULLY_CONNECTED());
    resolver.AddSoftmax(tflite::Register_SOFTMAX());
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        printf("AllocateTensors() failed");
        return;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);
  inference_count = 0;
}

void adxl367_read_device_id() {
    uint8_t devid_ad = adxl367_read_register(REG_DEVID_AD);
    uint8_t devid_mst = adxl367_read_register(REG_DEVID_MST);
    uint8_t partid    = adxl367_read_register(REG_PARTID);
    uint8_t revid     = adxl367_read_register(REG_REVID);

    printf("DEVID_AD:  0x%02X\n", devid_ad);
    printf("DEVID_MST: 0x%02X\n", devid_mst);
    printf("PARTID:    0x%02X\n", partid);
    printf("REVID:     0x%02X\n", revid);
    printf("----------------------\n");
}

float scale_value(int16_t raw, int axis) {
    return ((raw / 1000.0f) - mean[axis]) / scale[axis];
}

int main() {
    stdio_init_all();
    adxl367_init();

    sleep_ms(8000);  // シリアルモニターの準備時間
    adxl367_read_device_id();


    /*以下より要検証*/
    tfpico_setup();
    
    while (true) {
        int16_t x_raw, y_raw, z_raw;
        adxl367_read_xyz(&x_raw, &y_raw, &z_raw);

        input_buffer[buffer_index][0] = scale_value(x_raw, 0);
        input_buffer[buffer_index][1] = scale_value(y_raw, 1);
        input_buffer[buffer_index][2] = scale_value(z_raw, 2);
        buffer_index++;

        if (buffer_index >= window_size) {
            buffer_index = 0;

            // モデル入力テンソルにコピー（フラット化）
            for (int i = 0; i < window_size; ++i) {
                for (int j = 0; j < input_features; ++j) {
                    input->data.f[i * input_features + j] = input_buffer[i][j];
                }
            }

            //モデルに入力テンソルを渡して、出力テンソルに結果を返す処理
            if (interpreter->Invoke() != kTfLiteOk) {
                printf("Invoke failed");
                continue;
            }

            // 出力結果（3クラスの確率）
            for (int i = 0; i < 3; ++i) {
                printf("Class %d: %.3f\n", i, output->data.f[i]);
            }

            // 最も高い確率のクラスを表示
            int max_index = 0;
            float max_value = output->data.f[0];
            for (int i = 1; i < 3; ++i) {
                if (output->data.f[i] > max_value) {
                    max_value = output->data.f[i];
                    max_index = i;
                }
            }
            printf("Predicted class: %d\n\n", max_index);
        }

        sleep_ms(80);  // ODR 12.5Hz に合わせて約80ms間隔
    }



    // while (true) {
    //     int16_t x, y, z;
    //     adxl367_read_xyz(&x, &y, &z);
    //     printf("X: %d, Y: %d, Z: %d\n", x, y, z);
    //     sleep_ms(500);
    // }
}
