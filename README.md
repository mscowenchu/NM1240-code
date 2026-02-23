# NM1240 Motor Control Demo Code

NM1240 馬達控制範例程式碼 — 基於 Nuvoton NM1240 MCU 的 FOC / 六步方波 (Six-Step) 馬達控制範例。

## 目錄

- [專案概述](#專案概述)
- [目錄結構](#目錄結構)
- [系統架構](#系統架構)
- [模組說明](#模組說明)
- [NM1240 周邊使用對照表](#nm1240-周邊使用對照表)
- [設定與參數調整](#設定與參數調整)
- [建置方式 (Build)](#建置方式-build)
- [如何新增功能 (Step-by-Step)](#如何新增功能-step-by-step)
- [常見功能擴充範例](#常見功能擴充範例)
- [參考文件](#參考文件)

---

## 專案概述

本範例程式展示如何使用 **Nuvoton NM1240 MCU** 控制風扇馬達，支援兩種控制模式：

| 控制模式 | 說明 |
|---------|------|
| **FOC (Field-Oriented Control)** | 磁場導向控制，支援 1-shunt (1R) 及 2-shunt (2R) 電流偵測 |
| **Six-Step (六步方波控制)** | 支援開迴路 (Open Loop) 及閉迴路 (Close Loop) |

主要特色：
- 三相 Hall 感測器回授
- PI 電流/速度控制迴路
- SVPWM 空間向量脈寬調變
- UART 通訊協議 (可連接 NuMotor GUI 工具)
- DataFlash 參數儲存
- 硬體過電流保護 (ACMP)
- 溫度感測

---

## 目錄結構

```
NM1240_V4_Motor_Example_internal_20211005/
├── 1_Document_for_Demo_Code/          # 文件資料
│   ├── Demo_Board_Circuit/            # Demo 板電路圖
│   ├── Demo_Board_Document/           # Demo 板說明文件、NuMotor 使用手冊
│   └── NM1240 TRM/                    # NM1240 技術參考手冊 (TRM)
│
├── Library/                           # 函式庫
│   ├── CMSIS/                         # ARM CMSIS 核心庫
│   ├── Device/Nuvoton/NM1240/         # NM1240 裝置定義 (NM1240.h, system_NM1240.c)
│   └── StdDriver/                     # NM1240 標準驅動程式
│       ├── inc/                        # 標準驅動標頭檔
│       │   ├── acmp.h                  # 類比比較器
│       │   ├── adc.h                   # 類比數位轉換器
│       │   ├── bpwm.h                  # 基本 PWM
│       │   ├── clk.h                   # 系統時脈
│       │   ├── ecap.h                  # 增強型捕獲計時器
│       │   ├── epwm.h                  # 增強型 PWM
│       │   ├── fmc.h                   # Flash 記憶體控制器
│       │   ├── gdma.h                  # 通用 DMA
│       │   ├── gpio.h                  # 通用輸入輸出
│       │   ├── hdiv.h                  # 硬體除法器
│       │   ├── op.h                    # 運算放大器
│       │   ├── sys.h                   # 系統控制
│       │   ├── timer.h                 # 計時器
│       │   ├── usci_i2c.h              # USCI I2C
│       │   ├── usci_spi.h              # USCI SPI
│       │   ├── usci_uart.h             # USCI UART
│       │   └── wdt.h                   # 看門狗計時器
│       └── src/                        # 標準驅動原始碼
│
└── SampleCode/
    ├── Hard_Fault_Sample/              # Hard Fault 範例
    ├── Template/                       # 基本範本專案
    └── UserProj/NuMotor_NM1240/KEIL/   # ★ 馬達控制主程式 ★
        ├── NM1240_1R_2R_6stp_Hall_FOC.uvprojx   # Keil 專案檔
        └── program/                    # 原始碼目錄
            ├── main.c                  # 主程式進入點
            ├── system_initialize.C     # 系統初始化 (所有周邊設定)
            ├── system_initialize.H     # 系統初始化標頭
            ├── system_parameter.H      # ★ 系統參數設定 (FOC/六步選擇、PWM 頻率等)
            ├── variable_typedefine.h   # 馬達變數結構定義 (AMotor)
            ├── variable_initialize.c   # 變數初始值設定
            ├── INT_ISR.C               # ★ 中斷服務常式 (PWM/Timer/ECAP/ADC)
            ├── motor_FOC.c/.h          # FOC 演算法 (Clark/Park 轉換)
            ├── motor_funtions.c        # 馬達通用功能函式
            ├── motor_functions.h       # 馬達通用功能標頭
            ├── motor_six_step.c/.h     # 六步方波控制
            ├── PI_control.c/.h         # PI 控制器
            ├── svpwm.c/.h              # SVPWM 空間向量脈寬調變
            ├── svpwm_v02.lib           # SVPWM 函式庫 (預編譯)
            ├── protocol.c/.h           # UART 通訊協議 (NuMotor GUI)
            ├── DataFlash.c/.h          # DataFlash 讀寫
            └── temp.c                  # 溫度偵測相關
```

---

## 系統架構

```
┌─────────────────────────────────────────────────────────┐
│                     main.c (主迴圈)                       │
│  1. 系統初始化 → 2. 等待命令 → 3. 錯誤處理               │
└───────────────┬─────────────────────────────────────────┘
                │
    ┌───────────┴───────────────┐
    │   Timer0 中斷 (1ms)       │
    │   TMR0_IRQHandler         │
    │   ├── 速度 PI 控制 (1ms)   │
    │   ├── 讀取 VSP/UART (10ms) │
    │   └── LED 閃爍 (500ms)    │
    └───────────┬───────────────┘
                │
    ┌───────────┴───────────────┐
    │   EPWM 中斷 (每個 PWM 週期) │
    │   BRAKE0_IRQHandler /      │
    │   EPWM_IRQHandler          │
    │   ├── 讀取相電流 (ADC)      │
    │   ├── Clark/Park 轉換      │
    │   ├── Id/Iq PI 控制        │
    │   ├── 反Park/反Clark 轉換  │
    │   ├── SVPWM 計算           │
    │   └── 更新 PWM 佔空比      │
    └───────────┬───────────────┘
                │
    ┌───────────┴───────────────┐
    │   ECAP 中斷               │
    │   ECAP_IRQHandler         │
    │   ├── 偵測 Hall 邊緣       │
    │   ├── 計算轉子角度         │
    │   └── 估算轉子速度         │
    └───────────────────────────┘
```

### 控制流程 (FOC 模式)

```
Hall Sensor → ECAP (角度/速度估算)
                    ↓
Speed Command → Speed PI → Iq Command
                              ↓
              Iq PI → Vq ─┐
              Id PI → Vd ─┤
                          ↓
                 Inverse Park Transform
                          ↓
                 Inverse Clark Transform
                          ↓
                   SVPWM → PWM Duty
                          ↓
                  EPWM → 6-channel Gate Drive → Motor
```

---

## 模組說明

### 1. `system_parameter.H` — 系統參數設定

此檔案是新增功能時最常修改的檔案，包含：

| 參數 | 說明 | 預設值 |
|------|------|--------|
| `P_FOC_CONTROL` | 啟用 FOC 控制模式 | `1` |
| `P_SIX_STEP_CONTROL` | 啟用六步方波控制 | `0` |
| `P_1R_FOC` / `P_2R_FOC` | 選擇 1-shunt 或 2-shunt | `P_1R_FOC` |
| `SYSTEM_CLOCK` | MCU 系統時脈 (Hz) | `60000000` |
| `C_MOTOR_PWM_FREQ` | PWM 頻率 (Hz) | `15000` |
| `C_Tdead` | 死區時間 (μs) | `1` |
| `C_MOTOR_POLE` | 馬達極數 | `4` |
| `C_MAX_MOTOR_SPEED` | 最大轉速 (rpm) | `3000` |
| `C_I_Kp` / `C_I_Ki` | 電流迴路 PI 參數 | `500` / `50` |
| `C_SP_Kp` / `C_SP_Ki` | 速度迴路 PI 參數 | 見檔案 |

### 2. `variable_typedefine.h` — 馬達變數結構

所有馬達相關變數都集中在 `AMotor` 結構中：

```c
AMotor MotorA;  // 全域馬達變數

MotorA.spec   // AMotorSpec       - 馬達規格 (極數、PWM 頻率、限流值等)
MotorA.info   // AMotorInfo       - 即時回授資訊 (Iu/Iv/Iw, Id/Iq, 速度, 角度)
MotorA.cmd    // AMotorCommand    - 控制命令 (Vd/Vq, 速度目標, 方向)
MotorA.ctrl   // AMotorController - 控制器參數 (PI 限幅值)
MotorA.sym    // AMotorSym        - 錯誤旗標
MotorA.other  // AMotorOther      - 其他狀態 (運行狀態、ECAP 值等)
```

### 3. `INT_ISR.C` — 中斷服務常式

核心控制邏輯都在中斷中執行：

| 中斷 | 週期 | 功能 |
|------|------|------|
| `TMR0_IRQHandler` | 1ms | 速度 PI 控制、讀取命令 |
| `EPWM_IRQHandler` | 每個 PWM 週期 (~66μs @15kHz) | 電流迴路 PI、FOC 運算、更新 PWM |
| `ECAP_IRQHandler` | Hall 邊緣觸發 | 角度更新、速度估算 |
| `BRAKE0_IRQHandler` | 過電流觸發 | 緊急停機 |

### 4. `motor_FOC.c/.h` — FOC 演算法

- `Iuvw_to_Idq()` — Clark + Park 轉換 (三相電流 → d-q 軸電流)
- `Vdq_to_SVPWM_2R()` — 反 Park + 反 Clark + SVPWM (2-shunt)
- `Vdq_to_SVPWM_1R()` — 反 Park + 反 Clark + SVPWM (1-shunt)
- `Re_Constuct_3_Phase_Current_2R()` — ADC 值重建三相電流 (2R)
- `Re_Constuct_3_Phase_Current_1R()` — ADC 值重建三相電流 (1R)

### 5. `PI_control.c/.h` — PI 控制器

- `PI_Iq_current()` — Iq 電流 PI 控制
- `PI_Id_current()` — Id 電流 PI 控制
- `PI_speed()` — 速度 PI 控制
- `PI_controller()` — 通用 PI 控制器

### 6. `protocol.c/.h` — UART 通訊協議

支援與 PC 端 NuMotor GUI 工具通訊：
- 即時波形顯示
- 參數線上調整
- DataFlash 參數讀寫

---

## NM1240 周邊使用對照表

| 周邊模組 | 用途 | 相關 Pin | 初始化函式 |
|---------|------|---------|-----------|
| **EPWM** | 6 路 PWM 輸出驅動 MOS | PA0~PA5 | `Initialize_EPWM()` |
| **ADC0/ADC1** | 相電流偵測 (Iu, Iv)、VDC、VSP 讀取 | 多腳位 | `Initialize_ADC()` |
| **ECAP** | Hall 感測器邊緣捕獲 (速度/角度估算) | PF0~PF2 (IC0~IC2) | `Initialize_ECAP()` |
| **Timer0** | 1ms 系統計時 (速度迴路、命令讀取) | — | `Initialize_Timer0()` |
| **ACMP0** | 硬體過電流比較 | — | `Initialize_ACMP0()` |
| **OP1** | 運算放大器 (DC shunt 電流放大) | — | `Initialize_OP1()` |
| **USCI2 (UART)** | UART 通訊 (NuMotor GUI) | PD7 (TX), PE0 (RX) | `Initialize_USCI_UART()` |
| **USCI1 (SPI)** | 外部 DAC 控制 (選配) | PE5, PE7 | `Initialize_USCI1_SPI1_for_DAC()` |
| **FMC** | DataFlash 讀寫 (512 bytes) | — | `Initialize_FMC()` |
| **HDIV** | 32-bit 硬體除法器 | — | `Initialize_HDIV()` |
| **GPIO** | LED (PF4)、方向開關 (PA6)、測試點 | PF4, PA6, PC4 | `Initialize_GPIO()` |
| **GDMA** | UART DMA 傳輸 | — | `Initialize_GDMA()` |

### 尚未使用的周邊 (可用於擴充功能)

| 周邊模組 | 潛在用途 |
|---------|---------|
| **BPWM** | 額外 PWM 輸出 (如風扇控制、LED 調光) |
| **WDT** | 看門狗計時器 (系統安全) |
| **USCI I2C** | I2C 通訊 (外部感測器) |
| **其他 Timer** | 額外計時功能 |
| **其他 GPIO** | 額外 I/O 控制 |

---

## 設定與參數調整

### 切換控制模式

在 `system_parameter.H` 中修改：

```c
// 選擇 FOC 或 Six-Step（二選一）
#define P_FOC_CONTROL             1   // 設 1 啟用 FOC
#define P_SIX_STEP_CONTROL        0   // 設 1 啟用 Six-Step

// FOC 模式下選擇 1R 或 2R（二選一）
#define P_1R_FOC                      // 取消註解 = 使用 1-shunt
//#define P_2R_FOC                    // 取消註解 = 使用 2-shunt
```

### 調整馬達參數

在 `system_parameter.H` 中修改對應參數：

```c
#define C_MOTOR_POLE          4       // 馬達極數
#define C_MAX_MOTOR_SPEED     3000    // 最大轉速 (rpm)
#define C_MOTOR_PWM_FREQ      15000   // PWM 頻率 (Hz)
#define C_Tdead               1       // 死區時間 (μs)
#define SHUNT_R_mOhm          100     // Shunt 電阻 (mΩ)
#define OP_Amp                5       // 運算放大器增益倍數
```

### 調整 PI 控制參數

```c
// 電流迴路
#define C_I_Kp    (5000)/10     // 電流 Kp
#define C_I_Ki    (1000)/20     // 電流 Ki

// 速度迴路
#define C_SP_Kp   983040/8/2    // 速度 Kp
#define C_SP_Ki   3276/10*2/2   // 速度 Ki
```

---

## 建置方式 (Build)

### 需要的工具

- **Keil MDK-ARM** (μVision 5 或更新版本，建議使用 ARM Compiler 6)
- **Nu-Link Driver** (Nuvoton 燒錄/除錯工具)

### 建置步驟

1. 用 Keil μVision 開啟專案檔：
   ```
   SampleCode/UserProj/NuMotor_NM1240/KEIL/NM1240_1R_2R_6stp_Hall_FOC.uvprojx
   ```
2. 選擇 Build Target（預設已設定為 ARM Compiler 6）
3. 點擊 **Build** (F7) 編譯
4. 連接 Nu-Link，點擊 **Download** (F8) 燒錄

> **注意：** 本專案已更新為支援 ARM Compiler 6 (armclang)。若需使用 ARM Compiler 5，請在 Project Options → Target 中切換編譯器版本。

---

## 如何新增功能 (Step-by-Step)

以下是在此範例程式碼基礎上新增功能的通用步驟：

### Step 1：確認需要使用哪些 NM1240 周邊

查看 [NM1240 周邊使用對照表](#nm1240-周邊使用對照表)，確認：
- 需要使用的周邊是否已被使用
- 可用的 GPIO 腳位（參考 TRM 中的腳位多工表）

### Step 2：在 `system_parameter.H` 新增功能開關

```c
// 範例：新增功能的條件編譯開關
#define P_MY_NEW_FEATURE    1    // 設 1 啟用新功能
```

### Step 3：在 `system_initialize.C` 中新增周邊初始化

```c
// 在 system_initialize.C 中新增初始化函式
void Initialize_MyPeripheral(void)
{
    // 1. 啟用模組時脈
    CLK_EnableModuleClock(xxx_MODULE);

    // 2. 設定腳位多工功能 (MFP)
    SYS->GPx_MFP = ...;

    // 3. 設定周邊暫存器
    // 參考 TRM 對應章節
}
```

並在 `system_initialize.H` 中加入函式宣告：

```c
extern void Initialize_MyPeripheral(void);
```

### Step 4：在 `main.c` 中呼叫初始化

```c
// 在 main() 的初始化區段中加入
#if P_MY_NEW_FEATURE
    Initialize_MyPeripheral();
#endif
```

### Step 5：實作功能邏輯

根據功能需求，在適當的位置加入邏輯：

- **需要週期性執行** → 加入 `INT_ISR.C` 中的 Timer0 中斷 (1ms/10ms/100ms)
- **需要即時控制** → 加入 `INT_ISR.C` 中的 EPWM 中斷
- **新的控制演算法** → 新增獨立的 `.c/.h` 檔案
- **新的通訊功能** → 修改 `protocol.c/.h`
- **需要儲存參數** → 修改 `DataFlash.c/.h`

### Step 6：更新變數結構 (如需要)

在 `variable_typedefine.h` 中為 `AMotor` 結構新增欄位：

```c
typedef struct tag_MotorOther
{
    // ... 現有欄位 ...
    
    // 新增欄位
    int16_t i16_my_new_variable;
} AMotorOther;
```

在 `variable_initialize.c` 中設定初始值。

### Step 7：編譯並測試

1. 在 Keil 中編譯，確認無錯誤
2. 使用 NuMotor GUI 或示波器驗證功能

---

## 常見功能擴充範例

### 範例 1：新增 WDT (看門狗) 保護

**目的：** 防止程式當機導致馬達失控

**步驟：**

1. 在 `system_initialize.C` 新增：
   ```c
   void Initialize_WDT(void)
   {
       CLK_EnableModuleClock(WDT_MODULE);
       CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LIRC, 0);
       WDT_Open(WDT_TIMEOUT_2POW16, WDT_RESET_DELAY_18CLK, TRUE, FALSE);
       WDT_EnableInt();
       NVIC_EnableIRQ(WDT_IRQn);
   }
   ```
2. 在 `main.c` 呼叫 `Initialize_WDT()`
3. 在 Timer0 中斷中定期餵狗：`WDT_RESET_COUNTER()`

### 範例 2：新增 I2C 感測器讀取

**目的：** 透過 I2C 讀取外部感測器

**步驟：**

1. 在 `system_parameter.H` 新增開關：
   ```c
   #define P_ENABLE_I2C_SENSOR    1
   ```
2. 在 `system_initialize.C` 新增 USCI I2C 初始化
3. 新增 `sensor.c/.h` 實作感測器讀取邏輯
4. 在 Timer0 的 100ms 迴圈中讀取感測器資料

### 範例 3：新增 DC Bus 過電壓/欠電壓保護

**目的：** 偵測直流母線電壓異常

**步驟：**

1. 在 `system_parameter.H` 定義電壓門檻值：
   ```c
   #define C_DCBUS_OV_THRESHOLD   30    // 過電壓門檻 (V)
   #define C_DCBUS_UV_THRESHOLD   18    // 欠電壓門檻 (V)
   ```
2. 在 Timer0 的 10ms 迴圈中讀取 VDC Bus ADC 值
3. 比較門檻，觸發保護動作 (`Stop_Motor_PWMOUT_OFF()`)
4. 在 `MotorA.other.u8_flag_error_record` 新增錯誤旗標

---

## 參考文件

所有文件位於 `1_Document_for_Demo_Code/` 目錄：

| 文件 | 說明 |
|------|------|
| `NM1240 TRM/TRM_NM1240_V01_20201208.pdf` | NM1240 技術參考手冊 (TRM) — 所有周邊暫存器詳細說明 |
| `Demo_Board_Document/NuMotor User's Guide_EN_20210527.pdf` | NuMotor 使用手冊 (英文) |
| `Demo_Board_Document/NuMotor User's Guide_CN_20210527.pdf` | NuMotor 使用手冊 (中文) |
| `Demo_Board_Document/Motor Basic Demo System_FOC_2R_1R_&Six_step 20210512.pdf` | FOC/六步控制系統說明 |
| `Demo_Board_Document/Motor Demo System Introduction 20210518.pdf` | 馬達 Demo 系統介紹 |
| `Demo_Board_Circuit/Nu-MDA-NM1240_V1.0_20200609.pdf` | Demo 板電路圖 |

---

## 授權

Copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
