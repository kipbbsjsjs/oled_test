# oled_test — ESP32 OLED 3D 立方体动画

毕业设计《基于 ESP32 的洗车工位喷淋控制系统》的 OLED 单元测试程序。
用于验证 0.96 寸 I2C OLED（SSD1306 驱动，128x64，4 针版）的显示与 I2C 通信。

## 功能

- 屏幕中央渲染三轴同时旋转的 3D 线框立方体，带透视效果（近大远小）
- 约 30ms 一帧，millis() 非阻塞刷新，角度取模防止长时间运行精度漂移
- 初始化失败时串口提示逐项排查并在原地停机，不带病运行

## 接线

| OLED | ESP32 |
|------|-------|
| GND | GND |
| VDD | 3V3（禁接 5V） |
| SCK/SCL | D22 |
| SDA | D21 |

I2C 地址默认 `0x3C`（不亮可改 `0x3D`），具体 SDA/SCL 引脚以代码内 `OLED_SDA` / `OLED_SCL` 宏为准（已在硬件上验证点亮）。

原理图：[OLED显示实验.eprj2](OLED显示实验.eprj2)（嘉立创EDA专业版工程文件，SQLite 格式）

## 使用

Arduino IDE 安装 ESP32 板卡包及 Adafruit SSD1306 / Adafruit GFX / Adafruit BusIO 库，
打开烧录即可。屏幕不亮优先检查：VDD 是否误接 5V、SCK/SDA 是否接反、杜邦线是否虚接。
