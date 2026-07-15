# Accelerating Statistical Reconstruction in Cosmic Muon Tomography via O(1) Edge-Optimized Convolutional Networks

This repository contains the complete, reproducible source code for the computational physics method presented in:

> **"Accelerating Statistical Reconstruction in Cosmic Muon Tomography via O(1) Edge-Optimized Convolutional Networks"**
> Submitted to *Computer Physics Communications*

It provides the tools to simulate muon trajectories in Geant4, reconstruct densities using classical Maximum Likelihood Expectation Maximization (MLEM), and run the edge-optimized feed-forward Convolutional Neural Network (Edge-CNN) to solve the tomographic inverse problem in milliseconds.

---

## Repository Structure

```
├── README.md
├── requirements.txt            # Python dependencies
├── .gitignore
├── data/                       # POCA CSV files (G1–G7, 50k muons each)
│   └── raw/                    # Raw Geant4 MuonData CSV files (for MLEM)
├── output/                     # Generated model weights, plots, and voxel data
├── scripts/
│   ├── train_cnn.py            # Edge-CNN training (BCE loss, lr=0.005, seed=42)
│   ├── mlem_tracker.py         # 20-iteration quadratic MLEM on 25×25×10 grid
│   └── generate_metric_plots.py # ROC, SNR, and inference latency evaluation
├── src/
│   └── DetectorConstruction.cc # Geant4 detector geometry (G1–G7 configs)
└── include/
    └── DetectorConstruction.hh # Geant4 header
```

---

## 1. Prerequisites & Environment Setup

### Python (3.8+)
```bash
git clone https://github.com/aftabazad502-pixel/edge-cnn-muon-tomography.git
cd edge-cnn-muon-tomography
pip install -r requirements.txt
```

**Important:** All scripts must be run from the repository root directory (not from inside `scripts/`).

### Geant4 (for data generation only)
Compile the Geant4 application using standard CMake. Set `GEOM_ID` environment variable (1–7) to select geometry.

---

## 2. Step-by-Step Reproduction Workflow

### Step 1: Generate Geant4 Data (or use provided CSV)
```bash
export GEOM_ID=1
./muon_simulation -m run.mac
```
Place the output CSVs into `data/raw/` (for MLEM) and the processed POCA CSVs into `data/`.

### Step 2: Run MLEM Reconstruction (Baseline)
```bash
python scripts/mlem_tracker.py --data_dir data/raw/ --output_dir output/
```
Produces `output/mlem_voxels.npy` and `output/mlem_reconstruction.png`.

### Step 3: Train the Edge-CNN
```bash
python scripts/train_cnn.py --data_dir data/ --output_dir output/ --epochs 500
```
Produces `output/edge_cnn.pth` (model weights) and `output/cnn_stats.npy`.

### Step 4: Generate Evaluation Plots
```bash
python scripts/generate_metric_plots.py --data_dir data/ --output_dir output/
```
Produces `output/roc_curve.png`, `output/snr_comparison.png`, and `output/inference_time.png`.

---

## 3. Training Configuration

| Parameter | Value |
|---|---|
| Loss Function | Binary Cross-Entropy (BCE) |
| Optimizer | Adam |
| Learning Rate | 0.005 |
| Epochs | 500 |
| Random Seed | 42 (provided for future reproducibility) |
| MLEM Grid | 25 × 25 × 10 voxels |
| MLEM Iterations | 20 |

**Note:** The original training run reported in the manuscript did not use a fixed random seed. The seed (42) is provided in the code to enable deterministic reproduction for reviewers and future users. Results may vary slightly from the manuscript due to stochastic initialization differences.

---

## 4. Zero-Shot Test Protocol

- **Training set:** G1 (Baseline), G2 (Micro-Void), G4 (Off-Center), G5 (Solid), G7 (Rebar)
- **Test set (withheld):** G3 (Double Void), G6 (Micro-Crack)

G3 and G6 were never exposed to the network during training.

---

## Citation
```bibtex
@article{muon_edge_cnn_2026,
  title={Accelerating Statistical Reconstruction in Cosmic Muon Tomography via O(1) Edge-Optimized Convolutional Networks},
  author={[Your Name]},
  journal={Computer Physics Communications},
  year={2026}
}
```

## License
This code is released for academic and research use. See LICENSE for details.
