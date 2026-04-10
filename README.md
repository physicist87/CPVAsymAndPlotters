# CalCPVFit

**CP Violation Likelihood Fitter for ttbar Dilepton Analysis**
**ttbar 다이렙톤 분석용 CP 비대칭 라이클리후드 피터**

Author: Seungkyu Ha (seungkyu.ha@cern.ch)
CMS Collaboration / Korea University

---

## Overview / 개요

CalCPVFit reads ROOT histogram files from CMS NanoAOD-based ttbar dilepton analyses, extracts CP observable counts (N+, N−) from `h_Reco_CPO{N}_ReRange` histograms, and performs a Poisson likelihood fit using TMinuit to extract the CP asymmetry and ttbar cross section for observables O1–O13.

CalCPVFit는 CMS ttbar 다이렙톤 분석의 ROOT 히스토그램 파일을 읽어 `h_Reco_CPO{N}_ReRange` 히스토그램에서 CP 옵저버블의 양수/음수 이벤트 수(N+, N−)를 추출하고, TMinuit을 이용한 푸아송 라이클리후드 피팅으로 O1–O13 각각의 CP 비대칭도와 ttbar 생성 단면적을 산출합니다.

---

## Directory Structure / 디렉토리 구조

```
CalCPVFit/
├── main.cpp                    # Entry point / 진입점
├── Makefile
├── run.py                      # Batch job runner / 배치 실행 스크립트
├── include/
│   ├── ScaleConfigReader.hpp   # ScaleConfig parser / SF 파싱
│   ├── HistLoader.hpp          # ROOT file loader / 히스토그램 로더
│   ├── FitInputs.hpp           # Fit input structs / 피팅 입력 구조체
│   └── CPVFitter.hpp           # TMinuit wrapper / 피터 래퍼
└── src/
    ├── ScaleConfigReader.cpp
    ├── HistLoader.cpp
    └── CPVFitter.cpp

input/
└── {StudyName}/{RunPeriod}/{Channel}/
    └── {StudyName}_{RunPeriod}_{Channel}.list   # ROOT file list

ScaleConfig/
└── {StudyName}/{RunPeriod}/{Channel}/
    └── ScaleConfig_{StudyName}_{RunPeriod}_{Channel}.txt

output/
└── {StudyName}/{RunPeriod}/{Channel}/
    ├── FitInputs_{Channel}_{RunPeriod}.dat      # Fit inputs dump / 입력값 덤프
    └── FitResult_{Channel}_{RunPeriod}.txt      # Fit results / 피팅 결과
```

---

## Build / 빌드

```bash
make
```

Requires ROOT with Minuit. Tested with ROOT 6.
ROOT와 Minuit이 필요합니다.

---

## Usage / 사용법

### Single run / 단일 실행

```bash
./CalCPVFit <input.list> <ScaleConfig.txt> <channel> <runperiod> <studyname> [options]
```

**Options / 옵션:**

| Option | Default | Description |
|--------|---------|-------------|
| `--lumi <pb^-1>` | 59832.4 | Luminosity in pb⁻¹ / 루미노시티 |
| `--xsec <pb>` | 831.76 | ttbar cross section in pb / ttbar 생성 단면적 |
| `--br <value>` | 0.10706 | Dilepton branching fraction / 다이렙톤 분기율 |
| `--syst none\|BGStat` | none | Systematic type / 시스테마틱 타입 |

**Example / 예시:**

```bash
./CalCPVFit \
  input/AN_v6p4/UL2018/MuMu/AN_v6p4_UL2018_MuMu.list \
  ScaleConfig/AN_v6p4/UL2018/MuMu/ScaleConfig_AN_v6p4_UL2018_MuMu.txt \
  MuMu UL2018 AN_v6p4 \
  --lumi 59832.422397 --xsec 831.76 --br 0.10706
```

### Combined channels or periods / 채널·RunPeriod 합산

Comma-separated lists for Dilepton or Run2 combinations.
Dilepton 또는 Run2 합산 시 콤마로 구분하여 입력합니다.

```bash
# Dilepton (UL2018)
./CalCPVFit \
  MuMu.list,ElEl.list,MuEl.list \
  MuMu.txt,ElEl.txt,MuEl.txt \
  Dilepton UL2018 AN_v6p4 --lumi 59832.422397

# Run2 MuMu (all periods)
./CalCPVFit \
  PreVFP.list,PostVFP.list,2017.list,2018.list \
  PreVFP.txt,PostVFP.txt,2017.txt,2018.txt \
  MuMu Run2 AN_v6p4 --lumi 137642.0
```

### Batch run / 배치 실행

```bash
python3 run.py
```

Runs all combinations defined in `run.py`:
`run.py`에 정의된 모든 조합을 순서대로 실행합니다:

1. Per RunPeriod × Per Channel (MuMu, ElEl, MuEl)
2. Per RunPeriod × Dilepton
3. Run2 × Per Channel
4. Run2 × Dilepton

Edit the flags at the top of `run.py` to enable/disable each mode.
`run.py` 상단의 플래그로 각 모드를 켜고 끌 수 있습니다.

```python
RUN_PER_PERIOD    = True
RUN_DILEPTON      = True
RUN_RUN2_CHANNEL  = True
RUN_RUN2_DILEPTON = True
```

---

## ScaleConfig Format / ScaleConfig 포맷

Two formats are supported. DY uses the step-wise format; only the `*` entry (post b-tag) is applied.
두 가지 포맷을 지원합니다. DY는 step-wise 포맷을 사용하며 `*` 엔트리(b-tag 이후 단계)만 실제로 적용됩니다.

```
# Non-DY samples (flat format)
Data          1.0
TTbar_Signal  1.0
WJetsToLNu    1.0
...

# DY step-wise format
DY  0     1.000000
DY  1     0.994256
DY  2     0.994256
DY  3     0.997269
DY  5     1.230428
DY  *     1.230428   # <- this value is used / 이 값만 사용됨
```

---

## Likelihood Function / 라이클리후드 함수

```
N_pre_tt = Luminosity × sel_eff × sigma_tt

N+_pred  = N_pre_tt × (1 + A) / 2  +  N_bkg × f_bkg+
N-_pred  = N_pre_tt × (1 - A) / 2  +  N_bkg × f_bkg-

-ln L = Poisson(N+_obs | N+_pred) + Poisson(N-_obs | N-_pred)
```

Where:
- `A` = CP asymmetry (free parameter / 자유 파라미터)
- `sigma_tt` = ttbar cross section (free parameter / 자유 파라미터)
- `sel_eff` = signal selection efficiency = N_signal / (xsec × BR × lumi)
- `N_bkg`, `f_bkg±` = background total and fractions from MC

Fit uses TMinuit with MIGRAD + MINOS errors.
TMinuit의 MIGRAD로 최소화 후 MINOS로 비대칭 오차를 계산합니다.

**Systematic mode (BGStat):** Background N± are floated with Gaussian constraints.
**BGStat 모드:** 백그라운드 N±를 가우시안 제약과 함께 자유 파라미터로 처리합니다.

---

## Output Format / 출력 포맷

### FitResult_{channel}_{runperiod}.txt

```
# Obs  Asym  ErrPlus  ErrMinus  Sigma_tt  SigErrP  SigErrM  FitMin  Status
O1   0.001411   0.002171  -0.002173   85.566  0.185908  -0.185759  ...  3
O2  -0.005767   0.002172  -0.002171   85.566  ...
...
```

Fit status 3 = MINOS successful / 정상 수렴

### FitInputs_{channel}_{runperiod}.dat

Per-observable summary + per-sample N+/N− breakdown for debugging.
옵저버블별 요약 및 샘플별 N+/N− 내역 (디버깅용).

---

## Notes / 주의사항

- Signal sample name must be `TTbar_Signal` in ROOT files.
  ROOT 파일 내 시그널 샘플 이름은 반드시 `TTbar_Signal`이어야 합니다.
- CP observable histograms must follow the naming pattern `h_Reco_CPO{N}_ReRange`.
  CP 옵저버블 히스토그램 이름은 `h_Reco_CPO{N}_ReRange` 패턴을 따라야 합니다.
- N+ / N− are computed by integrating positive / negative halves of the histogram.
  N+/N−는 히스토그램의 양수/음수 영역을 적분하여 계산합니다.
- `{include,src}/` directory (if present) is a tarball artifact and can be safely deleted.
  `{include,src}/` 디렉토리가 있다면 tarball 해제 과정의 부산물이므로 삭제해도 됩니다.

```bash
rm -rf '{include,src}/'
```
