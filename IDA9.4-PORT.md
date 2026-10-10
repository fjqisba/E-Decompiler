# IDA 9.4 迁移说明 (7.5 → 9.4)

> 本分支将 E-Decompiler 插件从 IDA 7.5 迁移到 IDA 9.4。
> 迁移过程中的关键发现记录如下, 供后续维护参考。

## 构建环境

| 组件 | 说明 |
|---|---|
| IDA SDK 9.4 | GitHub: [HexRaysSA/ida-sdk](https://github.com/HexRaysSA/ida-sdk) (已开源, MIT) |
| 编译器 | MSVC v143 (VS2022 BuildTools 即可) |
| Windows SDK | 10.0.26100.0 (其他 10.x 亦可) |
| Qt | **无需安装** (见下文 Qt 部分) |

## 构建方式

旧 VS2019 vcxproj + Qt VsTools 工程已废弃, 改用 SDK 自带 CMake:

```powershell
cd E-Decompiler
cmake -S . -B build -DIDASDK="<IDA SDK 根目录>"   # SDK: github.com/HexRaysSA/ida-sdk (含 src/ 的 git 形态)
cmake --build build --config Release
```

产物自动输出为 `E-Decompiler.dll`, 复制到 `<IDA>\plugins\` 即可。

## 主要 API 变化 (7.5 → 9.4)

| 变化 | 处理 |
|---|---|
| `bin_search2` 删除 | 改为 `bin_search(start, end, compiled_binpat_vec_t&, flags)`, 共 4 处 |
| 旧 structs API 删除 (`add_struc`/`get_struc`/`add_struc_member`) | 虚表结构改用 til/udt API: `udt_type_data_t` + `udm_t` + `set_named_type(NTF_TYPE)` + `set_vftable()` |
| `enumerate_files2` → `enumerate_files` | 新签名带 answer 缓冲参数 |
| `plugin_t` / `action_desc_t` / `parse_decls` | **未变**, 零改动 |
| `hexdsp_t` / Hex-Rays 插件机制 | 不变, 仍为 `init_hexrays_plugin()` 路线 |

## HEXRAYS_API_MAGIC 版本匹配 (重要!)

`init_hexrays_plugin()` 通过 `callui(ui_broadcast, HEXRAYS_API_MAGIC, ...)`
与 hexrays 引擎握手, **魔数尾数即 hexrays API 版本**:

- IDA 9.4.0 正式版引擎 (2026-07 之后): `0x00DEC0DE00000005`
- 更早的 9.4 build (如 260610, 2026-06-10): `0x00DEC0DE00000004`

**用 SDK 头文件编译的插件, 魔数必须与装机引擎匹配, 否则握手被拒、
插件初始化失败** (出厂插件不受影响, 因其与引擎配套编译)。

本仓库 `E-Decompiler/CMakeLists.txt` 通过 `EDEC_HEXRAYS_API_VERSION` 变量
注入魔数, 默认 5 (匹配 9.4.0 正式版); 引擎较旧时传 `-DEDEC_HEXRAYS_API_VERSION=4`。

> 选 SDK 头文件/魔数以装机引擎的构建日期为准, 不要盲目用 GitHub 最新。

## 中文编码 (重要!)

SDK 强制 `/utf-8` (源码与执行字符集均为 UTF-8), 而旧代码假定执行字符集为
GBK (9.4 原生 UTF-8, **不再需要 7.5 时代的中文函数名补丁**)。因此:

1. 全部源码由 GBK 转为 UTF-8 (BOM)
2. 编码策略统一为 **"解析层一次转换, 存储/显示全 UTF-8 直通"**:
   - 二进制/特征库中的 GBK 名称在解析时用 `acp_utf8` 转换 (ESymbol.cpp)
   - 字面量本身已是 UTF-8, 不再经 `LocalCpToUtf8/getUTF8String` 二次转换
   - `IDAWrapper::setFuncName` 去除 acp_utf8 (输入已为 UTF-8)
3. 混淆案例: 事件名 = 控件名(GBK) + 事件字面量(UTF-8) 拼接后再按 GBK
   整体转换 → 必然乱码。统一存储编码后自然消失。

## Qt 模块

ControlInfoWidget / PropertyDelegate 为独立 Qt 属性查看窗口, 与其余模块
零耦合。9.4 已改用 Qt6, 若需启用请安装 Qt6 并以 `TYPE QT` 构建; 本迁移
暂将其排除出构建, 核心分析功能不受影响。

## 其他修复

- `common/public.h` 缺失: `UCharToStr` 已在 Utils/Common.h, `getUTF8String` 移入 Utils/Strings.h
- 源码统一转 UTF-8 BOM (SDK /utf-8 下 GBK 源码会触发 C2001)
- ECSigParser::GetFunctionMD5 未声明变量补全 (历史遗留)

## 验证

以易语言 5.95 静态编译样本实测: 插件加载、库函数特征码识别 (中文名)、
控件/事件解析与命名 (43 事件全中正常)、krnln 跳转表、虚表命名、
Hex-Rays 反编译修正钩子均正常工作。
