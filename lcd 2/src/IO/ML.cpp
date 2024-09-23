#include "ML.h"

static const char *TAG = "ML";

ML::ML()
{
}

void ML::init()
{
  ESP_LOGI(TAG, "init");

  model = tflite::GetModel(model_colour_channels4_tflite);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
    ESP_LOGI(TAG, "Model provided is schema version %d not equal to supported version %d", model->version(), TFLITE_SCHEMA_VERSION);
    while(true); // stop program here
  }
}