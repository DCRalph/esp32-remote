#include "can.h"

const static char *tag = "CAN";

#ifdef CAN_MCP

MCP_CAN CAN0(CAN0_CS);

int canInit()
{

  // Initialize MCP2515 at 500kbps
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
  {
    ESP_LOGI(tag, "MCP2515 Initialized Successfully!");
  }
  else
  {
    ESP_LOGE(tag, "Error Initializing MCP2515...");
    return -1;
  }

  pinMode(CAN0_INT, INPUT);
  return 0;
}

int canNormal()
{
  return CAN0.setMode(MCP_NORMAL);
}

int canSleep()
{
  return CAN0.setMode(MCP_SLEEP);
}

bool canInterupt()
{
  return !digitalRead(CAN0_INT);
}

int canRead(canFrame *frame)
{

  if (!digitalRead(CAN0_INT))
  {
    if (CAN0.readMsgBuf(&frame->id, &frame->extFlag, &frame->len, frame->data) == CAN_OK)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return -1;
}

int canSend(canFrame *frame)
{
  return CAN0.sendMsgBuf(frame->id, frame->extFlag, frame->len, frame->data);
}

#endif

#ifdef CAN_ESP

int canInit()
{
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX, (gpio_num_t)CAN_RX, TWAI_MODE_NORMAL);
  g_config.tx_queue_len = 10;
  g_config.rx_queue_len = 10;
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
  {
    ESP_LOGI(tag, "TWAI Driver installed");
  }
  else
  {
    ESP_LOGE(tag, "Failed to install TWAI Driver");
    return -1;
  }

  if (twai_start() == ESP_OK)
  {
    ESP_LOGI(tag, "TWAI Started");
  }
  else
  {
    ESP_LOGE(tag, "Failed to start TWAI");
    return -1;
  }

  uint32_t alerts_to_enable = TWAI_ALERT_TX_IDLE | TWAI_ALERT_TX_SUCCESS |
                              TWAI_ALERT_TX_FAILED | TWAI_ALERT_ERR_PASS |
                              TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_DATA |
                              TWAI_ALERT_RX_QUEUE_FULL;

  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
  {
    ESP_LOGI(tag, "CAN Alerts reconfigured");
  }
  else
  {
    ESP_LOGE(tag, "Failed to reconfigure alerts");
    return -1;
  }

  return 0;
}

int canNormal()
{
  return twai_start() == ESP_OK;
}

int canSleep()
{
  return twai_stop() == ESP_OK;
}

bool canInterupt()
{
  uint32_t alerts_triggered;
  twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));

  twai_status_info_t twai_status_info;
  twai_get_status_info(&twai_status_info);

  if (alerts_triggered & TWAI_ALERT_ERR_PASS)
  {
    Serial.println("Alert: TWAI controller has become error passive.");
  }
  if (alerts_triggered & TWAI_ALERT_BUS_ERROR)
  {
    Serial.println("Alert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
    Serial.printf("Bus error count: %lu\n", twai_status_info.bus_error_count);
  }
  if (alerts_triggered & TWAI_ALERT_TX_FAILED)
  {
    Serial.println("Alert: The Transmission failed.");
    Serial.printf("TX buffered: %lu\t", twai_status_info.msgs_to_tx);
    Serial.printf("TX error: %lu\t", twai_status_info.tx_error_counter);
    Serial.printf("TX failed: %lu\n", twai_status_info.tx_failed_count);
  }
  if (alerts_triggered & TWAI_ALERT_TX_SUCCESS)
  {
    Serial.println("Alert: The Transmission was successful.");
    Serial.printf("TX buffered: %lu\t", twai_status_info.msgs_to_tx);
  }

  if (alerts_triggered & TWAI_ALERT_RX_DATA)
  {
    return true;
  }
  return false;
}

int canRead(canFrame *frame)
{
  twai_message_t rx_msg;
  while (twai_receive(&rx_msg, pdMS_TO_TICKS(POLLING_RATE_MS)) == ESP_OK)
  {
    frame->id = rx_msg.identifier;
    frame->extFlag = rx_msg.extd;
    frame->len = rx_msg.data_length_code;
    memcpy(frame->data, rx_msg.data, rx_msg.data_length_code);
    return 1;
  }
  return 0;
}

int canSend(canFrame *frame)
{
  twai_message_t tx_msg;
  tx_msg.identifier = frame->id;
  tx_msg.extd = frame->extFlag;
  tx_msg.data_length_code = frame->len;
  memcpy(tx_msg.data, frame->data, frame->len);
  int res = twai_transmit(&tx_msg, pdMS_TO_TICKS(POLLING_RATE_MS));
  if (res != ESP_OK)
  {
    ESP_LOGE(tag, "Failed to send CAN frame. ERR: %d", res);
  }
  return res;
}

#endif