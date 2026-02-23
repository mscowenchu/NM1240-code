# 如何上傳 / 貢獻程式碼 (How to Upload Code)

本指南說明如何將 Keil V6 版本的程式碼上傳到此專案。

---

## 方法一：透過 GitHub 網頁上傳

1. **登入 GitHub** — 開啟 https://github.com 並登入你的帳號
2. **進入本專案** — 前往 repository 頁面
3. **建立新分支 (Branch)**
   - 點擊 `main` 分支下拉選單
   - 輸入新分支名稱（例如 `keilv6-update`）
   - 點擊 "Create branch"
4. **上傳檔案**
   - 點擊 "Add file" → "Upload files"
   - 將修改過的檔案拖曳到頁面中
   - 填寫 commit 訊息（例如 "更新 Keil V6 原始碼"）
   - 點擊 "Commit changes"
5. **建立 Pull Request (PR)**
   - 點擊 "Compare & pull request"
   - 填寫 PR 說明，描述修改的內容
   - 點擊 "Create pull request"

---

## 方法二：透過 Git 命令列上傳

```bash
# 1. 複製 (Clone) 專案
git clone https://github.com/mscowenchu/NM1240-code.git
cd NM1240-code

# 2. 建立新分支
git checkout -b keilv6-update

# 3. 將修改過的檔案複製到專案目錄中
#    例如：將 Keil V6 專案檔覆蓋到對應位置
cp /path/to/your/files/* NM1240_V4_Motor_Example_internal_20211005/SampleCode/UserProj/NuMotor_NM1240/KEIL/program/

# 4. 加入變更
git add .

# 5. 提交 (Commit)
git commit -m "Update to Keil V6 (ARM Compiler 6)"

# 6. 推送 (Push) 到 GitHub
git push origin keilv6-update

# 7. 在 GitHub 網頁上建立 Pull Request
```

---

## 方法三：透過 GitHub Issue 提交程式碼

如果無法直接上傳，可以：

1. 在 GitHub 上建立新的 **Issue**
2. 在 Issue 中描述修改內容
3. 將修改過的程式碼以 **附件** 方式上傳（壓縮為 `.zip` 或 `.7z`）
4. 指定 Copilot 協助 review 和整合

---

## Keil V5 → V6 遷移注意事項

從 Keil V5 (ARM Compiler 5 / armcc) 遷移到 Keil V6 (ARM Compiler 6 / armclang) 時，
需要注意以下幾點：

### 編譯器差異

| 項目 | ARM Compiler 5 (armcc) | ARM Compiler 6 (armclang) |
|------|----------------------|--------------------------|
| 匿名 Union | `#pragma anon_unions` | `-fms-extensions` 或 `#pragma clang diagnostic` |
| 函式級組語 | `__asm int func() { ... }` | `__attribute__((naked))` + `__asm("...")` |
| 診斷抑制 | `#pragma diag_suppress 1296` | `#pragma clang diagnostic ignored "-Wc99-extensions"` |
| 編譯器偵測 | `__CC_ARM` | `__ARMCC_VERSION >= 6010050` |
| Import 指令 | `#pragma import` | 不支援（移除即可） |
| C 內聯組語 | `__cpp(symbol)` | 直接使用 C 符號名稱 |

### 專案檔 (.uvprojx) 設定

```xml
<!-- 將以下欄位更新為 AC6 -->
<pCCUsed>6190000::V6.19::ARMCLANG</pCCUsed>
<uAC6>1</uAC6>
```

### 預編譯函式庫 (.lib)

使用 ARM Compiler 5 編譯的 `.lib` 檔案**無法**直接在 ARM Compiler 6 中使用。
解決方案：
- 使用原始碼 (`.c`) 重新編譯
- 或使用 ARM Compiler 6 重新建置函式庫

---

## 需要協助？

如果在遷移過程中遇到問題，請在 GitHub Issue 中描述錯誤訊息，
我們可以協助 review 和修正程式碼。
