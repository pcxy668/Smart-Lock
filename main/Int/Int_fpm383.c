#include "Int_fpm383.h"

// 初始化FPM383相关GPIO引脚 
void Int_FPM383_Init(void)
{
    uart_config_t uart_conf = {};
    uart_conf.baud_rate = 57600;
    uart_conf.data_bits = UART_DATA_8_BITS;
    uart_conf.stop_bits = UART_STOP_BITS_1;
    uart_conf.parity = UART_PARITY_DISABLE;
    uart_conf.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_conf.source_clk = UART_SCLK_DEFAULT;

    uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_conf);
    uart_set_pin(UART_NUM_1, GPIO_NUM_19, GPIO_NUM_18, UART_PIN_NO_CHANGE,
                UART_PIN_NO_CHANGE);

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << GPIO_NUM_10;
    gpio_config(&io_conf);

    DelayMs(300);

    Int_FPM383_GetSerialNumber();
    Int_FPM383_GetTemplatesNumber();
    Int_FPM383_Sleep();
}


// 获取FPM383的序列号
void Int_FPM383_GetSerialNumber(void)
{
    uint8_t cmd[13] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
        0x01,                   // 包标识
        0x00, 0x04,             // 包长度
        0x34,                   // 指令码
        0x00,                   // 参数
        0x00, 0x39              // 校验和
    };

    uart_write_bytes(UART_NUM_1, cmd, 13);

    uint8_t recv_data[64] = {0};

    int len =
        uart_read_bytes(UART_NUM_1, recv_data, 1024, 100 / portTICK_PERIOD_MS);

    if (len > 0) {
        if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
            printf("芯片唯一序列号：%.32s\n", &recv_data[10]);
        }
    }
}

// FPM383进入休眠模式
uint8_t Int_FPM383_Sleep(void)
{
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
        0x01,                   // 包标识
        0x00, 0x03,             // 包长度
        0x33,                   // 指令码
        0x00, 0x37              // 校验和
    };

    uart_write_bytes(UART_NUM_1, cmd, 12);

    uint8_t recv_data[64] = {0};

    int len =
        uart_read_bytes(UART_NUM_1, recv_data, 1024, 100 / portTICK_PERIOD_MS);

    if (len > 0) {
        if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
        printf("指纹模块休眠成功\n");
        return 0;
        }
    }
    printf("指纹模块休眠失败\n");
    return 1;
}

// 获取FPM383中指纹模板数量
uint8_t Int_FPM383_GetTemplatesNumber(void)
{
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
        0x01,                   // 包标识
        0x00, 0x03,             // 包长度
        0x1D,                   // 指令码
        0x00, 0x21              // 校验和
    };

    uart_write_bytes(UART_NUM_1, cmd, 12);

    uint8_t recv_data[64] = {0};

    int len =
        uart_read_bytes(UART_NUM_1, recv_data, 1024, 100 / portTICK_PERIOD_MS);

    if (len > 0) {
        if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
        printf("指纹模块中保存的指纹数量：%d\n", recv_data[11]); //最大存储50个指纹，只取低字节
        return recv_data[11];
        }
    }

    printf("获取指纹数量失败\n");
    return 0xFF;
}

// 获取指纹图像
uint8_t Int_FPM383_GetImage(void)
{
    uint8_t cmd[12] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
        0x01,                   // 包标识
        0x00, 0x03,             // 包长度
        0x01,                   // 指令码
        0x00, 0x05              // 校验和
    };

    uart_write_bytes(UART_NUM_1, cmd, 12);

    uint8_t recv_data[64] = {0};

    int len =
        uart_read_bytes(UART_NUM_1, recv_data, 1024, 200 / portTICK_PERIOD_MS);

    if (len > 0) {
        if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
        printf("获取指纹图像成功\n");
        return 0;
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x01) {
        printf("收包错误\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x02) {
        printf("传感器上无手指\n");
        }
    }

    printf("获取指纹图像失败\n");
    return 1;
}

// 生成指纹特征
uint8_t Int_FPM383_GenChar(uint8_t bufferID)
{
    uint8_t cmd[13] = {
        0xEF, 0x01,             // 包头
        0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
        0x01,                   // 包标识
        0x00, 0x04,             // 包长度
        0x02,                   // 指令码
        0,                      // BufferID
        0,    0                 // 校验和
    };

    cmd[10] = bufferID;
    uint16_t sum = cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10];
    cmd[11] = sum >> 8;
    cmd[12] = sum;

    uart_write_bytes(UART_NUM_1, cmd, 13);

    uint8_t recv_data[64] = {0};

    int len =
        uart_read_bytes(UART_NUM_1, recv_data, 1024, 500 / portTICK_PERIOD_MS);

    if (len > 0) {
        if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
        printf("生成特征成功\n");
        return 0;
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x01) {
        printf("生成特征 ==> 收包错误\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x06) {
        printf("指纹图像太乱而生不成特征\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x07) {
        printf("指纹图像正常，但特征点太少而生不成特征\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x08) {
        printf("当前指纹特征与之前特征之间无关联\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x0A) {
        printf("合并失败\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x15) {
        printf("图像缓冲区内没有有效原始图而生不成图像\n");
        } else if (recv_data[6] == 0x07 && recv_data[9] == 0x28) {
        printf("当前指纹特征与之前特征之间有关联\n");
        }
    }

    printf("获取特征失败\n");
    return 1;
}

// 搜索指纹模板
uint8_t Int_FPM383_Search(void)
{
  uint8_t cmd[17] = {
      0xEF, 0x01,             // 包头
      0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
      0x01,                   // 包标识
      0x00, 0x08,             // 包长度
      0x04,                   // 指令码
      0x01,                   // 缓冲区号
      0x00, 0x00,             // StartPage
      0xFF, 0xFF,             // PageNum
      0x02, 0x0C              // 校验和
  };

  uart_write_bytes(UART_NUM_1, cmd, 17);

  uint8_t recv_data[64] = {0};

  int len =
      uart_read_bytes(UART_NUM_1, recv_data, 1024, 500 / portTICK_PERIOD_MS);

  if (len > 0) {
    if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
      printf("搜索成功\n");
      return 0;
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x01) {
      printf("收包错误\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x09) {
      printf("没搜索到；此时页码与得分为 0\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x17) {
      printf("残留指纹或两次采集之间手指没有移动过\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x31) {
      printf("功能与加密等级不匹配\n");
    }
  }

  printf("搜索失败失败\n");
  return 1;
}

// 合并指纹模板
uint8_t Int_FPM383_RegModel(void)
{
  uint8_t cmd[12] = {
      0xEF, 0x01,             // 包头
      0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
      0x01,                   // 包标识
      0x00, 0x03,             // 包长度
      0x05,                   // 指令码
      0x00, 0x09              // 校验和
  };

  uart_write_bytes(UART_NUM_1, cmd, 12);

  uint8_t recv_data[64] = {0};

  int len =
      uart_read_bytes(UART_NUM_1, recv_data, 1024, 500 / portTICK_PERIOD_MS);

  if (len > 0) {
    if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
      printf("合并成功\n");
      return 0;
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x01) {
      printf("收包错误\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x0A) {
      printf("合并失败\n");
    }
  }

  printf("合并特征失败\n");
  return 1;
}

// 储存指纹模板
uint8_t Int_FPM383_StoreChar(uint8_t pageID)
{
  uint8_t cmd[15] = {
      0xEF, 0x01,             // 包头
      0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
      0x01,                   // 包标识
      0x00, 0x06,             // 包长度
      0x06,                   // 指令码
      0x01,                   // BufferID
      0x00, 0x00,             // PageID
      0x00, 0x00              // 校验和
  };

  cmd[12] = pageID;
  uint16_t sum =
      cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10] + cmd[11] + cmd[12];
  cmd[13] = sum >> 8;
  cmd[14] = sum;

  uart_write_bytes(UART_NUM_1, cmd, 15);

  uint8_t recv_data[64] = {0};

  int len =
      uart_read_bytes(UART_NUM_1, recv_data, 1024, 500 / portTICK_PERIOD_MS);

  if (len > 0) {
    if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
      printf("储存成功\n");
      return 0;
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x01) {
      printf("收包错误\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x0B) {
      printf("PageID 超出指纹库范围\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x18) {
      printf("写 FLASH 出错\n");
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x18) {
      printf("功能与加密等级不匹配\n");
    }
  }

  printf("储存失败\n");
  return 1;
}

// 写系统寄存器
uint8_t Int_FPM383_WriteReg(uint8_t regNum, uint8_t regValue)
{
  uint8_t cmd[14] = {
      0xEF, 0x01,             // 包头
      0xFF, 0xFF, 0xFF, 0xFF, // 默认设备地址
      0x01,                   // 包标识
      0x00, 0x05,             // 包长度
      0x0E,                   // 指令码
      0,                      // 寄存器序号
      0,                      // 内容
      0x00, 0x1B              // 校验和
  };

  cmd[10] = regNum;
  cmd[11] = regValue;
  uint16_t sum = cmd[6] + cmd[7] + cmd[8] + cmd[9] + cmd[10] + cmd[11];

  cmd[12] = sum >> 8;
  cmd[13] = sum;
  uart_write_bytes(UART_NUM_1, cmd, 14);

  uint8_t recv_data[64] = {0};

  int len =
      uart_read_bytes(UART_NUM_1, recv_data, 1024, 100 / portTICK_PERIOD_MS);

  if (len > 0) {
    if (recv_data[6] == 0x07 && recv_data[9] == 0x00) {
      printf("设置成功\n");
      return 0;
    } else if (recv_data[6] == 0x07 && recv_data[9] == 0x01) {
      printf("收包错误\n");
    }
  }

  printf("设置失败\n");
  return 1;
}

// 注册指纹模板
uint8_t Int_FPM383_Enroll(void)
{
    uint8_t n = 1;
SendGetImageCmdLabel:
    if(Int_FPM383_GetImage())
        goto SendGetImageCmdLabel;
    if(Int_FPM383_GenChar(n))
        goto SendGetImageCmdLabel;
    if (n < 4)
    {
        n++;
        goto SendGetImageCmdLabel;
    }
    if(Int_FPM383_RegModel())
        goto Error;

    uint8_t templates_num = Int_FPM383_GetTemplatesNumber();
    if (Int_FPM383_StoreChar(templates_num + 1))
        goto Error;

    Int_FPM383_GetTemplatesNumber();
    return 0;    

Error:
    return 1;
}

// 识别指纹模板
uint8_t Int_FPM383_Identify(void)
{
START_LABEL:
    if(Int_FPM383_GetImage())
        goto START_LABEL;
    if(Int_FPM383_GenChar(1))
        goto START_LABEL;
    return Int_FPM383_Search();
}
