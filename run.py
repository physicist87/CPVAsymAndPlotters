#!/usr/bin/env python3

import subprocess
import os
import sys

EXEC      = "./CalCPVFit"
STUDYNAME = "AN_v6p4"
SYST      = "none"   # "none" or "BGStat"
XSEC      = 831.76
BR        = 0.10706

RUNPERIODS = ["UL2016PreVFP", "UL2016PostVFP", "UL2017", "UL2018"]
CHANNELS   = ["MuMu", "ElEl", "MuEl"]

LUMI = {
    "UL2018":        59832.422397,
    "UL2017":        41477.877399,
    "UL2016PostVFP": 16812.151722,
    "UL2016PreVFP":  19520.0,
}
LUMI_RUN2 = sum(LUMI.values())   # ~137642

# Options
RUN_PER_PERIOD   = True   # per RunPeriod, per Channel
RUN_DILEPTON     = True   # per RunPeriod, Dilepton (MuMu+ElEl+MuEl)
RUN_RUN2_CHANNEL = True   # Run2, per Channel
RUN_RUN2_DILEPTON= True   # Run2, Dilepton


def get_input_list(runperiod, channel):
    return (f"input/{STUDYNAME}/{runperiod}/{channel}/"
            f"{STUDYNAME}_{runperiod}_{channel}.list")


def get_scale_config(runperiod, channel):
    return (f"ScaleConfig/{STUDYNAME}/{runperiod}/{channel}/"
            f"ScaleConfig_{STUDYNAME}_{runperiod}_{channel}.txt")


def run_job(input_lists, scale_configs, channel, runperiod, lumi, label=""):
    if not input_lists:
        print(f"  No valid inputs for {label}, skipping.")
        return False

    input_arg  = ",".join(input_lists)
    scale_arg  = ",".join(scale_configs)

    cmd = [
        EXEC,
        input_arg, scale_arg,
        channel, runperiod, STUDYNAME,
        "--syst", SYST,
        "--lumi", str(lumi),
        "--xsec", str(XSEC),
        "--br",   str(BR),
    ]

    tag = label or f"{runperiod}/{channel}"
    print(f"\n--- {STUDYNAME} / {tag} ---")
    print(f"  {EXEC} {input_arg[:60]}{'...' if len(input_arg)>60 else ''}")

    try:
        result = subprocess.run(cmd, check=True, text=True, capture_output=False)
        return True
    except subprocess.CalledProcessError as e:
        print(f"  Error (exit {e.returncode})")
        return False
    except FileNotFoundError:
        print(f"  Error: '{EXEC}' not found. Run 'make' first.")
        sys.exit(1)


def collect(runperiods, channels):
    lists, scales = [], []
    for rp in runperiods:
        for ch in channels:
            il = get_input_list(rp, ch)
            sc = get_scale_config(rp, ch)
            if not os.path.exists(il):
                print(f"  Warning: {il} not found, skipping.")
                continue
            if not os.path.exists(sc):
                print(f"  Warning: {sc} not found, skipping.")
                continue
            lists.append(il)
            scales.append(sc)
    return lists, scales


print("=" * 60)
print(f"CalCPVFit run script  [{STUDYNAME}]")
print("=" * 60)

# 1. Per RunPeriod, per Channel
if RUN_PER_PERIOD:
    print(f"\n{'='*60}")
    print("[1] Per RunPeriod / Per Channel")
    print(f"{'='*60}")
    for rp in RUNPERIODS:
        for ch in CHANNELS:
            il = get_input_list(rp, ch)
            sc = get_scale_config(rp, ch)
            if not os.path.exists(il):
                print(f"  Warning: {il} not found, skipping.")
                continue
            if not os.path.exists(sc):
                print(f"  Warning: {sc} not found, skipping.")
                continue
            run_job([il], [sc], ch, rp, LUMI[rp])

# 2. Per RunPeriod, Dilepton
if RUN_DILEPTON:
    print(f"\n{'='*60}")
    print("[2] Per RunPeriod / Dilepton")
    print(f"{'='*60}")
    for rp in RUNPERIODS:
        lists, scales = collect([rp], CHANNELS)
        run_job(lists, scales, "Dilepton", rp, LUMI[rp],
                label=f"{rp}/Dilepton")

# 3. Run2, per Channel
if RUN_RUN2_CHANNEL:
    print(f"\n{'='*60}")
    print("[3] Run2 / Per Channel")
    print(f"{'='*60}")
    for ch in CHANNELS:
        lists, scales = collect(RUNPERIODS, [ch])
        run_job(lists, scales, ch, "Run2", LUMI_RUN2,
                label=f"Run2/{ch}")

# 4. Run2, Dilepton
if RUN_RUN2_DILEPTON:
    print(f"\n{'='*60}")
    print("[4] Run2 / Dilepton")
    print(f"{'='*60}")
    lists, scales = collect(RUNPERIODS, CHANNELS)
    run_job(lists, scales, "Dilepton", "Run2", LUMI_RUN2,
            label="Run2/Dilepton")

print(f"\n{'='*60}")
print("All jobs done.")
print(f"{'='*60}")
