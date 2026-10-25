/*
 * ESP32 0.96寸 I2C OLED 显示实验（SSD1306驱动，128x64，4针版）
 * 现象：屏幕中央显示3D旋转立方体动画，三个轴同时旋转，带透视效果
 * 接线：GND→GND  VDD→3V3  SCK(SCL)→D22  SDA→D21
 * 使用ESP32默认I2C引脚，SCL/SDA固定在D22/D21
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

#define OLED_ADDR       0x3C    // 0.96寸SSD1306常用地址，若不亮可改0x3D
#define OLED_SDA        22      // SCK(SCL) → D21
#define OLED_SCL        21     // SDA → D22

// I2C版构造函数：参数为 (宽, 高, &Wire, 复位脚-1表示不用)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 立方体8个顶点（原始坐标，中心为原点）
const float VERTICES[8][3] = {
  {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
  {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}
};

// 12条棱（顶点索引对）
const int EDGES[12][2] = {
  {0, 1}, {1, 2}, {2, 3}, {3, 0},    // 顶面
  {4, 5}, {5, 6}, {6, 7}, {7, 4},    // 底面
  {0, 4}, {1, 5}, {2, 6}, {3, 7}     // 竖边
};

unsigned long lastFrame = 0;
float angleX = 0, angleY = 0, angleZ = 0;
const float TAU = 6.2832f;              // 2π，角度超一圈就取模，防止长跑精度漂移

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED初始化失败！逐项检查：VDD接3V3、SCK(SCL)接D22、SDA接D21");
    for (;;);                   // 停在这里，不让程序带病运行
  }
  Serial.println("OLED初始化成功");
}

void loop() {
  unsigned long now = millis();
  if (now - lastFrame < 30) return;     // 约30ms一帧，非阻塞刷新

  lastFrame = now;

  // 预计算三轴旋转的正余弦
  float cx = cosf(angleX), sx = sinf(angleX);
  float cy = cosf(angleY), sy = sinf(angleY);
  float cz = cosf(angleZ), sz = sinf(angleZ);

  const float SCALE = 32.0;             // 投影放大倍数
  const float DIST  = 4.0;              // 视距，越小透视越强
  int px[8], py[8];

  for (int i = 0; i < 8; i++) {
    float x = VERTICES[i][0], y = VERTICES[i][1], z = VERTICES[i][2];

    // 绕X轴旋转
    float y1 = y * cx - z * sx;
    float z1 = y * sx + z * cx;
    // 绕Y轴旋转
    float x2 = x * cy + z1 * sy;
    float z2 = -x * sy + z1 * cy;
    // 绕Z轴旋转
    float x3 = x2 * cz - y1 * sz;
    float y3 = x2 * sz + y1 * cz;

    // 透视投影到屏幕坐标（近大远小）
    float p = SCALE / (DIST + z2);
    px[i] = (int)(SCREEN_WIDTH / 2 + x3 * p);
    py[i] = (int)(SCREEN_HEIGHT / 2 + y3 * p);
  }

  display.clearDisplay();
  for (int e = 0; e < 12; e++) {
    display.drawLine(px[EDGES[e][0]], py[EDGES[e][0]],
                     px[EDGES[e][1]], py[EDGES[e][1]], SSD1306_WHITE);
  }
  display.display();

  // 三轴不同速率旋转，产生翻滚效果
  angleX = fmodf(angleX + 0.10f, TAU);
  angleY = fmodf(angleY + 0.14f, TAU);
  angleZ = fmodf(angleZ + 0.06f, TAU);
}
